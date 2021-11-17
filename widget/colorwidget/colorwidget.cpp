#pragma execution_character_set("utf-8")

#include "colorwidget.h"
#include "qmutex.h"
#include "qgridlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qapplication.h"
#include "qtimer.h"
#include "qevent.h"
#include "qdebug.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include "qscreen.h"
#define deskGeometry qApp->primaryScreen()->geometry()
#define deskGeometry2 qApp->primaryScreen()->availableGeometry()
#else
#include "qdesktopwidget.h"
#define deskGeometry qApp->desktop()->geometry()
#define deskGeometry2 qApp->desktop()->availableGeometry()
#endif

ColorWidget *ColorWidget::instance = 0;
ColorWidget *ColorWidget::Instance()
{
    if (!instance) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!instance) {
            instance = new ColorWidget;
        }
    }

    return instance;
}

ColorWidget::ColorWidget(QWidget *parent) : QWidget(parent)
{
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);

    labColor = new QLabel(this);
    labColor->setText("+");
    labColor->setStyleSheet("background-color: rgb(255, 107, 107);color: rgb(250, 250, 250);");
    labColor->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPixelSize(35);
    font.setBold(true);
    labColor->setFont(font);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(labColor->sizePolicy().hasHeightForWidth());
    labColor->setSizePolicy(sizePolicy);
    labColor->setMinimumSize(QSize(80, 70));
    labColor->setMaximumSize(QSize(80, 70));
    labColor->setCursor(QCursor(Qt::CrossCursor));
    labColor->setFrameShape(QFrame::StyledPanel);
    labColor->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(labColor);

    label = new QLabel(this);
    label->setMinimumSize(QSize(0, 18));
    label->setStyleSheet("background-color: rgb(0, 0, 0);color: rgb(200, 200, 200);");
    label->setAlignment(Qt::AlignCenter);

    verticalLayout->addWidget(label);
    gridLayout->addLayout(verticalLayout, 0, 0, 3, 1);

    labWeb = new QLabel(this);
    gridLayout->addWidget(labWeb, 0, 1, 1, 1);

    txtWeb = new QLineEdit(this);
    gridLayout->addWidget(txtWeb, 0, 2, 1, 1);

    labRgb = new QLabel(this);
    gridLayout->addWidget(labRgb, 1, 1, 1, 1);

    txtRgb = new QLineEdit(this);
    gridLayout->addWidget(txtRgb, 1, 2, 1, 1);

    labPoint = new QLabel(this);
    gridLayout->addWidget(labPoint, 2, 1, 1, 1);

    txtPoint = new QLineEdit(this);
    gridLayout->addWidget(txtPoint, 2, 2, 1, 1);

    label->setText("当前颜色");
    labWeb->setText("web值:");
    labRgb->setText("rgb值:");
    labPoint->setText("坐标值:");

    this->setLayout(gridLayout);
    this->setWindowTitle("屏幕拾色器");
    this->setFixedSize(300, 108);

    cp = QApplication::clipboard();
    pressed = false;

    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(showColorValue()));
    timer->start();
}

ColorWidget::~ColorWidget()
{
}

void ColorWidget::mousePressEvent(QMouseEvent *e)
{
    if (labColor->rect().contains(e->pos())) {
        pressed = true;
    }
}

void ColorWidget::mouseReleaseEvent(QMouseEvent *)
{
    pressed = false;
}

void ColorWidget::showColorValue()
{
    if (!pressed) {
        return;
    }

    int x = QCursor::pos().x();
    int y = QCursor::pos().y();
    txtPoint->setText(tr("x:%1  y:%2").arg(x).arg(y));

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QScreen *screen = qApp->primaryScreen();
    QPixmap pixmap = screen->grabWindow(0, x, y, 2, 2);
#else
    QPixmap pixmap = QPixmap::grabWindow(qApp->desktop()->winId(), x, y, 2, 2);
#endif

    int red, green, blue;
    QString strDecimalValue, strHex;
    if (pixmap.isNull()) {
        return;
    }

    QImage image = pixmap.toImage();
    if (image.valid(0, 0)) {
        QColor color = image.pixel(0, 0);
        red = color.red();
        green = color.green();
        blue = color.blue();
        QString strRed = tr("%1").arg(red & 0xFF, 2, 16, QChar('0'));
        QString strGreen = tr("%1").arg(green & 0xFF, 2, 16, QChar('0'));
        QString strBlue = tr("%1").arg(blue & 0xFF, 2, 16, QChar('0'));

        strDecimalValue = tr("%1, %2, %3").arg(red).arg(green).arg(blue);
        strHex = tr("#%1%2%3").arg(strRed.toUpper()).arg(strGreen.toUpper()).arg(strBlue.toUpper());
    }

    //根据背景色自动计算合适的前景色
    QColor color(red, green, blue);
    double gray = (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;
    QColor textColor = gray > 0.5 ? Qt::black : Qt::white;

    QString str = tr("background:rgb(%1);color:%2").arg(strDecimalValue).arg(textColor.name());
    labColor->setStyleSheet(str);
    txtRgb->setText(strDecimalValue);
    txtWeb->setText(strHex);
}
