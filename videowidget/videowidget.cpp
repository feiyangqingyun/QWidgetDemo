#pragma execution_character_set("utf-8")

#include "videowidget.h"
#include "qfontdatabase.h"
#include "qpushbutton.h"
#include "qtreewidget.h"
#include "qlayout.h"
#include "qtimer.h"
#include "qdir.h"
#include "qpainter.h"
#include "qevent.h"
#include "qmimedata.h"
#include "qurl.h"
#include "qdebug.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{
    //设置强焦点
    setFocusPolicy(Qt::StrongFocus);
    //设置支持拖放
    setAcceptDrops(true);

    //定时器校验视频
    timerCheck = new QTimer(this);
    timerCheck->setInterval(10 * 1000);
    connect(timerCheck, SIGNAL(timeout()), this, SLOT(checkVideo()));

    image = QImage();
    copyImage = false;
    checkLive = true;
    drawImage = true;
    fillImage = true;

    flowEnable = false;
    flowBgColor = "#000000";
    flowPressColor = "#5EC7D9";

    timeout = 20;
    borderWidth = 5;
    borderColor = "#000000";
    focusColor = "#22A3A9";
    bgText = "实时视频";
    bgImage = QImage();

    osd1Visible = false;
    osd1FontSize = 12;
    osd1Text = "时间";
    osd1Color = "#FF0000";
    osd1Image = QImage();
    osd1Format = OSDFormat_DateTime;
    osd1Position = OSDPosition_Right_Top;

    osd2Visible = false;
    osd2FontSize = 12;
    osd2Text = "通道名称";
    osd2Color = "#FF0000";
    osd2Image = QImage();
    osd2Format = OSDFormat_Text;
    osd2Position = OSDPosition_Left_Bottom;

    //初始化解码线程
    this->initThread();
    //初始化悬浮条
    this->initFlowPanel();
    //初始化悬浮条样式
    this->initFlowStyle();
}

void VideoWidget::initThread()
{

}

void VideoWidget::initFlowPanel()
{
    //顶部工具栏,默认隐藏,鼠标移入显示移除隐藏
    flowPanel = new QWidget(this);
    flowPanel->setObjectName("flowPanel");
    flowPanel->setVisible(false);

    //用布局顶住,左侧弹簧
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setMargin(0);
    layout->addStretch();
    flowPanel->setLayout(layout);

    //按钮集合名称,如果需要新增按钮则在这里增加即可
    QList<QString> btns;
    btns << "btnFlowVideo" << "btnFlowSnap" << "btnFlowSound" << "btnFlowAlarm" << "btnFlowClose";

    //有多种办法来设置图片,qt内置的图标+自定义的图标+图形字体
    //既可以设置图标形式,也可以直接图形字体设置文本
#if 0
    QList<QIcon> icons;
    icons << QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_DialogOkButton);
    icons << QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton);
#else
    QList<QChar> chars;
    chars << 0xe68d << 0xe672 << 0xe674 << 0xea36 << 0xe74c;

    //判断图形字体是否存在,不存在则加入
    QFont iconFont;
    QFontDatabase fontDb;
    if (!fontDb.families().contains("iconfont")) {
        int fontId = fontDb.addApplicationFont(":/image/iconfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load iconfont.ttf error";
        }
    }

    if (fontDb.families().contains("iconfont")) {
        iconFont = QFont("iconfont");
        iconFont.setPixelSize(17);
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }
#endif

    //循环添加顶部按钮
    for (int i = 0; i < btns.count(); i++) {
        QPushButton *btn = new QPushButton;
        //绑定按钮单击事件,用来发出信号通知
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
        //设置标识,用来区别按钮
        btn->setObjectName(btns.at(i));
        //设置固定宽度
        btn->setFixedWidth(20);
        //设置拉伸策略使得填充
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        //设置焦点策略为无焦点,避免单击后焦点跑到按钮上
        btn->setFocusPolicy(Qt::NoFocus);

#if 0
        //设置图标大小和图标
        btn->setIconSize(QSize(16, 16));
        btn->setIcon(icons.at(i));
#else
        btn->setFont(iconFont);
        btn->setText(chars.at(i));
#endif

        //将按钮加到布局中
        layout->addWidget(btn);
    }
}

void VideoWidget::initFlowStyle()
{
    //设置样式以便区分,可以自行更改样式,也可以不用样式
    QStringList qss;
    QString rgba = QString("rgba(%1,%2,%3,150)").arg(flowBgColor.red()).arg(flowBgColor.green()).arg(flowBgColor.blue());
    qss.append(QString("#flowPanel{background:%1;border:none;}").arg(rgba));
    qss.append(QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}"));
    qss.append(QString("QPushButton:pressed{color:%1;}").arg(flowPressColor.name()));
    flowPanel->setStyleSheet(qss.join(""));
}

VideoWidget::~VideoWidget()
{
    if (timerCheck->isActive()) {
        timerCheck->stop();
    }

    close();
}

void VideoWidget::resizeEvent(QResizeEvent *)
{
    //重新设置顶部工具栏的位置和宽高,可以自行设置顶部显示或者底部显示
    int height = 20;
    flowPanel->setGeometry(borderWidth, borderWidth, this->width() - (borderWidth * 2), height);
    //flowPanel->setGeometry(borderWidth, this->height() - height - borderWidth, this->width() - (borderWidth * 2), height);
}

void VideoWidget::enterEvent(QEvent *)
{
    //这里还可以增加一个判断,是否获取了焦点的才需要显示
    //if (this->hasFocus()) {}
    if (flowEnable) {
        flowPanel->setVisible(true);
    }
}

void VideoWidget::leaveEvent(QEvent *)
{
    if (flowEnable) {
        flowPanel->setVisible(false);
    }
}

void VideoWidget::dropEvent(QDropEvent *event)
{
    //拖放完毕鼠标松开的时候执行
    //判断拖放进来的类型,取出文件,进行播放
    if (event->mimeData()->hasUrls()) {
        QString url = event->mimeData()->urls().first().toLocalFile();
        this->close();
        this->setUrl(url);
        this->open();
        emit fileDrag(url);
    } else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QTreeWidget *treeWidget = (QTreeWidget *)event->source();
        if (treeWidget != 0) {
            QString url = treeWidget->currentItem()->data(0, Qt::UserRole).toString();
            this->close();
            this->setUrl(url);
            this->open();
            emit fileDrag(url);
        }
    }
}

void VideoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //拖曳进来的时候先判断下类型,非法类型则不处理
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else if (event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::LinkAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    //如果不需要绘制
    if (!drawImage) {
        return;
    }

    //qDebug() << TIMEMS << "paintEvent" << objectName();
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    //绘制边框
    drawBorder(&painter);
    if (!image.isNull()) {
        //绘制背景图片
        drawImg(&painter, image);
        //绘制标签
        drawOSD(&painter, osd1Visible, osd1FontSize, osd1Text, osd1Color, osd1Image, osd1Format, osd1Position);
        drawOSD(&painter, osd2Visible, osd2FontSize, osd2Text, osd2Color, osd2Image, osd2Format, osd2Position);
    } else {
        //绘制背景
        drawBg(&painter);
    }
}

void VideoWidget::drawBorder(QPainter *painter)
{
    if (borderWidth == 0) {
        return;
    }

    painter->save();
    QPen pen;
    pen.setWidth(borderWidth);
    pen.setColor(hasFocus() ? focusColor : borderColor);
    painter->setPen(pen);
    painter->drawRect(rect());
    painter->restore();
}

void VideoWidget::drawBg(QPainter *painter)
{
    painter->save();

    //背景图片为空则绘制文字,否则绘制背景图片
    if (bgImage.isNull()) {
        painter->setFont(this->font());
        painter->setPen(palette().foreground().color());
        painter->drawText(rect(), Qt::AlignCenter, bgText);
    } else {
        //居中绘制
        int x = rect().center().x() - bgImage.width() / 2;
        int y = rect().center().y() - bgImage.height() / 2;
        QPoint point(x, y);
        painter->drawImage(point, bgImage);
    }

    painter->restore();
}

void VideoWidget::drawImg(QPainter *painter, QImage img)
{
    painter->save();

    int offset = borderWidth * 1 + 0;
    if (fillImage) {
        QRect rect(offset / 2, offset / 2, width() - offset, height() - offset);
        painter->drawImage(rect, img);
    } else {
        //按照比例自动居中绘制
        img = img.scaled(width() - offset, height() - offset, Qt::KeepAspectRatio);
        int x = rect().center().x() - img.width() / 2;
        int y = rect().center().y() - img.height() / 2;
        QPoint point(x, y);
        painter->drawImage(point, img);
    }

    painter->restore();
}

void VideoWidget::drawOSD(QPainter *painter,
                          bool osdVisible,
                          int osdFontSize,
                          const QString &osdText,
                          const QColor &osdColor,
                          const QImage &osdImage,
                          const VideoWidget::OSDFormat &osdFormat,
                          const VideoWidget::OSDPosition &osdPosition)
{
    if (!osdVisible) {
        return;
    }

    painter->save();

    //标签位置尽量偏移多一点避免遮挡
    QRect osdRect(rect().x() + (borderWidth * 2), rect().y() + (borderWidth * 2), width() - (borderWidth * 5), height() - (borderWidth * 5));
    int flag = Qt::AlignLeft | Qt::AlignTop;
    QPoint point = QPoint(osdRect.x(), osdRect.y());

    if (osdPosition == OSDPosition_Left_Top) {
        flag = Qt::AlignLeft | Qt::AlignTop;
        point = QPoint(osdRect.x(), osdRect.y());
    } else if (osdPosition == OSDPosition_Left_Bottom) {
        flag = Qt::AlignLeft | Qt::AlignBottom;
        point = QPoint(osdRect.x(), osdRect.height() - osdImage.height());
    } else if (osdPosition == OSDPosition_Right_Top) {
        flag = Qt::AlignRight | Qt::AlignTop;
        point = QPoint(osdRect.width() - osdImage.width(), osdRect.y());
    } else if (osdPosition == OSDPosition_Right_Bottom) {
        flag = Qt::AlignRight | Qt::AlignBottom;
        point = QPoint(osdRect.width() - osdImage.width(), osdRect.height() - osdImage.height());
    }

    if (osdFormat == OSDFormat_Image) {
        painter->drawImage(point, osdImage);
    } else {
        QDateTime now = QDateTime::currentDateTime();
        QString text = osdText;
        if (osdFormat == OSDFormat_Date) {
            text = now.toString("yyyy-MM-dd");
        } else if (osdFormat == OSDFormat_Time) {
            text = now.toString("HH:mm:ss");
        } else if (osdFormat == OSDFormat_DateTime) {
            text = now.toString("yyyy-MM-dd HH:mm:ss");
        }

        //设置颜色及字号
        QFont font;
        font.setPixelSize(osdFontSize);
        painter->setPen(osdColor);
        painter->setFont(font);

        painter->drawText(osdRect, flag, text);
    }

    painter->restore();
}

QImage VideoWidget::getImage() const
{
    return this->image;
}

QPixmap VideoWidget::getPixmap() const
{
    return QPixmap();
}

QString VideoWidget::getUrl() const
{
    return this->property("url").toString();
}

QDateTime VideoWidget::getLastTime() const
{
    return QDateTime::currentDateTime();
}

bool VideoWidget::getCallback() const
{
    return false;
}

bool VideoWidget::getIsPlaying() const
{
    return false;
}

bool VideoWidget::getIsRtsp() const
{
    return false;
}

bool VideoWidget::getIsUsbCamera() const
{
    return false;
}

bool VideoWidget::getCopyImage() const
{
    return this->copyImage;
}

bool VideoWidget::getCheckLive() const
{
    return this->checkLive;
}

bool VideoWidget::getDrawImage() const
{
    return this->drawImage;
}

bool VideoWidget::getFillImage() const
{
    return this->fillImage;
}

bool VideoWidget::getFlowEnable() const
{
    return this->flowEnable;
}

QColor VideoWidget::getFlowBgColor() const
{
    return this->flowBgColor;
}

QColor VideoWidget::getFlowPressColor() const
{
    return this->flowPressColor;
}

int VideoWidget::getTimeout() const
{
    return this->timeout;
}

int VideoWidget::getBorderWidth() const
{
    return this->borderWidth;
}

QColor VideoWidget::getBorderColor() const
{
    return this->borderColor;
}

QColor VideoWidget::getFocusColor() const
{
    return this->focusColor;
}

QString VideoWidget::getBgText() const
{
    return this->bgText;
}

QImage VideoWidget::getBgImage() const
{
    return this->bgImage;
}

bool VideoWidget::getOSD1Visible() const
{
    return this->osd1Visible;
}

int VideoWidget::getOSD1FontSize() const
{
    return this->osd1FontSize;
}

QString VideoWidget::getOSD1Text() const
{
    return this->osd1Text;
}

QColor VideoWidget::getOSD1Color() const
{
    return this->osd1Color;
}

QImage VideoWidget::getOSD1Image() const
{
    return this->osd1Image;
}

VideoWidget::OSDFormat VideoWidget::getOSD1Format() const
{
    return this->osd1Format;
}

VideoWidget::OSDPosition VideoWidget::getOSD1Position() const
{
    return this->osd1Position;
}

bool VideoWidget::getOSD2Visible() const
{
    return this->osd2Visible;
}

int VideoWidget::getOSD2FontSize() const
{
    return this->osd2FontSize;
}

QString VideoWidget::getOSD2Text() const
{
    return this->osd2Text;
}

QColor VideoWidget::getOSD2Color() const
{
    return this->osd2Color;
}

QImage VideoWidget::getOSD2Image() const
{
    return this->osd2Image;
}

VideoWidget::OSDFormat VideoWidget::getOSD2Format() const
{
    return this->osd2Format;
}

VideoWidget::OSDPosition VideoWidget::getOSD2Position() const
{
    return this->osd2Position;
}

int VideoWidget::getFaceBorder() const
{
    return this->faceBorder;
}

QColor VideoWidget::getFaceColor() const
{
    return this->faceColor;
}

QList<QRect> VideoWidget::getFaceRects() const
{
    return this->faceRects;
}

void VideoWidget::updateImage(const QImage &image)
{
    //拷贝图片有个好处,当处理器比较差的时候,图片不会产生断层,缺点是占用时间
    //默认QImage类型是浅拷贝,可能正在绘制的时候,那边已经更改了图片的上部分数据
    this->image = copyImage ? image.copy() : image;
    this->update();
}

void VideoWidget::checkVideo()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime lastTime = now;
    int sec = lastTime.secsTo(now);
    if (sec >= timeout) {
        restart();
    }
}

void VideoWidget::btnClicked()
{
    QPushButton *btn = (QPushButton *)sender();
    emit btnClicked(btn->objectName());
}

void VideoWidget::setVideoWidth(int videoWidth)
{

}

void VideoWidget::setVideoHeight(int videoHeight)
{

}

void VideoWidget::setBufferWidth(int bufferWidth)
{

}

void VideoWidget::setBufferHeight(int bufferHeight)
{

}

uint VideoWidget::getLength()
{
    return 0;
}

uint VideoWidget::getPosition()
{
    return 0;
}

void VideoWidget::setPosition(int position)
{

}

bool VideoWidget::getMuted()
{
    return false;
}

void VideoWidget::setMuted(bool muted)
{

}

int VideoWidget::getVolume()
{
    return 0;
}

void VideoWidget::setVolume(int volume)
{

}

void VideoWidget::setInterval(int interval)
{

}

void VideoWidget::setSleepTime(int sleepTime)
{

}

void VideoWidget::setCheckTime(int checkTime)
{

}

void VideoWidget::setCheckConn(bool checkConn)
{

}

void VideoWidget::setUrl(const QString &url)
{
    this->setProperty("url", url);
}

void VideoWidget::setCallback(bool callback)
{

}

void VideoWidget::setHardware(const QString &hardware)
{

}

void VideoWidget::setSaveFile(bool saveFile)
{

}

void VideoWidget::setSaveInterval(int saveInterval)
{

}

void VideoWidget::setFileFlag(const QString &fileFlag)
{

}

void VideoWidget::setSavePath(const QString &savePath)
{
    //如果目录不存在则新建
    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkdir(savePath);
    }
}

void VideoWidget::setFileName(const QString &fileName)
{

}

void VideoWidget::setCopyImage(bool copyImage)
{
    this->copyImage = copyImage;
}

void VideoWidget::setCheckLive(bool checkLive)
{
    this->checkLive = checkLive;
}

void VideoWidget::setDrawImage(bool drawImage)
{
    this->drawImage = drawImage;
}

void VideoWidget::setFillImage(bool fillImage)
{
    this->fillImage = fillImage;
}

void VideoWidget::setFlowEnable(bool flowEnable)
{
    this->flowEnable = flowEnable;
}

void VideoWidget::setFlowBgColor(const QColor &flowBgColor)
{
    if (this->flowBgColor != flowBgColor) {
        this->flowBgColor = flowBgColor;
        this->initFlowStyle();
    }
}

void VideoWidget::setFlowPressColor(const QColor &flowPressColor)
{
    if (this->flowPressColor != flowPressColor) {
        this->flowPressColor = flowPressColor;
        this->initFlowStyle();
    }
}

void VideoWidget::setTimeout(int timeout)
{
    this->timeout = timeout;
}

void VideoWidget::setBorderWidth(int borderWidth)
{
    this->borderWidth = borderWidth;
}

void VideoWidget::setBorderColor(const QColor &borderColor)
{
    this->borderColor = borderColor;
}

void VideoWidget::setFocusColor(const QColor &focusColor)
{
    this->focusColor = focusColor;
}

void VideoWidget::setBgText(const QString &bgText)
{
    this->bgText = bgText;
}

void VideoWidget::setBgImage(const QImage &bgImage)
{
    this->bgImage = bgImage;
}

void VideoWidget::setOSD1Visible(bool osdVisible)
{
    this->osd1Visible = osdVisible;
}

void VideoWidget::setOSD1FontSize(int osdFontSize)
{
    this->osd1FontSize = osdFontSize;
}

void VideoWidget::setOSD1Text(const QString &osdText)
{
    this->osd1Text = osdText;
}

void VideoWidget::setOSD1Color(const QColor &osdColor)
{
    this->osd1Color = osdColor;
}

void VideoWidget::setOSD1Image(const QImage &osdImage)
{
    this->osd1Image = osdImage;
}

void VideoWidget::setOSD1Format(const VideoWidget::OSDFormat &osdFormat)
{
    this->osd1Format = osdFormat;
}

void VideoWidget::setOSD1Position(const VideoWidget::OSDPosition &osdPosition)
{
    this->osd1Position = osdPosition;
}

void VideoWidget::setOSD2Visible(bool osdVisible)
{
    this->osd2Visible = osdVisible;
}

void VideoWidget::setOSD2FontSize(int osdFontSize)
{
    this->osd2FontSize = osdFontSize;
}

void VideoWidget::setOSD2Text(const QString &osdText)
{
    this->osd2Text = osdText;
}

void VideoWidget::setOSD2Color(const QColor &osdColor)
{
    this->osd2Color = osdColor;
}

void VideoWidget::setOSD2Image(const QImage &osdImage)
{
    this->osd2Image = osdImage;
}

void VideoWidget::setOSD2Format(const VideoWidget::OSDFormat &osdFormat)
{
    this->osd2Format = osdFormat;
}

void VideoWidget::setOSD2Position(const VideoWidget::OSDPosition &osdPosition)
{
    this->osd2Position = osdPosition;
}

void VideoWidget::setFaceBorder(int faceBorder)
{
    this->faceBorder = faceBorder;
}

void VideoWidget::setFaceColor(const QColor &faceColor)
{
    this->faceColor = faceColor;
}

void VideoWidget::setFaceRects(const QList<QRect> &faceRects)
{
    this->faceRects = faceRects;
}

void VideoWidget::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();
    clear();

    //如果是图片则只显示图片就行
    image = QImage(this->property("url").toString());
    if (!image.isNull()) {
        this->update();
        return;
    }



    if (checkLive) {
        timerCheck->start();
    }
}

void VideoWidget::pause()
{

}

void VideoWidget::next()
{

}

void VideoWidget::close()
{
    if (checkLive) {
        timerCheck->stop();
    }

    QTimer::singleShot(1, this, SLOT(clear()));
}

void VideoWidget::restart(int delayOpen)
{
    //qDebug() << TIMEMS << "restart video" << objectName();
    close();
    if (delayOpen > 0) {
        QTimer::singleShot(delayOpen, this, SLOT(open()));
    } else {
        open();
    }
}

void VideoWidget::clear()
{
    image = QImage();
    this->update();
}

void VideoWidget::snap(const QString &fileName)
{

}

