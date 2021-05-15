#include "framelesswidget2.h"
#include "qevent.h"
#include "qdebug.h"
#ifdef Q_OS_WIN
#include "windows.h"
#endif

FramelessWidget2::FramelessWidget2(QWidget *parent) : QWidget(parent)
{
    padding = 5;
    this->installEventFilter(this);
}

bool FramelessWidget2::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
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

bool FramelessWidget2::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    MSG *msg = (MSG *)message;
    switch (msg->message) {
        case WM_NCHITTEST: {
            QPoint pos = mapFromGlobal(QPoint(LOWORD(msg->lParam), HIWORD(msg->lParam)));
            bool left = pos.x() < padding;
            bool right = pos.x() > width() - padding;
            bool top = pos.y() < padding;
            bool bottom = pos.y() > height() - padding;
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
            } else {
                return false;
            }
            return true;
        }
        break;
        default:
            break;
    }
#endif

    return false;
}

#ifdef Q_OS_WIN
bool FramelessWidget2::winEvent(MSG *message, long *result)
{
    return nativeEvent("windows_generic_MSG", message, result);
}
#endif
