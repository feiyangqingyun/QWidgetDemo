#include "framelessdialog.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qdebug.h"

#ifdef Q_OS_WIN
#include "windows.h"
#include "windowsx.h"
#pragma comment (lib,"user32.lib")
#endif

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

FramelessDialog::FramelessDialog(QWidget *parent) : QDialog(parent)
{
    padding = 8;
    moveEnable = true;
    resizeEnable = true;

    mousePressed = false;
    mousePoint = QPoint(0, 0);
    mouseRect = QRect(0, 0, 0, 0);

    for (int i = 0; i < 8; ++i) {
        pressedArea << false;
        pressedRect << QRect(0, 0, 0, 0);
    }

    isMin = false;
    flags = this->windowFlags();
    titleBar = 0;

    //设置背景透明 官方在5.3以后才彻底修复 WA_TranslucentBackground+FramelessWindowHint 并存不绘制的bug
#if (QT_VERSION >= QT_VERSION_CHECK(5,3,0))
    this->setAttribute(Qt::WA_TranslucentBackground);
#endif
    //设置无边框属性
    this->setWindowFlags(flags | Qt::FramelessWindowHint);
    //安装事件过滤器识别拖动
    this->installEventFilter(this);

    //设置属性产生win窗体效果,移动到边缘半屏或者最大化等
    //设置以后会产生标题栏需要在下面拦截消息重新去掉
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
#endif
}

void FramelessDialog::showEvent(QShowEvent *event)
{
    //解决有时候窗体重新显示的时候假死不刷新的bug
    setAttribute(Qt::WA_Mapped);
    QDialog::showEvent(event);
}

void FramelessDialog::doWindowStateChange(QEvent *event)
{
    //非最大化才能移动和拖动大小
    if (windowState() == Qt::WindowNoState) {
        moveEnable = true;
        resizeEnable = true;
    } else {
        moveEnable = false;
        resizeEnable = false;
    }

    //发出最大化最小化等改变事件,以便界面上更改对应的信息比如右上角图标和文字
    emit windowStateChange(!moveEnable);

    //解决mac系统上无边框最小化失效的bug
#ifdef Q_OS_MACOS
    if (windowState() & Qt::WindowMinimized) {
        isMin = true;
    } else {
        if (isMin) {
            //设置无边框属性
            this->setWindowFlags(flags | Qt::FramelessWindowHint);
            this->setVisible(true);
            isMin = false;
        }
    }
#endif
}

void FramelessDialog::doResizeEvent(QEvent *event)
{
    //非win系统的无边框拉伸,win系统上已经采用了nativeEvent来处理拉伸
    //为何不统一用计算的方式因为在win上用这个方式往左拉伸会发抖妹的
#ifndef Q_OS_WIN
    if (event->type() == QEvent::Resize) {
        //重新计算八个描点的区域,描点区域的作用还有就是计算鼠标坐标是否在某一个区域内
        int width = this->width();
        int height = this->height();

        //左侧描点区域
        pressedRect[0] = QRect(0, padding, padding, height - padding * 2);
        //右侧描点区域
        pressedRect[1] = QRect(width - padding, padding, padding, height - padding * 2);
        //上侧描点区域
        pressedRect[2] = QRect(padding, 0, width - padding * 2, padding);
        //下侧描点区域
        pressedRect[3] = QRect(padding, height - padding, width - padding * 2, padding);

        //左上角描点区域
        pressedRect[4] = QRect(0, 0, padding, padding);
        //右上角描点区域
        pressedRect[5] = QRect(width - padding, 0, padding, padding);
        //左下角描点区域
        pressedRect[6] = QRect(0, height - padding, padding, padding);
        //右下角描点区域
        pressedRect[7] = QRect(width - padding, height - padding, padding, padding);
    } else if (event->type() == QEvent::HoverMove) {
        //设置对应鼠标形状,这个必须放在这里而不是下面,因为可以在鼠标没有按下的时候识别
        QHoverEvent *hoverEvent = (QHoverEvent *)event;
        QPoint point = hoverEvent->pos();
        if (resizeEnable) {
            if (pressedRect.at(0).contains(point)) {
                this->setCursor(Qt::SizeHorCursor);
            } else if (pressedRect.at(1).contains(point)) {
                this->setCursor(Qt::SizeHorCursor);
            } else if (pressedRect.at(2).contains(point)) {
                this->setCursor(Qt::SizeVerCursor);
            } else if (pressedRect.at(3).contains(point)) {
                this->setCursor(Qt::SizeVerCursor);
            } else if (pressedRect.at(4).contains(point)) {
                this->setCursor(Qt::SizeFDiagCursor);
            } else if (pressedRect.at(5).contains(point)) {
                this->setCursor(Qt::SizeBDiagCursor);
            } else if (pressedRect.at(6).contains(point)) {
                this->setCursor(Qt::SizeBDiagCursor);
            } else if (pressedRect.at(7).contains(point)) {
                this->setCursor(Qt::SizeFDiagCursor);
            } else {
                this->setCursor(Qt::ArrowCursor);
            }
        }

        //根据当前鼠标位置,计算XY轴移动了多少
        int offsetX = point.x() - mousePoint.x();
        int offsetY = point.y() - mousePoint.y();

        //根据按下处的位置判断是否是移动控件还是拉伸控件
        if (moveEnable && mousePressed) {
            this->move(this->x() + offsetX, this->y() + offsetY);
        }

        if (resizeEnable) {
            int rectX = mouseRect.x();
            int rectY = mouseRect.y();
            int rectW = mouseRect.width();
            int rectH = mouseRect.height();

            if (pressedArea.at(0)) {
                int resizeW = this->width() - offsetX;
                if (this->minimumWidth() <= resizeW) {
                    this->setGeometry(this->x() + offsetX, rectY, resizeW, rectH);
                }
            } else if (pressedArea.at(1)) {
                this->setGeometry(rectX, rectY, rectW + offsetX, rectH);
            } else if (pressedArea.at(2)) {
                int resizeH = this->height() - offsetY;
                if (this->minimumHeight() <= resizeH) {
                    this->setGeometry(rectX, this->y() + offsetY, rectW, resizeH);
                }
            } else if (pressedArea.at(3)) {
                this->setGeometry(rectX, rectY, rectW, rectH + offsetY);
            } else if (pressedArea.at(4)) {
                int resizeW = this->width() - offsetX;
                int resizeH = this->height() - offsetY;
                if (this->minimumWidth() <= resizeW) {
                    this->setGeometry(this->x() + offsetX, this->y(), resizeW, resizeH);
                }
                if (this->minimumHeight() <= resizeH) {
                    this->setGeometry(this->x(), this->y() + offsetY, resizeW, resizeH);
                }
            } else if (pressedArea.at(5)) {
                int resizeW = rectW + offsetX;
                int resizeH = this->height() - offsetY;
                if (this->minimumHeight() <= resizeH) {
                    this->setGeometry(this->x(), this->y() + offsetY, resizeW, resizeH);
                }
            } else if (pressedArea.at(6)) {
                int resizeW = this->width() - offsetX;
                int resizeH = rectH + offsetY;
                if (this->minimumWidth() <= resizeW) {
                    this->setGeometry(this->x() + offsetX, this->y(), resizeW, resizeH);
                }
                if (this->minimumHeight() <= resizeH) {
                    this->setGeometry(this->x(), this->y(), resizeW, resizeH);
                }
            } else if (pressedArea.at(7)) {
                int resizeW = rectW + offsetX;
                int resizeH = rectH + offsetY;
                this->setGeometry(this->x(), this->y(), resizeW, resizeH);
            }
        }
    } else if (event->type() == QEvent::MouseButtonPress) {
        //记住鼠标按下的坐标+窗体区域
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        mousePoint = mouseEvent->pos();
        mouseRect = this->geometry();

        //判断按下的手柄的区域位置
        if (pressedRect.at(0).contains(mousePoint)) {
            pressedArea[0] = true;
        } else if (pressedRect.at(1).contains(mousePoint)) {
            pressedArea[1] = true;
        } else if (pressedRect.at(2).contains(mousePoint)) {
            pressedArea[2] = true;
        } else if (pressedRect.at(3).contains(mousePoint)) {
            pressedArea[3] = true;
        } else if (pressedRect.at(4).contains(mousePoint)) {
            pressedArea[4] = true;
        } else if (pressedRect.at(5).contains(mousePoint)) {
            pressedArea[5] = true;
        } else if (pressedRect.at(6).contains(mousePoint)) {
            pressedArea[6] = true;
        } else if (pressedRect.at(7).contains(mousePoint)) {
            pressedArea[7] = true;
        } else {
            mousePressed = true;
        }
    } else if (event->type() == QEvent::MouseMove) {
        //改成用HoverMove识别
    } else if (event->type() == QEvent::MouseButtonRelease) {
        //恢复所有
        this->setCursor(Qt::ArrowCursor);
        mousePressed = false;
        for (int i = 0; i < 8; ++i) {
            pressedArea[i] = false;
        }
    }
#endif
}

bool FramelessDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        if (event->type() == QEvent::WindowStateChange) {
            doWindowStateChange(event);
        } else {
            doResizeEvent(event);
        }
    } else if (watched == titleBar) {
        //双击标题栏发出双击信号给主界面
        //下面的 *result = HTCAPTION; 标志位也会自动识别双击标题栏
#ifndef Q_OS_WIN
        if (event->type() == QEvent::MouseButtonDblClick) {
            emit titleDblClick();
        } else if (event->type() == QEvent::NonClientAreaMouseButtonDblClick) {
            emit titleDblClick();
        }
#endif
    }

    return QDialog::eventFilter(watched, event);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
bool FramelessDialog::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool FramelessDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif
{
    if (eventType == "windows_generic_MSG") {
#ifdef Q_OS_WIN
        MSG *msg = static_cast<MSG *>(message);
        //qDebug() << TIMEMS << "nativeEvent" << msg->wParam << msg->message;

        //不同的消息类型和参数进行不同的处理
        if (msg->message == WM_NCCALCSIZE) {
            *result = 0;
            return true;
        } else if (msg->message == WM_SYSKEYDOWN) {
            //屏蔽alt键按下
        } else if (msg->message == WM_SYSKEYUP) {
            //屏蔽alt键松开
        } else if (msg->message == WM_NCHITTEST) {
            //计算鼠标对应的屏幕坐标
            //这里最开始用的 LOWORD HIWORD 在多屏幕的时候会有问题
            //官方说明在这里 https://docs.microsoft.com/zh-cn/windows/win32/inputdev/wm-nchittest
            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);
            QPoint pos = mapFromGlobal(QPoint(x, y));

            //判断当前鼠标位置在哪个区域
            bool left = pos.x() < padding;
            bool right = pos.x() > width() - padding;
            bool top = pos.y() < padding;
            bool bottom = pos.y() > height() - padding;

            //鼠标移动到四个角,这个消息是当鼠标移动或者有鼠标键按下时候发出的
            *result = 0;
            if (resizeEnable) {
                if (left && top) {
                    *result = HTTOPLEFT;
                } else if (left && bottom) {
                    *result = HTBOTTOMLEFT;
                } else if (right && top) {
                    *result = HTTOPRIGHT;
                } else if (right && bottom) {
                    *result = HTBOTTOMRIGHT;
                } else if (left) {
                    *result = HTLEFT;
                } else if (right) {
                    *result = HTRIGHT;
                } else if (top) {
                    *result = HTTOP;
                } else if (bottom) {
                    *result = HTBOTTOM;
                }
            }

            //先处理掉拉伸
            if (0 != *result) {
                return true;
            }

            //识别标题栏拖动产生半屏全屏效果
            if (titleBar && titleBar->rect().contains(pos)) {
                QWidget *child = titleBar->childAt(pos);
                if (!child) {
                    *result = HTCAPTION;
                    return true;
                }
            }
        } else if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST) {
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC) {
            //休眠唤醒后自动打开
            this->showNormal();
        }
#endif
    } else if (eventType == "NSEvent") {
#ifdef Q_OS_MACOS
#endif
    } else if (eventType == "xcb_generic_event_t") {
#ifdef Q_OS_LINUX
#endif
    }
    return false;
}

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#ifdef Q_OS_WIN
bool FramelessDialog::winEvent(MSG *message, long *result)
{
    return nativeEvent("windows_generic_MSG", message, result);
}
#endif
#endif

void FramelessDialog::setPadding(int padding)
{
    this->padding = padding;
}

void FramelessDialog::setMoveEnable(bool moveEnable)
{
    this->moveEnable = moveEnable;
}

void FramelessDialog::setResizeEnable(bool resizeEnable)
{
    this->resizeEnable = resizeEnable;
}

void FramelessDialog::setTitleBar(QWidget *titleBar)
{
    this->titleBar = titleBar;
    this->titleBar->installEventFilter(this);
}


