#pragma execution_character_set("utf-8")

#include "videowindow.h"
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

VideoWindow::VideoWindow(QWidget *parent) : QWidget(parent)
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
    bgColor = Qt::transparent;
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

void VideoWindow::initThread()
{

}

void VideoWindow::initFlowPanel()
{
    //顶部工具栏,默认隐藏,鼠标移入显示移除隐藏
    flowPanel = new QWidget(this);
    flowPanel->setObjectName("flowPanel");
    flowPanel->setVisible(false);

    //用布局顶住,左侧弹簧
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setContentsMargins(0, 0, 0, 0);
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
    QList<int> icons;
    icons << 0xe68d << 0xe672 << 0xe674 << 0xea36 << 0xe74c;

    //判断图形字体是否存在,不存在则加入
    QFont iconFont;
    QFontDatabase fontDb;
    if (!fontDb.families().contains("iconfont")) {
        int fontId = fontDb.addApplicationFont(":/font/iconfont.ttf");
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
    for (int i = 0; i < btns.count(); ++i) {
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
        btn->setText((QChar)icons.at(i));
#endif

        //将按钮加到布局中
        layout->addWidget(btn);
    }
}

void VideoWindow::initFlowStyle()
{
    //设置样式以便区分,可以自行更改样式,也可以不用样式
    QStringList qss;
    QString rgba = QString("rgba(%1,%2,%3,150)").arg(flowBgColor.red()).arg(flowBgColor.green()).arg(flowBgColor.blue());
    qss.append(QString("#flowPanel{background:%1;border:none;}").arg(rgba));
    qss.append(QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}"));
    qss.append(QString("QPushButton:pressed{color:%1;}").arg(flowPressColor.name()));
    flowPanel->setStyleSheet(qss.join(""));
}

VideoWindow::~VideoWindow()
{
    if (timerCheck->isActive()) {
        timerCheck->stop();
    }

    close();
}

void VideoWindow::resizeEvent(QResizeEvent *)
{
    //重新设置顶部工具栏的位置和宽高,可以自行设置顶部显示或者底部显示
    int height = 20;
    flowPanel->setGeometry(borderWidth, borderWidth, this->width() - (borderWidth * 2), height);
    //flowPanel->setGeometry(borderWidth, this->height() - height - borderWidth, this->width() - (borderWidth * 2), height);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
void VideoWindow::enterEvent(QEnterEvent *)
#else
void VideoWindow::enterEvent(QEvent *)
#endif
{
    //这里还可以增加一个判断,是否获取了焦点的才需要显示
    //if (this->hasFocus()) {}
    if (flowEnable) {
        flowPanel->setVisible(true);
    }
}

void VideoWindow::leaveEvent(QEvent *)
{
    if (flowEnable) {
        flowPanel->setVisible(false);
    }
}

void VideoWindow::dropEvent(QDropEvent *event)
{
    //拖放完毕鼠标松开的时候执行
    //判断拖放进来的类型,取出文件,进行播放
    QString url;
    if (event->mimeData()->hasUrls()) {
        url = event->mimeData()->urls().first().toLocalFile();
    } else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QTreeWidget *treeWidget = (QTreeWidget *)event->source();
        if (treeWidget) {
            url = treeWidget->currentItem()->data(0, Qt::UserRole).toString();
        }
    }

    if (!url.isEmpty()) {
        emit fileDrag(url);
        this->restart(url);
    }
}

void VideoWindow::dragEnterEvent(QDragEnterEvent *event)
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

void VideoWindow::paintEvent(QPaintEvent *)
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

void VideoWindow::drawBorder(QPainter *painter)
{
    painter->save();

    QPen pen;
    pen.setWidth(borderWidth);
    pen.setColor(hasFocus() ? focusColor : borderColor);
    //边框宽度=0则不绘制边框
    painter->setPen(borderWidth == 0 ? Qt::NoPen : pen);
    //顺带把背景颜色这里也一并处理
    if (bgColor != Qt::transparent) {
        painter->setBrush(bgColor);
    }
    painter->drawRect(rect());

    painter->restore();
}

void VideoWindow::drawBg(QPainter *painter)
{
    painter->save();

    //背景图片为空则绘制文字,否则绘制背景图片
    if (bgImage.isNull()) {
        painter->setFont(this->font());
        painter->setPen(palette().windowText().color());
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

void VideoWindow::drawImg(QPainter *painter, QImage img)
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

void VideoWindow::drawOSD(QPainter *painter,
                          bool osdVisible,
                          int osdFontSize,
                          const QString &osdText,
                          const QColor &osdColor,
                          const QImage &osdImage,
                          const VideoWindow::OSDFormat &osdFormat,
                          const VideoWindow::OSDPosition &osdPosition)
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

QImage VideoWindow::getImage() const
{
    return this->image;
}

QPixmap VideoWindow::getPixmap() const
{
    return QPixmap();
}

QString VideoWindow::getUrl() const
{
    return this->property("url").toString();
}

QDateTime VideoWindow::getLastTime() const
{
    return QDateTime::currentDateTime();
}

bool VideoWindow::getCallback() const
{
    return false;
}

bool VideoWindow::getIsPlaying() const
{
    return false;
}

bool VideoWindow::getIsRtsp() const
{
    return false;
}

bool VideoWindow::getIsUsbCamera() const
{
    return false;
}

bool VideoWindow::getCopyImage() const
{
    return this->copyImage;
}

bool VideoWindow::getCheckLive() const
{
    return this->checkLive;
}

bool VideoWindow::getDrawImage() const
{
    return this->drawImage;
}

bool VideoWindow::getFillImage() const
{
    return this->fillImage;
}

bool VideoWindow::getFlowEnable() const
{
    return this->flowEnable;
}

QColor VideoWindow::getFlowBgColor() const
{
    return this->flowBgColor;
}

QColor VideoWindow::getFlowPressColor() const
{
    return this->flowPressColor;
}

int VideoWindow::getTimeout() const
{
    return this->timeout;
}

int VideoWindow::getBorderWidth() const
{
    return this->borderWidth;
}

QColor VideoWindow::getBorderColor() const
{
    return this->borderColor;
}

QColor VideoWindow::getFocusColor() const
{
    return this->focusColor;
}

QColor VideoWindow::getBgColor() const
{
    return this->bgColor;
}

QString VideoWindow::getBgText() const
{
    return this->bgText;
}

QImage VideoWindow::getBgImage() const
{
    return this->bgImage;
}

bool VideoWindow::getOSD1Visible() const
{
    return this->osd1Visible;
}

int VideoWindow::getOSD1FontSize() const
{
    return this->osd1FontSize;
}

QString VideoWindow::getOSD1Text() const
{
    return this->osd1Text;
}

QColor VideoWindow::getOSD1Color() const
{
    return this->osd1Color;
}

QImage VideoWindow::getOSD1Image() const
{
    return this->osd1Image;
}

VideoWindow::OSDFormat VideoWindow::getOSD1Format() const
{
    return this->osd1Format;
}

VideoWindow::OSDPosition VideoWindow::getOSD1Position() const
{
    return this->osd1Position;
}

bool VideoWindow::getOSD2Visible() const
{
    return this->osd2Visible;
}

int VideoWindow::getOSD2FontSize() const
{
    return this->osd2FontSize;
}

QString VideoWindow::getOSD2Text() const
{
    return this->osd2Text;
}

QColor VideoWindow::getOSD2Color() const
{
    return this->osd2Color;
}

QImage VideoWindow::getOSD2Image() const
{
    return this->osd2Image;
}

VideoWindow::OSDFormat VideoWindow::getOSD2Format() const
{
    return this->osd2Format;
}

VideoWindow::OSDPosition VideoWindow::getOSD2Position() const
{
    return this->osd2Position;
}

int VideoWindow::getFaceBorder() const
{
    return this->faceBorder;
}

QColor VideoWindow::getFaceColor() const
{
    return this->faceColor;
}

QList<QRect> VideoWindow::getFaceRects() const
{
    return this->faceRects;
}

QSize VideoWindow::sizeHint() const
{
    return QSize(400, 300);
}

QSize VideoWindow::minimumSizeHint() const
{
    return QSize(40, 30);
}

void VideoWindow::updateImage(const QImage &image)
{
    //拷贝图片有个好处,当处理器比较差的时候,图片不会产生断层,缺点是占用时间
    //默认QImage类型是浅拷贝,可能正在绘制的时候,那边已经更改了图片的上部分数据
    this->image = copyImage ? image.copy() : image;
    this->update();
}

void VideoWindow::checkVideo()
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime lastTime = now;
    int sec = lastTime.secsTo(now);
    if (sec >= timeout) {
        restart(this->getUrl());
    }
}

void VideoWindow::btnClicked()
{
    QPushButton *btn = (QPushButton *)sender();
    emit btnClicked(btn->objectName());
}

uint VideoWindow::getLength()
{
    return 0;
}

uint VideoWindow::getPosition()
{
    return 0;
}

void VideoWindow::setPosition(int position)
{

}

bool VideoWindow::getMuted()
{
    return false;
}

void VideoWindow::setMuted(bool muted)
{

}

int VideoWindow::getVolume()
{
    return 0;
}

void VideoWindow::setVolume(int volume)
{

}

void VideoWindow::setInterval(int interval)
{

}

void VideoWindow::setSleepTime(int sleepTime)
{

}

void VideoWindow::setCheckTime(int checkTime)
{

}

void VideoWindow::setCheckConn(bool checkConn)
{

}

void VideoWindow::setUrl(const QString &url)
{
    this->setProperty("url", url);
}

void VideoWindow::setCallback(bool callback)
{

}

void VideoWindow::setHardware(const QString &hardware)
{

}

void VideoWindow::setTransport(const QString &transport)
{

}

void VideoWindow::setSaveFile(bool saveFile)
{

}

void VideoWindow::setSaveInterval(int saveInterval)
{

}

void VideoWindow::setFileFlag(const QString &fileFlag)
{

}

void VideoWindow::setSavePath(const QString &savePath)
{
    //如果目录不存在则新建
    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkdir(savePath);
    }
}

void VideoWindow::setFileName(const QString &fileName)
{

}

void VideoWindow::setCopyImage(bool copyImage)
{
    this->copyImage = copyImage;
}

void VideoWindow::setCheckLive(bool checkLive)
{
    this->checkLive = checkLive;
}

void VideoWindow::setDrawImage(bool drawImage)
{
    this->drawImage = drawImage;
}

void VideoWindow::setFillImage(bool fillImage)
{
    this->fillImage = fillImage;
}

void VideoWindow::setFlowEnable(bool flowEnable)
{
    this->flowEnable = flowEnable;
}

void VideoWindow::setFlowBgColor(const QColor &flowBgColor)
{
    if (this->flowBgColor != flowBgColor) {
        this->flowBgColor = flowBgColor;
        this->initFlowStyle();
    }
}

void VideoWindow::setFlowPressColor(const QColor &flowPressColor)
{
    if (this->flowPressColor != flowPressColor) {
        this->flowPressColor = flowPressColor;
        this->initFlowStyle();
    }
}

void VideoWindow::setTimeout(int timeout)
{
    this->timeout = timeout;
}

void VideoWindow::setBorderWidth(int borderWidth)
{
    this->borderWidth = borderWidth;
    this->update();
}

void VideoWindow::setBorderColor(const QColor &borderColor)
{
    this->borderColor = borderColor;
    this->update();
}

void VideoWindow::setFocusColor(const QColor &focusColor)
{
    this->focusColor = focusColor;
    this->update();
}

void VideoWindow::setBgColor(const QColor &bgColor)
{
    this->bgColor = bgColor;
    this->update();
}

void VideoWindow::setBgText(const QString &bgText)
{
    this->bgText = bgText;
    this->update();
}

void VideoWindow::setBgImage(const QImage &bgImage)
{
    this->bgImage = bgImage;
    this->update();
}

void VideoWindow::setOSD1Visible(bool osdVisible)
{
    this->osd1Visible = osdVisible;
    this->update();
}

void VideoWindow::setOSD1FontSize(int osdFontSize)
{
    this->osd1FontSize = osdFontSize;
    this->update();
}

void VideoWindow::setOSD1Text(const QString &osdText)
{
    this->osd1Text = osdText;
    this->update();
}

void VideoWindow::setOSD1Color(const QColor &osdColor)
{
    this->osd1Color = osdColor;
    this->update();
}

void VideoWindow::setOSD1Image(const QImage &osdImage)
{
    this->osd1Image = osdImage;
    this->update();
}

void VideoWindow::setOSD1Format(const VideoWindow::OSDFormat &osdFormat)
{
    this->osd1Format = osdFormat;
    this->update();
}

void VideoWindow::setOSD1Position(const VideoWindow::OSDPosition &osdPosition)
{
    this->osd1Position = osdPosition;
    this->update();
}

void VideoWindow::setOSD2Visible(bool osdVisible)
{
    this->osd2Visible = osdVisible;
    this->update();
}

void VideoWindow::setOSD2FontSize(int osdFontSize)
{
    this->osd2FontSize = osdFontSize;
    this->update();
}

void VideoWindow::setOSD2Text(const QString &osdText)
{
    this->osd2Text = osdText;
    this->update();
}

void VideoWindow::setOSD2Color(const QColor &osdColor)
{
    this->osd2Color = osdColor;
    this->update();
}

void VideoWindow::setOSD2Image(const QImage &osdImage)
{
    this->osd2Image = osdImage;
    this->update();
}

void VideoWindow::setOSD2Format(const VideoWindow::OSDFormat &osdFormat)
{
    this->osd2Format = osdFormat;
    this->update();
}

void VideoWindow::setOSD2Position(const VideoWindow::OSDPosition &osdPosition)
{
    this->osd2Position = osdPosition;
    this->update();
}

void VideoWindow::setOSD1Format(quint8 osdFormat)
{
    setOSD1Format((VideoWindow::OSDFormat)osdFormat);
}

void VideoWindow::setOSD2Format(quint8 osdFormat)
{
    setOSD2Format((VideoWindow::OSDFormat)osdFormat);
}

void VideoWindow::setOSD1Position(quint8 osdPosition)
{
    setOSD1Position((VideoWindow::OSDPosition)osdPosition);
}

void VideoWindow::setOSD2Position(quint8 osdPosition)
{
    setOSD2Position((VideoWindow::OSDPosition)osdPosition);
}

void VideoWindow::setFaceBorder(int faceBorder)
{
    this->faceBorder = faceBorder;
    this->update();
}

void VideoWindow::setFaceColor(const QColor &faceColor)
{
    this->faceColor = faceColor;
    this->update();
}

void VideoWindow::setFaceRects(const QList<QRect> &faceRects)
{
    this->faceRects = faceRects;
    this->update();
}

void VideoWindow::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();
    clear();

    //如果是图片则只显示图片就行
    image = QImage(this->property("url").toString());
    if (!image.isNull()) {
        this->update();
        return;
    }

    //thread->play();
    //thread->start();

    if (checkLive) {
        timerCheck->start();
    }

    this->setProperty("isPause", false);
}

void VideoWindow::pause()
{
    if (!this->property("isPause").toBool()) {
        //thread->pause();
        this->setProperty("isPause", true);
    }
}

void VideoWindow::next()
{
    if (this->property("isPause").toBool()) {
        //thread->next();
        this->setProperty("isPause", false);
    }
}

void VideoWindow::close()
{
    if (checkLive) {
        timerCheck->stop();
    }

    this->clear();
    //QTimer::singleShot(5, this, SLOT(clear()));
}

void VideoWindow::restart(const QString &url, int delayOpen)
{
    //qDebug() << TIMEMS << "restart video" << objectName();
    //关闭视频
    close();
    //重新设置播放地址
    setUrl(url);

    //打开视频
    if (delayOpen > 0) {
        QTimer::singleShot(delayOpen, this, SLOT(open()));
    } else {
        open();
    }
}

void VideoWindow::clear()
{
    image = QImage();
    this->update();
}

void VideoWindow::snap(const QString &fileName)
{

}
