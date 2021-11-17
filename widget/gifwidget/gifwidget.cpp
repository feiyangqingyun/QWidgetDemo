#pragma execution_character_set("utf-8")

#include "gifwidget.h"
#include "qmutex.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "qpainter.h"
#include "qevent.h"
#include "qstyle.h"
#include "qpixmap.h"
#include "qtimer.h"
#include "qdatetime.h"
#include "qapplication.h"
#include "qdesktopservices.h"
#include "qfiledialog.h"
#include "qurl.h"
#include "qtextcodec.h"
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

QScopedPointer<GifWidget> GifWidget::self;
GifWidget *GifWidget::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new GifWidget);
        }
    }

    return self.data();
}

GifWidget::GifWidget(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
}

bool GifWidget::eventFilter(QObject *watched, QEvent *event)
{
    static QPoint mousePoint;
    static bool mousePressed = false;

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    if (mouseEvent->type() == QEvent::MouseButtonPress) {
        if (mouseEvent->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = mouseEvent->globalPos() - this->pos();
            return true;
        }
    } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
        mousePressed = false;
        return true;
    } else if (mouseEvent->type() == QEvent::MouseMove) {
        if (mousePressed) {
            this->move(mouseEvent->globalPos() - mousePoint);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void GifWidget::resizeEvent(QResizeEvent *e)
{
    //拉动右下角改变大小自动赋值
    txtWidth->setText(QString::number(widgetMain->width()));
    txtHeight->setText(QString::number(widgetMain->height()));
    QDialog::resizeEvent(e);
}

void GifWidget::paintEvent(QPaintEvent *)
{
    int width = txtWidth->text().toInt();
    int height = txtHeight->text().toInt();
    rectGif = QRect(borderWidth, widgetTop->height(), width - (borderWidth * 2), height);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(this->rect(), 5, 5);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rectGif, Qt::SolidPattern);
}

int GifWidget::getBorderWidth() const
{
    return this->borderWidth;
}

QColor GifWidget::getBgColor() const
{
    return this->bgColor;
}

void GifWidget::initControl()
{
    this->setObjectName("GifWidget");
    this->resize(800, 600);
    this->setSizeGripEnabled(true);
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(11, 11, 11, 11);
    verticalLayout->setObjectName("verticalLayout");
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    widgetTop = new QWidget(this);
    widgetTop->setObjectName("widgetTop");
    widgetTop->setMinimumSize(QSize(0, 35));
    widgetTop->setMaximumSize(QSize(16777215, 35));

    QHBoxLayout *layoutTop = new QHBoxLayout(widgetTop);
    layoutTop->setSpacing(0);
    layoutTop->setContentsMargins(11, 11, 11, 11);
    layoutTop->setObjectName("layoutTop");
    layoutTop->setContentsMargins(0, 0, 0, 0);

    QPushButton *btnIcon = new QPushButton(widgetTop);
    btnIcon->setObjectName("btnIcon");
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(btnIcon->sizePolicy().hasHeightForWidth());
    btnIcon->setSizePolicy(sizePolicy);
    btnIcon->setMinimumSize(QSize(35, 0));
    btnIcon->setFlat(true);
    layoutTop->addWidget(btnIcon);

    QLabel *labTitle = new QLabel(widgetTop);
    labTitle->setObjectName("labTitle");
    layoutTop->addWidget(labTitle);

    QSpacerItem *horizontalSpacer = new QSpacerItem(87, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layoutTop->addItem(horizontalSpacer);

    QPushButton *btnClose = new QPushButton(widgetTop);
    btnClose->setObjectName("btnClose");
    sizePolicy.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
    btnClose->setSizePolicy(sizePolicy);
    btnClose->setMinimumSize(QSize(35, 0));
    btnClose->setFocusPolicy(Qt::NoFocus);
    btnClose->setFlat(true);
    layoutTop->addWidget(btnClose);
    verticalLayout->addWidget(widgetTop);

    widgetMain = new QWidget(this);
    widgetMain->setObjectName("widgetMain");
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(widgetMain->sizePolicy().hasHeightForWidth());
    widgetMain->setSizePolicy(sizePolicy1);
    verticalLayout->addWidget(widgetMain);

    widgetBottom = new QWidget(this);
    widgetBottom->setObjectName("widgetBottom");
    widgetBottom->setMinimumSize(QSize(0, 45));
    widgetBottom->setMaximumSize(QSize(16777215, 45));

    QHBoxLayout *layoutBottom = new QHBoxLayout(widgetBottom);
    layoutBottom->setSpacing(6);
    layoutBottom->setContentsMargins(11, 11, 11, 11);
    layoutBottom->setObjectName("layoutBottom");
    layoutBottom->setContentsMargins(9, 9, -1, -1);

    QLabel *labFps = new QLabel(widgetBottom);
    labFps->setObjectName("labFps");
    layoutBottom->addWidget(labFps);

    txtFps = new QLineEdit(widgetBottom);
    txtFps->setObjectName("txtFps");
    txtFps->setMaximumSize(QSize(50, 16777215));
    txtFps->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtFps);

    QLabel *labWidth = new QLabel(widgetBottom);
    labWidth->setObjectName("labWidth");
    layoutBottom->addWidget(labWidth);

    txtWidth = new QLineEdit(widgetBottom);
    txtWidth->setObjectName("txtWidth");
    txtWidth->setEnabled(true);
    txtWidth->setMaximumSize(QSize(50, 16777215));
    txtWidth->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtWidth);

    QLabel *labHeight = new QLabel(widgetBottom);
    labHeight->setObjectName("labHeight");
    layoutBottom->addWidget(labHeight);

    txtHeight = new QLineEdit(widgetBottom);
    txtHeight->setObjectName("txtHeight");
    txtHeight->setEnabled(true);
    txtHeight->setMaximumSize(QSize(50, 16777215));
    txtHeight->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(txtHeight);

    labStatus = new QLabel(widgetBottom);
    labStatus->setObjectName("labStatus");
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labStatus->sizePolicy().hasHeightForWidth());
    labStatus->setSizePolicy(sizePolicy2);
    labStatus->setAlignment(Qt::AlignCenter);
    layoutBottom->addWidget(labStatus);

    btnStart = new QPushButton(widgetBottom);
    btnStart->setObjectName("btnStart");
    sizePolicy.setHeightForWidth(btnStart->sizePolicy().hasHeightForWidth());
    btnStart->setSizePolicy(sizePolicy);
    layoutBottom->addWidget(btnStart);
    verticalLayout->addWidget(widgetBottom);

    labTitle->setText("GIF录屏工具(QQ:517216493)");
    labFps->setText("帧率");
    labWidth->setText("宽度");
    labHeight->setText("高度");
    btnStart->setText("开始");
    this->setWindowTitle(labTitle->text());

    btnIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    btnClose->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    connect(btnClose, SIGNAL(clicked(bool)), this, SLOT(closeAll()));
    connect(btnStart, SIGNAL(clicked(bool)), this, SLOT(record()));
    connect(txtWidth, SIGNAL(editingFinished()), this, SLOT(resizeForm()));
    connect(txtHeight, SIGNAL(editingFinished()), this, SLOT(resizeForm()));
}

void GifWidget::initForm()
{
    borderWidth = 3;
    bgColor = QColor(34, 163, 169);

    fps = 10;
    txtFps->setText(QString::number(fps));
    gifWriter = 0;

    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(saveImage()));

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->installEventFilter(this);

    QStringList qss;
    qss.append("QLabel{color:#ffffff;}");
    qss.append("#btnClose,#btnIcon{border:none;border-radius:0px;}");
    qss.append("#btnClose:hover{background-color:#ff0000;}");
    qss.append("#btnClose{border-top-right-radius:5px;}");
    qss.append("#labTitle{font:bold 16px;}");
    qss.append("#labStatus{font:15px;}");
    this->setStyleSheet(qss.join(""));
}

void GifWidget::saveImage()
{
    if (!gifWriter) {
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QScreen *screen = QApplication::primaryScreen();
    QPixmap pix = screen->grabWindow(0, x() + rectGif.x(), y() + rectGif.y(), rectGif.width(), rectGif.height());
    QImage image = pix.toImage().convertToFormat(QImage::Format_RGBA8888);
#else
    //由于qt4没有RGBA8888,采用最接近RGBA8888的是ARGB32,颜色会有点偏差
    QPixmap pix = QPixmap::grabWindow(0, x() + rectGif.x(), y() + rectGif.y(), rectGif.width(), rectGif.height());
    QImage image = pix.toImage().convertToFormat(QImage::Format_ARGB32);
#endif

    gif.GifWriteFrame(gifWriter, image.bits(), rectGif.width(), rectGif.height(), fps);
    count++;
    labStatus->setText(QString("正在录制 第 %1 帧").arg(count));
}

void GifWidget::record()
{
    if (btnStart->text() == "开始") {
        if (0 != gifWriter) {
            delete gifWriter;
            gifWriter = 0;
        }

        //先弹出文件保存对话框
        //fileName = qApp->applicationDirPath() + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.gif");
        fileName = QFileDialog::getSaveFileName(this, "选择保存位置", qApp->applicationDirPath() + "/", "gif图片(*.gif)");
        if (fileName.isEmpty()) {
            return;
        }

        int width = txtWidth->text().toInt();
        int height = txtHeight->text().toInt();
        fps = txtFps->text().toInt();

#ifdef Q_OS_WIN
        //windows上需要先转码
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
        QTextCodec *code = QTextCodec::codecForName("utf-8");
#else
        QTextCodec *code = QTextCodec::codecForName("gbk");
#endif
        const char *name = code->fromUnicode(fileName).constData();
#else
        const char *name = fileName.toUtf8().constData();
#endif

        gifWriter = new Gif::GifWriter;
        bool ok = gif.GifBegin(gifWriter, name, width, height, fps);
        if (!ok) {
            delete gifWriter;
            gifWriter = 0;
            return;
        }

        count = 0;
        labStatus->setText("开始录制...");
        btnStart->setText("停止");
        //延时启动
        timer->setInterval(1000 / fps);
        QTimer::singleShot(1000, timer, SLOT(start()));
        //saveImage();
    } else {
        timer->stop();
        gif.GifEnd(gifWriter);

        delete gifWriter;
        gifWriter = 0;

        labStatus->setText(QString("录制完成 共 %1 帧").arg(count));
        btnStart->setText("开始");
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void GifWidget::closeAll()
{
    if (0 != gifWriter) {
        delete gifWriter;
        gifWriter = 0;
    }

    this->close();
}

void GifWidget::resizeForm()
{
    int width = txtWidth->text().toInt();
    int height = txtHeight->text().toInt();
    this->resize(width, height + widgetTop->height() + widgetBottom->height());
}

void GifWidget::setBorderWidth(int borderWidth)
{
    if (this->borderWidth != borderWidth) {
        this->borderWidth = borderWidth;
        this->update();
    }
}

void GifWidget::setBgColor(const QColor &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
        this->update();
    }
}
