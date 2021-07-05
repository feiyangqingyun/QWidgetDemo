#include "framelesswidget2.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qdebug.h"

#ifdef Q_OS_WIN
#include "windows.h"
#pragma comment (lib,"user32.lib")
#endif

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

FramelessWidget2::FramelessWidget2(QWidget *parent) : QWidget(parent)
{
    padding = 8;
    moveEnable = true;
    resizeEnable = true;

    //安装事件过滤器识别拖动
    this->installEventFilter(this);
}

bool FramelessWidget2::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this && moveEnable) {
        static QPoint mousePoint;
        static bool mousePressed = false;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            if (mouseEvent->button() == Qt::LeftButton) {
                mousePressed = true;
                mousePoint = mouseEvent->globalPos() - this->pos();
            }
        } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            mousePressed = false;
        } else if (mouseEvent->type() == QEvent::MouseMove) {
            if (mousePressed) {
                this->move(mouseEvent->globalPos() - mousePoint);
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
bool FramelessWidget2::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool FramelessWidget2::nativeEvent(const QByteArray &eventType, void *message, long *result)
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
        } else if (msg->message == WM_NCHITTEST) {
            //计算鼠标对应的屏幕坐标
            long x = LOWORD(msg->lParam);
            long y = HIWORD(msg->lParam);
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
bool FramelessWidget2::winEvent(MSG *message, long *result)
{
    return nativeEvent("windows_generic_MSG", message, result);
}
#endif
#endif

void FramelessWidget2::setPadding(int padding)
{
    this->padding = padding;
}

void FramelessWidget2::setMoveEnable(bool moveEnable)
{
    this->moveEnable = moveEnable;
}

void FramelessWidget2::setResizeEnable(bool resizeEnable)
{
    this->resizeEnable = resizeEnable;
}
