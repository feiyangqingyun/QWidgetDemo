#include "quiwidget.h"

#ifdef __arm__
#ifdef arma7
#define TOOL true
#else
#define TOOL false
#endif
#else
#define TOOL false
#endif

QUIWidget::QUIWidget(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

QUIWidget::~QUIWidget()
{
}

bool QUIWidget::eventFilter(QObject *watched, QEvent *event)
{
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
            if (this->property("canMove").toBool()) {
                this->move(mouseEvent->globalPos() - mousePoint);
            }
        }
    } else if (mouseEvent->type() == QEvent::MouseButtonDblClick) {
        //以下写法可以将双击识别限定在标题栏
        if (this->btnMenu_Max->isVisible() && watched == this->widgetTitle) {
            //if (this->btnMenu_Max->isVisible()) {
            this->on_btnMenu_Max_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
}

QLabel *QUIWidget::getLabIco() const
{
    return this->labIco;
}

QLabel *QUIWidget::getLabTitle() const
{
    return this->labTitle;
}

QToolButton *QUIWidget::getBtnMenu() const
{
    return this->btnMenu;
}

QPushButton *QUIWidget::getBtnMenuMin() const
{
    return this->btnMenu_Min;
}

QPushButton *QUIWidget::getBtnMenuMax() const
{
    return this->btnMenu_Max;
}

QPushButton *QUIWidget::getBtnMenuMClose() const
{
    return this->btnMenu_Close;
}

QString QUIWidget::getTitle() const
{
    return this->title;
}

Qt::Alignment QUIWidget::getAlignment() const
{
    return this->alignment;
}

bool QUIWidget::getMinHide() const
{
    return this->minHide;
}

bool QUIWidget::getExitAll() const
{
    return this->exitAll;
}

QSize QUIWidget::sizeHint() const
{
    return QSize(600, 450);
}

QSize QUIWidget::minimumSizeHint() const
{
    return QSize(200, 150);
}

void QUIWidget::initControl()
{
    this->setObjectName(QString::fromUtf8("QUIWidget"));
    this->resize(900, 750);
    verticalLayout1 = new QVBoxLayout(this);
    verticalLayout1->setSpacing(0);
    verticalLayout1->setContentsMargins(11, 11, 11, 11);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(1, 1, 1, 1);
    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setSpacing(0);
    verticalLayout2->setContentsMargins(11, 11, 11, 11);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->setContentsMargins(0, 0, 0, 0);
    widgetTitle = new QWidget(widgetMain);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);
    widgetTitle->setMinimumSize(QSize(0, 30));
    horizontalLayout4 = new QHBoxLayout(widgetTitle);
    horizontalLayout4->setSpacing(0);
    horizontalLayout4->setContentsMargins(11, 11, 11, 11);
    horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
    horizontalLayout4->setContentsMargins(0, 0, 0, 0);

    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
    labIco->setSizePolicy(sizePolicy1);
    labIco->setMinimumSize(QSize(30, 0));
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout4->addWidget(labIco);

    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labTitle->sizePolicy().hasHeightForWidth());
    labTitle->setSizePolicy(sizePolicy2);
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout4->addWidget(labTitle);

    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy1);
    horizontalLayout = new QHBoxLayout(widgetMenu);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    btnMenu = new QToolButton(widgetMenu);
    btnMenu->setObjectName(QString::fromUtf8("btnMenu"));
    QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu->sizePolicy().hasHeightForWidth());
    btnMenu->setSizePolicy(sizePolicy3);
    btnMenu->setMinimumSize(QSize(30, 0));
    btnMenu->setMaximumSize(QSize(30, 16777215));
    btnMenu->setFocusPolicy(Qt::NoFocus);
    btnMenu->setPopupMode(QToolButton::InstantPopup);
    horizontalLayout->addWidget(btnMenu);

    btnMenu_Min = new QPushButton(widgetMenu);
    btnMenu_Min->setObjectName(QString::fromUtf8("btnMenu_Min"));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(btnMenu_Min->sizePolicy().hasHeightForWidth());
    btnMenu_Min->setSizePolicy(sizePolicy4);
    btnMenu_Min->setMinimumSize(QSize(30, 0));
    btnMenu_Min->setMaximumSize(QSize(30, 16777215));
    btnMenu_Min->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Min->setFocusPolicy(Qt::NoFocus);
    horizontalLayout->addWidget(btnMenu_Min);

    btnMenu_Max = new QPushButton(widgetMenu);
    btnMenu_Max->setObjectName(QString::fromUtf8("btnMenu_Max"));
    sizePolicy3.setHeightForWidth(btnMenu_Max->sizePolicy().hasHeightForWidth());
    btnMenu_Max->setSizePolicy(sizePolicy3);
    btnMenu_Max->setMinimumSize(QSize(30, 0));
    btnMenu_Max->setMaximumSize(QSize(30, 16777215));
    btnMenu_Max->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Max->setFocusPolicy(Qt::NoFocus);
    horizontalLayout->addWidget(btnMenu_Max);

    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setMinimumSize(QSize(30, 0));
    btnMenu_Close->setMaximumSize(QSize(30, 16777215));
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    horizontalLayout->addWidget(btnMenu_Close);
    horizontalLayout4->addWidget(widgetMenu);
    verticalLayout2->addWidget(widgetTitle);

    widget = new QWidget(widgetMain);
    widget->setObjectName(QString::fromUtf8("widget"));
    verticalLayout3 = new QVBoxLayout(widget);
    verticalLayout3->setSpacing(0);
    verticalLayout3->setContentsMargins(11, 11, 11, 11);
    verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
    verticalLayout3->setContentsMargins(0, 0, 0, 0);
    verticalLayout2->addWidget(widget);
    verticalLayout1->addWidget(widgetMain);

    connect(this->btnMenu_Min, SIGNAL(clicked()), this, SLOT(on_btnMenu_Min_clicked()));
    connect(this->btnMenu_Max, SIGNAL(clicked()), this, SLOT(on_btnMenu_Max_clicked()));
    connect(this->btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIWidget::initForm()
{
    //设置图形字体
    setIcon(QUIWidget::Lab_Ico, QUIConfig::IconMain, 11);
    setIcon(QUIWidget::BtnMenu, QUIConfig::IconMenu);
    setIcon(QUIWidget::BtnMenu_Min, QUIConfig::IconMin);
    setIcon(QUIWidget::BtnMenu_Normal, QUIConfig::IconNormal);
    setIcon(QUIWidget::BtnMenu_Close, QUIConfig::IconClose);

    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->widgetTitle->setProperty("form", "title");
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);

    //设置标题及对齐方式
    title = "QUI Demo";
    alignment = Qt::AlignLeft | Qt::AlignVCenter;
    minHide = false;
    exitAll = true;
    mainWidget = 0;

    setVisible(QUIWidget::BtnMenu, false);

    //绑定事件过滤器监听鼠标移动
    this->installEventFilter(this);
    this->widgetTitle->installEventFilter(this);

    //添加换肤菜单
    QStringList styleNames;
    styleNames << "银色" << "蓝色" << "浅蓝色" << "深蓝色" << "灰色" << "浅灰色" << "深灰色" << "黑色"
               << "浅黑色" << "深黑色" << "PS黑色" << "黑色扁平" << "白色扁平" << "蓝色扁平" << "紫色" << "黑蓝色" << "视频黑";

    foreach (QString styleName, styleNames) {
        QAction *action = new QAction(styleName, this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle()));
        this->btnMenu->addAction(action);
    }
}

void QUIWidget::changeStyle()
{
    QAction *act = (QAction *)sender();
    QString name = act->text();
    QString qssFile = ":/qss/lightblue.css";

    if (name == "银色") {
        qssFile = ":/qss/silvery.css";
        QUIHelper::setStyle(QUIWidget::Style_Silvery);
    } else if (name == "蓝色") {
        qssFile = ":/qss/blue.css";
        QUIHelper::setStyle(QUIWidget::Style_Blue);
    } else if (name == "浅蓝色") {
        qssFile = ":/qss/lightblue.css";
        QUIHelper::setStyle(QUIWidget::Style_LightBlue);
    } else if (name == "深蓝色") {
        qssFile = ":/qss/darkblue.css";
        QUIHelper::setStyle(QUIWidget::Style_DarkBlue);
    } else if (name == "灰色") {
        qssFile = ":/qss/gray.css";
        QUIHelper::setStyle(QUIWidget::Style_Gray);
    } else if (name == "浅灰色") {
        qssFile = ":/qss/lightgray.css";
        QUIHelper::setStyle(QUIWidget::Style_LightGray);
    } else if (name == "深灰色") {
        qssFile = ":/qss/darkgray.css";
        QUIHelper::setStyle(QUIWidget::Style_DarkGray);
    } else if (name == "黑色") {
        qssFile = ":/qss/black.css";
        QUIHelper::setStyle(QUIWidget::Style_Black);
    } else if (name == "浅黑色") {
        qssFile = ":/qss/lightblack.css";
        QUIHelper::setStyle(QUIWidget::Style_LightBlack);
    } else if (name == "深黑色") {
        qssFile = ":/qss/darkblack.css";
        QUIHelper::setStyle(QUIWidget::Style_DarkBlack);
    } else if (name == "PS黑色") {
        qssFile = ":/qss/psblack.css";
        QUIHelper::setStyle(QUIWidget::Style_PSBlack);
    } else if (name == "黑色扁平") {
        qssFile = ":/qss/flatblack.css";
        QUIHelper::setStyle(QUIWidget::Style_FlatBlack);
    } else if (name == "白色扁平") {
        qssFile = ":/qss/flatwhite.css";
        QUIHelper::setStyle(QUIWidget::Style_FlatWhite);
    } else if (name == "蓝色扁平") {
        qssFile = ":/qss/flatblue.css";
        QUIHelper::setStyle(QUIWidget::Style_FlatBlue);
    } else if (name == "紫色") {
        qssFile = ":/qss/purple.css";
        QUIHelper::setStyle(QUIWidget::Style_Purple);
    } else if (name == "黑蓝色") {
        qssFile = ":/qss/blackblue.css";
        QUIHelper::setStyle(QUIWidget::Style_BlackBlue);
    } else if (name == "视频黑") {
        qssFile = ":/qss/blackvideo.css";
        QUIHelper::setStyle(QUIWidget::Style_BlackVideo);
    }

    emit changeStyle(qssFile);
}

void QUIWidget::setIcon(QUIWidget::Widget widget, const QChar &str, quint32 size)
{
    if (widget == QUIWidget::Lab_Ico) {
        setIconMain(str, size);
    } else if (widget == QUIWidget::BtnMenu) {
        QUIConfig::IconMenu = str;
        IconHelper::Instance()->setIcon(this->btnMenu, str, size);
    } else if (widget == QUIWidget::BtnMenu_Min) {
        QUIConfig::IconMin = str;
        IconHelper::Instance()->setIcon(this->btnMenu_Min, str, size);
    } else if (widget == QUIWidget::BtnMenu_Max) {
        QUIConfig::IconMax = str;
        IconHelper::Instance()->setIcon(this->btnMenu_Max, str, size);
    } else if (widget == QUIWidget::BtnMenu_Normal) {
        QUIConfig::IconNormal = str;
        IconHelper::Instance()->setIcon(this->btnMenu_Max, str, size);
    } else if (widget == QUIWidget::BtnMenu_Close) {
        QUIConfig::IconClose = str;
        IconHelper::Instance()->setIcon(this->btnMenu_Close, str, size);
    }
}

void QUIWidget::setIconMain(const QChar &str, quint32 size)
{
    QUIConfig::IconMain = str;
    IconHelper::Instance()->setIcon(this->labIco, str, size);
    QUIMessageBox::Instance()->setIconMain(str, size);
    QUIInputBox::Instance()->setIconMain(str, size);
    QUIDateSelect::Instance()->setIconMain(str, size);
}

void QUIWidget::setPixmap(QUIWidget::Widget widget, const QString &file, const QSize &size)
{
    //按照宽高比自动缩放
    QPixmap pix = QPixmap(file);
    pix = pix.scaled(size, Qt::KeepAspectRatio);
    if (widget == QUIWidget::Lab_Ico) {
        this->labIco->setPixmap(pix);
    } else if (widget == QUIWidget::BtnMenu) {
        this->btnMenu->setIcon(QIcon(file));
    } else if (widget == QUIWidget::BtnMenu_Min) {
        this->btnMenu_Min->setIcon(QIcon(file));
    } else if (widget == QUIWidget::BtnMenu_Max) {
        this->btnMenu_Max->setIcon(QIcon(file));
    } else if (widget == QUIWidget::BtnMenu_Close) {
        this->btnMenu_Close->setIcon(QIcon(file));
    }
}

void QUIWidget::setVisible(QUIWidget::Widget widget, bool visible)
{
    if (widget == QUIWidget::Lab_Ico) {
        this->labIco->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu) {
        this->btnMenu->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Min) {
        this->btnMenu_Min->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Max) {
        this->btnMenu_Max->setVisible(visible);
    } else if (widget == QUIWidget::BtnMenu_Close) {
        this->btnMenu_Close->setVisible(visible);
    }
}

void QUIWidget::setOnlyCloseBtn()
{
    this->btnMenu->setVisible(false);
    this->btnMenu_Min->setVisible(false);
    this->btnMenu_Max->setVisible(false);
}

void QUIWidget::setTitleHeight(int height)
{
    this->widgetTitle->setFixedHeight(height);
}

void QUIWidget::setBtnWidth(int width)
{
    this->labIco->setFixedWidth(width);
    this->btnMenu->setFixedWidth(width);
    this->btnMenu_Min->setFixedWidth(width);
    this->btnMenu_Max->setFixedWidth(width);
    this->btnMenu_Close->setFixedWidth(width);
}

void QUIWidget::setTitle(const QString &title)
{
    if (this->title != title) {
        this->title = title;
        this->labTitle->setText(title);
        this->setWindowTitle(this->labTitle->text());
    }
}

void QUIWidget::setAlignment(Qt::Alignment alignment)
{
    if (this->alignment != alignment) {
        this->alignment = alignment;
        this->labTitle->setAlignment(alignment);
    }
}

void QUIWidget::setMinHide(bool minHide)
{
    if (this->minHide != minHide) {
        this->minHide = minHide;
    }
}

void QUIWidget::setExitAll(bool exitAll)
{
    if (this->exitAll != exitAll) {
        this->exitAll = exitAll;
    }
}

void QUIWidget::setMainWidget(QWidget *mainWidget)
{
    //一个QUI窗体对象只能设置一个主窗体
    if (this->mainWidget == 0) {
        //将子窗体添加到布局
        this->widget->layout()->addWidget(mainWidget);
        //自动设置大小
        resize(mainWidget->width(), mainWidget->height() + this->widgetTitle->height());
        this->mainWidget = mainWidget;
    }
}

void QUIWidget::on_btnMenu_Min_clicked()
{
    if (minHide) {
        hide();
    } else {
        showMinimized();
    }
}

void QUIWidget::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
        setIcon(QUIWidget::BtnMenu_Normal, QUIConfig::IconNormal);
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        setIcon(QUIWidget::BtnMenu_Max, QUIConfig::IconMax);
    }

    this->setProperty("canMove", max);
    max = !max;
}

void QUIWidget::on_btnMenu_Close_clicked()
{
    //先发送关闭信号
    emit closing();
    mainWidget->close();
    if (exitAll) {
        this->close();
    }
}


QScopedPointer<QUIMessageBox> QUIMessageBox::self;
QUIMessageBox *QUIMessageBox::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new QUIMessageBox);
        }
    }

    return self.data();
}

QUIMessageBox::QUIMessageBox(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

QUIMessageBox::~QUIMessageBox()
{
    delete widgetMain;
}

void QUIMessageBox::showEvent(QShowEvent *)
{
    this->activateWindow();
}

void QUIMessageBox::closeEvent(QCloseEvent *)
{
    closeSec = 0;
    currentSec = 0;
}

bool QUIMessageBox::eventFilter(QObject *watched, QEvent *event)
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

void QUIMessageBox::initControl()
{
    this->setObjectName(QString::fromUtf8("QUIMessageBox"));

    verticalLayout1 = new QVBoxLayout(this);
    verticalLayout1->setSpacing(0);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(1, 1, 1, 1);

    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);

    horizontalLayout3 = new QHBoxLayout(widgetTitle);
    horizontalLayout3->setSpacing(0);
    horizontalLayout3->setObjectName(QString::fromUtf8("horizontalLayout3"));
    horizontalLayout3->setContentsMargins(0, 0, 0, 0);

    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
    labIco->setSizePolicy(sizePolicy1);
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout3->addWidget(labIco);

    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout3->addWidget(labTitle);

    labCountDown = new QLabel(widgetTitle);
    labCountDown->setObjectName(QString::fromUtf8("labCountDown"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labCountDown->sizePolicy().hasHeightForWidth());
    labCountDown->setSizePolicy(sizePolicy2);
    labCountDown->setAlignment(Qt::AlignCenter);
    horizontalLayout3->addWidget(labCountDown);

    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy1);

    horizontalLayout4 = new QHBoxLayout(widgetMenu);
    horizontalLayout4->setSpacing(0);
    horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
    horizontalLayout4->setContentsMargins(0, 0, 0, 0);

    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    btnMenu_Close->setFlat(true);

    horizontalLayout4->addWidget(btnMenu_Close);
    horizontalLayout3->addWidget(widgetMenu);
    verticalLayout1->addWidget(widgetTitle);

    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));

    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setSpacing(5);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->setContentsMargins(5, 5, 5, 5);

    frame = new QFrame(widgetMain);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);

    labIcoMain = new QLabel(frame);
    labIcoMain->setObjectName(QString::fromUtf8("labIcoMain"));
    labIcoMain->setAlignment(Qt::AlignCenter);

    verticalLayout4 = new QVBoxLayout(frame);
    verticalLayout4->setObjectName(QString::fromUtf8("verticalLayout4"));
    verticalLayout4->setContentsMargins(-1, 9, -1, -1);

    horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
    horizontalLayout1->addWidget(labIcoMain);
    horizontalSpacer1 = new QSpacerItem(5, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontalLayout1->addItem(horizontalSpacer1);

    labInfo = new QLabel(frame);
    labInfo->setObjectName(QString::fromUtf8("labInfo"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(labInfo->sizePolicy().hasHeightForWidth());
    labInfo->setSizePolicy(sizePolicy4);
    labInfo->setMinimumSize(QSize(0, TitleMinSize));
    labInfo->setScaledContents(false);
    labInfo->setWordWrap(true);
    horizontalLayout1->addWidget(labInfo);
    verticalLayout4->addLayout(horizontalLayout1);

    horizontalLayout2 = new QHBoxLayout();
    horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
    horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout2->addItem(horizontalSpacer2);

    btnOk = new QPushButton(frame);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setMinimumSize(QSize(85, 0));
    btnOk->setFocusPolicy(Qt::StrongFocus);
    horizontalLayout2->addWidget(btnOk);
    btnOk->setDefault(true);

    btnCancel = new QPushButton(frame);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setMinimumSize(QSize(85, 0));
    btnCancel->setFocusPolicy(Qt::StrongFocus);
    horizontalLayout2->addWidget(btnCancel);

    verticalLayout4->addLayout(horizontalLayout2);
    verticalLayout2->addWidget(frame);
    verticalLayout1->addWidget(widgetMain);

    widgetTitle->raise();
    widgetMain->raise();
    frame->raise();

    btnOk->setText("确定");
    btnCancel->setText("取消");
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnCancel, ":/image/btn_close.png", 0xf00d);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIMessageBox::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());
    this->setFixedSize(DialogMinWidth, DialogMinHeight);
    labIcoMain->setFixedSize(TitleMinSize, TitleMinSize);

#ifdef __arm__
    int width = 90;
    int iconWidth = 22;
    int iconHeight = 22;
#else
    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;
#endif

    QList<QPushButton *> btns = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }

    closeSec = 0;
    currentSec = 0;

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
    timer->start();

    this->installEventFilter(this);
}

void QUIMessageBox::checkSec()
{
    if (closeSec == 0) {
        return;
    }

    if (currentSec < closeSec) {
        currentSec++;
    } else {
        this->close();
    }

    QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
    this->labCountDown->setText(str);
}

void QUIMessageBox::on_btnOk_clicked()
{
    done(QMessageBox::Yes);
    close();
}

void QUIMessageBox::on_btnMenu_Close_clicked()
{
    done(QMessageBox::No);
    close();
}

void QUIMessageBox::setIconMain(const QChar &str, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, str, size);
}

void QUIMessageBox::setIconMsg(const QString &png, const QChar &str)
{
    //图片存在则取图片,不存在则取图形字体
    int size = this->labIcoMain->size().height();
    if (QImage(png).isNull()) {
        IconHelper::Instance()->setIcon(this->labIcoMain, str, size);
    } else {
        this->labIcoMain->setStyleSheet(QString("border-image:url(%1);").arg(png));
    }
}

void QUIMessageBox::setMessage(const QString &msg, int type, int closeSec)
{
    this->closeSec = closeSec;
    this->currentSec = 0;
    this->labCountDown->clear();
    checkSec();

    if (type == 0) {
        setIconMsg(":/image/msg_info.png", 0xf05a);
        this->btnCancel->setVisible(false);
        this->labTitle->setText("提示");
    } else if (type == 1) {
        setIconMsg(":/image/msg_question.png", 0xf059);
        this->labTitle->setText("询问");
    } else if (type == 2) {
        setIconMsg(":/image/msg_error.png", 0xf057);
        this->btnCancel->setVisible(false);
        this->labTitle->setText("错误");
    }

    this->labInfo->setText(msg);
    this->setWindowTitle(this->labTitle->text());
    //设置对话框的大小总以最合适的大小显示
    if (msg.length() < 70) {
        this->layout()->setSizeConstraint(QLayout::SetMinimumSize);
        this->setFixedSize(DialogMinWidth, DialogMinHeight);
    } else {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }
}


QScopedPointer<QUITipBox> QUITipBox::self;
QUITipBox *QUITipBox::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new QUITipBox);
        }
    }

    return self.data();
}

QUITipBox::QUITipBox(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
}

QUITipBox::~QUITipBox()
{
    delete widgetMain;
}

void QUITipBox::showEvent(QShowEvent *)
{
    this->activateWindow();
}

void QUITipBox::closeEvent(QCloseEvent *)
{
    closeSec = 0;
    currentSec = 0;
}

bool QUITipBox::eventFilter(QObject *watched, QEvent *event)
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

void QUITipBox::initControl()
{
    this->setObjectName(QString::fromUtf8("QUITipBox"));

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(1, 1, 1, 1);

    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);

    horizontalLayout2 = new QHBoxLayout(widgetTitle);
    horizontalLayout2->setSpacing(0);
    horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
    horizontalLayout2->setContentsMargins(0, 0, 0, 0);

    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout2->addWidget(labIco);

    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout2->addWidget(labTitle);

    labCountDown = new QLabel(widgetTitle);
    labCountDown->setObjectName(QString::fromUtf8("labCountDown"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labCountDown->sizePolicy().hasHeightForWidth());
    labCountDown->setSizePolicy(sizePolicy1);
    labCountDown->setAlignment(Qt::AlignCenter);
    horizontalLayout2->addWidget(labCountDown);

    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy2);

    horizontalLayout = new QHBoxLayout(widgetMenu);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    btnMenu_Close->setFlat(true);

    horizontalLayout->addWidget(btnMenu_Close);
    horizontalLayout2->addWidget(widgetMenu);
    verticalLayout->addWidget(widgetTitle);

    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));
    widgetMain->setAutoFillBackground(true);

    labInfo = new QLabel(widgetMain);
    labInfo->setObjectName(QString::fromUtf8("labInfo"));
    labInfo->setScaledContents(true);
    labInfo->setWordWrap(true);

    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->addWidget(labInfo);
    verticalLayout->addWidget(widgetMain);

    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUITipBox::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
    this->setFixedSize(350, 180);
#else
    this->setFixedSize(280, 150);
#endif

    closeSec = 0;
    currentSec = 0;

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
    timer->start();

    this->installEventFilter(this);

    //字体加大
    QFont font;
    font.setPixelSize(QUIConfig::FontSize + 3);
    font.setBold(true);
    this->labInfo->setFont(font);

    //显示和隐藏窗体动画效果
    animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(500);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
}

void QUITipBox::checkSec()
{
    if (closeSec == 0) {
        return;
    }

    if (currentSec < closeSec) {
        currentSec++;
    } else {
        this->close();
    }

    QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
    this->labCountDown->setText(str);
}

void QUITipBox::on_btnMenu_Close_clicked()
{
    done(QMessageBox::No);
    close();
}

void QUITipBox::setIconMain(const QChar &str, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, str, size);
}

void QUITipBox::setTip(const QString &title, const QString &tip, bool fullScreen, bool center, int closeSec)
{
    this->closeSec = closeSec;
    this->currentSec = 0;
    this->labCountDown->clear();
    checkSec();

    this->fullScreen = fullScreen;
    this->labTitle->setText(title);
    this->labInfo->setText(tip);
    this->labInfo->setAlignment(center ? Qt::AlignCenter : Qt::AlignLeft);
    this->setWindowTitle(this->labTitle->text());

    QRect rect = fullScreen ? qApp->desktop()->geometry() : qApp->desktop()->availableGeometry();
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width();
    int y = height - this->height();

    //移到右下角
    this->move(x, y);

    //启动动画
    animation->stop();
    animation->setStartValue(QPoint(x, height));
    animation->setEndValue(QPoint(x, y));
    animation->start();
}

void QUITipBox::hide()
{
    QRect rect = fullScreen ? qApp->desktop()->availableGeometry() : qApp->desktop()->geometry();
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width();
    int y = height - this->height();

    //启动动画
    animation->stop();
    animation->setStartValue(QPoint(x, y));
    animation->setEndValue(QPoint(x, qApp->desktop()->geometry().height()));
    animation->start();
}


QScopedPointer<QUIInputBox> QUIInputBox::self;
QUIInputBox *QUIInputBox::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new QUIInputBox);
        }
    }

    return self.data();
}

QUIInputBox::QUIInputBox(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

QUIInputBox::~QUIInputBox()
{
    delete widgetMain;
}

void QUIInputBox::showEvent(QShowEvent *)
{
    txtValue->setFocus();
    this->activateWindow();
}

void QUIInputBox::initControl()
{
    this->setObjectName(QString::fromUtf8("QUIInputBox"));

    verticalLayout1 = new QVBoxLayout(this);
    verticalLayout1->setSpacing(0);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(1, 1, 1, 1);

    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);

    horizontalLayout1 = new QHBoxLayout(widgetTitle);
    horizontalLayout1->setSpacing(0);
    horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
    horizontalLayout1->setContentsMargins(0, 0, 0, 0);

    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
    labIco->setSizePolicy(sizePolicy1);
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout1->addWidget(labIco);

    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout1->addWidget(labTitle);

    labCountDown = new QLabel(widgetTitle);
    labCountDown->setObjectName(QString::fromUtf8("labCountDown"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labCountDown->sizePolicy().hasHeightForWidth());
    labCountDown->setSizePolicy(sizePolicy2);
    labCountDown->setAlignment(Qt::AlignCenter);
    horizontalLayout1->addWidget(labCountDown);

    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy1);

    horizontalLayout2 = new QHBoxLayout(widgetMenu);
    horizontalLayout2->setSpacing(0);
    horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
    horizontalLayout2->setContentsMargins(0, 0, 0, 0);

    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    btnMenu_Close->setFlat(true);

    horizontalLayout2->addWidget(btnMenu_Close);
    horizontalLayout1->addWidget(widgetMenu);
    verticalLayout1->addWidget(widgetTitle);

    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));

    verticalLayout2 = new QVBoxLayout(widgetMain);
    verticalLayout2->setSpacing(5);
    verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
    verticalLayout2->setContentsMargins(5, 5, 5, 5);

    frame = new QFrame(widgetMain);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);

    labInfo = new QLabel(frame);
    labInfo->setObjectName(QString::fromUtf8("labInfo"));
    labInfo->setScaledContents(false);
    labInfo->setWordWrap(true);

    verticalLayout3 = new QVBoxLayout(frame);
    verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
    verticalLayout3->addWidget(labInfo);

    txtValue = new QLineEdit(frame);
    txtValue->setObjectName(QString::fromUtf8("txtValue"));
    verticalLayout3->addWidget(txtValue);

    cboxValue = new QComboBox(frame);
    cboxValue->setObjectName(QString::fromUtf8("cboxValue"));
    verticalLayout3->addWidget(cboxValue);

    lay = new QHBoxLayout();
    lay->setObjectName(QString::fromUtf8("lay"));
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    lay->addItem(horizontalSpacer);

    btnOk = new QPushButton(frame);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setMinimumSize(QSize(85, 0));
    lay->addWidget(btnOk);
    btnOk->setDefault(true);

    btnCancel = new QPushButton(frame);
    btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
    btnCancel->setMinimumSize(QSize(85, 0));
    lay->addWidget(btnCancel);

    verticalLayout3->addLayout(lay);
    verticalLayout2->addWidget(frame);
    verticalLayout1->addWidget(widgetMain);

    QWidget::setTabOrder(txtValue, btnOk);
    QWidget::setTabOrder(btnOk, btnCancel);

    labTitle->setText("输入框");
    btnOk->setText("确定");
    btnCancel->setText("取消");
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnCancel, ":/image/btn_close.png", 0xf00d);

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIInputBox::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
    int width = 90;
    int iconWidth = 22;
    int iconHeight = 22;
    this->setFixedSize(350, 180);
#else
    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;
    this->setFixedSize(280, 150);
#endif

    QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }

    closeSec = 0;
    currentSec = 0;

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkSec()));
    timer->start();

    this->installEventFilter(this);
}

void QUIInputBox::checkSec()
{
    if (closeSec == 0) {
        return;
    }

    if (currentSec < closeSec) {
        currentSec++;
    } else {
        this->close();
    }

    QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
    this->labCountDown->setText(str);
}

void QUIInputBox::setParameter(const QString &title, int type, int closeSec,
                               QString placeholderText, bool pwd,
                               const QString &defaultValue)
{
    this->closeSec = closeSec;
    this->currentSec = 0;
    this->labCountDown->clear();
    this->labInfo->setText(title);
    checkSec();

    if (type == 0) {
        this->cboxValue->setVisible(false);
        this->txtValue->setPlaceholderText(placeholderText);
        this->txtValue->setText(defaultValue);

        if (pwd) {
            this->txtValue->setEchoMode(QLineEdit::Password);
        }
    } else if (type == 1) {
        this->txtValue->setVisible(false);
        this->cboxValue->addItems(defaultValue.split("|"));
    }
}

QString QUIInputBox::getValue() const
{
    return this->value;
}

void QUIInputBox::closeEvent(QCloseEvent *)
{
    closeSec = 0;
    currentSec = 0;
}

bool QUIInputBox::eventFilter(QObject *watched, QEvent *event)
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

void QUIInputBox::on_btnOk_clicked()
{
    if (this->txtValue->isVisible()) {
        value = this->txtValue->text();
    } else if (this->cboxValue->isVisible()) {
        value = this->cboxValue->currentText();
    }

    done(QMessageBox::Ok);
    close();
}

void QUIInputBox::on_btnMenu_Close_clicked()
{
    done(QMessageBox::Cancel);
    close();
}

void QUIInputBox::setIconMain(const QChar &str, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, str, size);
}


QScopedPointer<QUIDateSelect> QUIDateSelect::self;
QUIDateSelect *QUIDateSelect::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new QUIDateSelect);
        }
    }

    return self.data();
}

QUIDateSelect::QUIDateSelect(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();
    QUIHelper::setFormInCenter(this);
}

QUIDateSelect::~QUIDateSelect()
{
    delete widgetMain;
}

void QUIDateSelect::showEvent(QShowEvent *)
{
    this->activateWindow();
}

bool QUIDateSelect::eventFilter(QObject *watched, QEvent *event)
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

void QUIDateSelect::initControl()
{
    this->setObjectName(QString::fromUtf8("QUIDateSelect"));

    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(1, 1, 1, 1);

    widgetTitle = new QWidget(this);
    widgetTitle->setObjectName(QString::fromUtf8("widgetTitle"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(widgetTitle->sizePolicy().hasHeightForWidth());
    widgetTitle->setSizePolicy(sizePolicy);

    horizontalLayout1 = new QHBoxLayout(widgetTitle);
    horizontalLayout1->setSpacing(0);
    horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
    horizontalLayout1->setContentsMargins(0, 0, 0, 0);

    labIco = new QLabel(widgetTitle);
    labIco->setObjectName(QString::fromUtf8("labIco"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(labIco->sizePolicy().hasHeightForWidth());
    labIco->setSizePolicy(sizePolicy1);
    labIco->setAlignment(Qt::AlignCenter);
    horizontalLayout1->addWidget(labIco);

    labTitle = new QLabel(widgetTitle);
    labTitle->setObjectName(QString::fromUtf8("labTitle"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(labTitle->sizePolicy().hasHeightForWidth());
    labTitle->setSizePolicy(sizePolicy2);
    labTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    horizontalLayout1->addWidget(labTitle);

    widgetMenu = new QWidget(widgetTitle);
    widgetMenu->setObjectName(QString::fromUtf8("widgetMenu"));
    sizePolicy1.setHeightForWidth(widgetMenu->sizePolicy().hasHeightForWidth());
    widgetMenu->setSizePolicy(sizePolicy1);

    horizontalLayout = new QHBoxLayout(widgetMenu);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    btnMenu_Close = new QPushButton(widgetMenu);
    btnMenu_Close->setObjectName(QString::fromUtf8("btnMenu_Close"));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(btnMenu_Close->sizePolicy().hasHeightForWidth());
    btnMenu_Close->setSizePolicy(sizePolicy3);
    btnMenu_Close->setCursor(QCursor(Qt::ArrowCursor));
    btnMenu_Close->setFocusPolicy(Qt::NoFocus);
    btnMenu_Close->setFlat(true);

    horizontalLayout->addWidget(btnMenu_Close);
    horizontalLayout1->addWidget(widgetMenu);
    verticalLayout->addWidget(widgetTitle);

    widgetMain = new QWidget(this);
    widgetMain->setObjectName(QString::fromUtf8("widgetMainQUI"));

    verticalLayout1 = new QVBoxLayout(widgetMain);
    verticalLayout1->setSpacing(6);
    verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
    verticalLayout1->setContentsMargins(6, 6, 6, 6);

    frame = new QFrame(widgetMain);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Sunken);

    gridLayout = new QGridLayout(frame);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    labStart = new QLabel(frame);
    labStart->setObjectName(QString::fromUtf8("labStart"));
    labStart->setFocusPolicy(Qt::TabFocus);
    gridLayout->addWidget(labStart, 0, 0, 1, 1);

    btnOk = new QPushButton(frame);
    btnOk->setObjectName(QString::fromUtf8("btnOk"));
    btnOk->setMinimumSize(QSize(85, 0));
    btnOk->setCursor(QCursor(Qt::PointingHandCursor));
    btnOk->setFocusPolicy(Qt::StrongFocus);
    gridLayout->addWidget(btnOk, 0, 2, 1, 1);
    btnOk->setDefault(true);

    labEnd = new QLabel(frame);
    labEnd->setObjectName(QString::fromUtf8("labEnd"));
    labEnd->setFocusPolicy(Qt::TabFocus);
    gridLayout->addWidget(labEnd, 1, 0, 1, 1);

    btnClose = new QPushButton(frame);
    btnClose->setObjectName(QString::fromUtf8("btnClose"));
    btnClose->setMinimumSize(QSize(85, 0));
    btnClose->setCursor(QCursor(Qt::PointingHandCursor));
    btnClose->setFocusPolicy(Qt::StrongFocus);
    gridLayout->addWidget(btnClose, 1, 2, 1, 1);

    dateStart = new QDateTimeEdit(frame);
    dateStart->setObjectName(QString::fromUtf8("dateStart"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(dateStart->sizePolicy().hasHeightForWidth());
    dateStart->setSizePolicy(sizePolicy4);
    dateStart->setCalendarPopup(true);
    gridLayout->addWidget(dateStart, 0, 1, 1, 1);

    dateEnd = new QDateTimeEdit(frame);
    dateEnd->setObjectName(QString::fromUtf8("dateEnd"));
    sizePolicy4.setHeightForWidth(dateEnd->sizePolicy().hasHeightForWidth());
    dateEnd->setSizePolicy(sizePolicy4);
    dateEnd->setCalendarPopup(true);

    gridLayout->addWidget(dateEnd, 1, 1, 1, 1);
    verticalLayout1->addWidget(frame);
    verticalLayout->addWidget(widgetMain);

    QWidget::setTabOrder(labStart, labEnd);
    QWidget::setTabOrder(labEnd, dateStart);
    QWidget::setTabOrder(dateStart, dateEnd);
    QWidget::setTabOrder(dateEnd, btnOk);
    QWidget::setTabOrder(btnOk, btnClose);

    labTitle->setText("日期时间选择");
    labStart->setText("开始时间");
    labEnd->setText("结束时间");
    btnOk->setText("确定");
    btnClose->setText("关闭");
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnClose, ":/image/btn_close.png", 0xf00d);

    dateStart->setDate(QDate::currentDate());
    dateEnd->setDate(QDate::currentDate().addDays(1));

    dateStart->calendarWidget()->setGridVisible(true);
    dateEnd->calendarWidget()->setGridVisible(true);
    dateStart->calendarWidget()->setLocale(QLocale::Chinese);
    dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
    setFormat("yyyy-MM-dd");

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIDateSelect::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());

#ifdef __arm__
    int width = 90;
    int iconWidth = 22;
    int iconHeight = 22;
    this->setFixedSize(370, 160);
#else
    int width = 80;
    int iconWidth = 18;
    int iconHeight = 18;
    this->setFixedSize(320, 130);
#endif

    QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(width);
        btn->setIconSize(QSize(iconWidth, iconHeight));
    }

    this->installEventFilter(this);
}

void QUIDateSelect::on_btnOk_clicked()
{
    if (dateStart->dateTime() > dateEnd->dateTime()) {
        QUIHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
        return;
    }

    startDateTime = dateStart->dateTime().toString(format);
    endDateTime = dateEnd->dateTime().toString(format);

    done(QMessageBox::Ok);
    close();
}

void QUIDateSelect::on_btnMenu_Close_clicked()
{
    done(QMessageBox::Cancel);
    close();
}

QString QUIDateSelect::getStartDateTime() const
{
    return this->startDateTime;
}

QString QUIDateSelect::getEndDateTime() const
{
    return this->endDateTime;
}

void QUIDateSelect::setIconMain(const QChar &str, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, str, size);
}

void QUIDateSelect::setFormat(const QString &format)
{
    this->format = format;
    this->dateStart->setDisplayFormat(format);
    this->dateEnd->setDisplayFormat(format);
}


QScopedPointer<IconHelper> IconHelper::self;
IconHelper *IconHelper::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new IconHelper);
        }
    }

    return self.data();
}

IconHelper::IconHelper(QObject *parent) : QObject(parent)
{
    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/image/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }
}

QFont IconHelper::getIconFont()
{
    return this->iconFont;
}

void IconHelper::setIcon(QLabel *lab, const QChar &str, quint32 size)
{
    iconFont.setPixelSize(size);
    lab->setFont(iconFont);
    lab->setText(str);
}

void IconHelper::setIcon(QAbstractButton *btn, const QChar &str, quint32 size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText(str);
}

QPixmap IconHelper::getPixmap(const QColor &color, const QChar &str, quint32 size,
                              quint32 pixWidth, quint32 pixHeight, int flags)
{
    QPixmap pix(pixWidth, pixHeight);
    pix.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(color);

    iconFont.setPixelSize(size);
    painter.setFont(iconFont);
    painter.drawText(pix.rect(), flags, str);
    painter.end();

    return pix;
}

QPixmap IconHelper::getPixmap(QToolButton *btn, bool normal)
{
    QPixmap pix;
    int index = btns.indexOf(btn);
    if (index >= 0) {
        if (normal) {
            pix = pixNormal.at(index);
        } else {
            pix = pixDark.at(index);
        }
    }

    return pix;
}

QPixmap IconHelper::getPixmap(QToolButton *btn, int type)
{
    QPixmap pix;
    int index = btns.indexOf(btn);
    if (index >= 0) {
        if (type == 0) {
            pix = pixNormal.at(index);
        } else if (type == 1) {
            pix = pixHover.at(index);
        } else if (type == 2) {
            pix = pixPressed.at(index);
        } else if (type == 3) {
            pix = pixChecked.at(index);
        }
    }

    return pix;
}

void IconHelper::setStyle(QFrame *frame, QList<QToolButton *> btns, QList<int> pixChar,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = pixChar.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QStringList qss;
    qss.append(QString("QFrame>QToolButton{border-style:none;border-width:0px;"
                       "background-color:%1;color:%2;}").arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QFrame>QToolButton:hover,QFrame>QToolButton:pressed,QFrame>QToolButton:checked"
                       "{background-color:%1;color:%2;}").arg(darkBgColor).arg(darkTextColor));

    frame->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QChar c = QChar(pixChar.at(i));
        QPixmap pixNormal = getPixmap(normalTextColor, c, iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, c, iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
        this->pixHover.append(pixDark);
        this->pixPressed.append(pixDark);
        this->pixChecked.append(pixDark);
    }
}

void IconHelper::setStyle(QWidget *widget, const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    QStringList qss;
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;"
                       "color:%2;background:%3;}").arg(type).arg(normalTextColor).arg(normalBgColor));

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    widget->setStyleSheet(qss.join(""));
}

void IconHelper::removeStyle(QList<QToolButton *> btns)
{
    for (int i = 0; i < btns.count(); i++) {
        for (int j = 0; j < this->btns.count(); j++) {
            if (this->btns.at(j) == btns.at(i)) {
                this->btns.at(j)->removeEventFilter(this);
                this->btns.removeAt(j);
                this->pixNormal.removeAt(j);
                this->pixDark.removeAt(j);
                this->pixHover.removeAt(j);
                this->pixPressed.removeAt(j);
                this->pixChecked.removeAt(j);
                break;
            }
        }
    }
}

void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> pixChar,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = pixChar.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    QStringList qss;
    if (btns.at(0)->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
                   .arg(type).arg(strBorder).arg(normalBgColor).arg(normalTextColor).arg(normalBgColor));
    } else {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
                   .arg(type).arg(normalTextColor).arg(normalBgColor));
    }

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    qss.append(QString("QWidget#%1{background:%2;}").arg(widget->objectName()).arg(normalBgColor));
    qss.append(QString("QWidget>QToolButton{border-width:0px;"
                       "background-color:%1;color:%2;}").arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QWidget>QToolButton:hover,QWidget>QToolButton:pressed,QWidget>QToolButton:checked{"
                       "background-color:%1;color:%2;}").arg(darkBgColor).arg(darkTextColor));

    widget->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QChar c = QChar(pixChar.at(i));
        QPixmap pixNormal = getPixmap(normalTextColor, c, iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, c, iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
        this->pixHover.append(pixDark);
        this->pixPressed.append(pixDark);
        this->pixChecked.append(pixDark);
    }
}

void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> pixChar, const IconHelper::StyleColor &styleColor)
{
    int btnCount = btns.count();
    int charCount = pixChar.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    quint32 iconSize = styleColor.iconSize;
    quint32 iconWidth = styleColor.iconWidth;
    quint32 iconHeight = styleColor.iconHeight;
    quint32 borderWidth = styleColor.borderWidth;
    QString type = styleColor.type;

    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding-top:%1px;padding-bottom:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding-right:%1px;padding-left:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding-bottom:%1px;padding-top:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding-left:%1px;padding-right:%2px;")
                    .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    QStringList qss;
    if (btns.at(0)->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
                   .arg(type).arg(strBorder).arg(styleColor.normalBgColor).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor));
    } else {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
                   .arg(type).arg(styleColor.normalTextColor).arg(styleColor.normalBgColor));
    }

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.hoverTextColor).arg(styleColor.hoverBgColor));
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:pressed{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.pressedTextColor).arg(styleColor.pressedBgColor));
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:checked{border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(styleColor.borderColor).arg(styleColor.checkedTextColor).arg(styleColor.checkedBgColor));

    qss.append(QString("QWidget#%1{background:%2;}").arg(widget->objectName()).arg(styleColor.normalBgColor));
    qss.append(QString("QWidget>QToolButton{border-width:0px;background-color:%1;color:%2;}").arg(styleColor.normalBgColor).arg(styleColor.normalTextColor));
    qss.append(QString("QWidget>QToolButton:hover{background-color:%1;color:%2;}").arg(styleColor.hoverBgColor).arg(styleColor.hoverTextColor));
    qss.append(QString("QWidget>QToolButton:pressed{background-color:%1;color:%2;}").arg(styleColor.pressedBgColor).arg(styleColor.pressedTextColor));
    qss.append(QString("QWidget>QToolButton:checked{background-color:%1;color:%2;}").arg(styleColor.checkedBgColor).arg(styleColor.checkedTextColor));

    widget->setStyleSheet(qss.join(""));

    //存储对应按钮对象,方便鼠标移上去的时候切换图片
    for (int i = 0; i < btnCount; i++) {
        QChar c = QChar(pixChar.at(i));
        QPixmap pixNormal = getPixmap(styleColor.normalTextColor, c, iconSize, iconWidth, iconHeight);
        QPixmap pixHover = getPixmap(styleColor.hoverTextColor, c, iconSize, iconWidth, iconHeight);
        QPixmap pixPressed = getPixmap(styleColor.pressedTextColor, c, iconSize, iconWidth, iconHeight);
        QPixmap pixChecked = getPixmap(styleColor.checkedTextColor, c, iconSize, iconWidth, iconHeight);

        QToolButton *btn = btns.at(i);
        btn->setIcon(QIcon(pixNormal));
        btn->setIconSize(QSize(iconWidth, iconHeight));
        btn->installEventFilter(this);

        this->btns.append(btn);
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixHover);
        this->pixHover.append(pixHover);
        this->pixPressed.append(pixPressed);
        this->pixChecked.append(pixChecked);
    }
}

bool IconHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched->inherits("QToolButton")) {
        QToolButton *btn = (QToolButton *)watched;
        int index = btns.indexOf(btn);
        if (index >= 0) {
            if (event->type() == QEvent::Enter) {
                btn->setIcon(QIcon(pixHover.at(index)));
            } else if (event->type() == QEvent::MouseButtonPress) {
                btn->setIcon(QIcon(pixPressed.at(index)));
            } else if (event->type() == QEvent::Leave) {
                if (btn->isChecked()) {
                    btn->setIcon(QIcon(pixChecked.at(index)));
                } else {
                    btn->setIcon(QIcon(pixNormal.at(index)));
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}


QScopedPointer<TrayIcon> TrayIcon::self;
TrayIcon *TrayIcon::Instance()
{
    if (self.isNull()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (self.isNull()) {
            self.reset(new TrayIcon);
        }
    }

    return self.data();
}

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    mainWidget = 0;
    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    menu = new QMenu(QApplication::desktop());
    exitDirect = true;
}

void TrayIcon::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick: {
            mainWidget->showNormal();
            break;
        }

        default:
            break;
    }
}

void TrayIcon::setExitDirect(bool exitDirect)
{
    if (this->exitDirect != exitDirect) {
        this->exitDirect = exitDirect;
    }
}

void TrayIcon::setMainWidget(QWidget *mainWidget)
{
    this->mainWidget = mainWidget;
    menu->addAction("主界面", mainWidget, SLOT(showNormal()));

    if (exitDirect) {
        menu->addAction("退出", this, SLOT(closeAll()));
    } else {
        menu->addAction("退出", this, SIGNAL(trayIconExit()));
    }

    trayIcon->setContextMenu(menu);
}

void TrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msecs)
{
    trayIcon->showMessage(title, msg, icon, msecs);
}

void TrayIcon::setIcon(const QString &strIcon)
{
    trayIcon->setIcon(QIcon(strIcon));
}

void TrayIcon::setToolTip(const QString &tip)
{
    trayIcon->setToolTip(tip);
}

void TrayIcon::setVisible(bool visible)
{
    trayIcon->setVisible(visible);
}

void TrayIcon::closeAll()
{
    trayIcon->hide();
    trayIcon->deleteLater();
    exit(0);
}


int QUIHelper::deskWidth()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int width = 0;
    if (width == 0) {
        width = qApp->desktop()->availableGeometry().width();
    }

    return width;
}

int QUIHelper::deskHeight()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static int height = 0;
    if (height == 0) {
        height = qApp->desktop()->availableGeometry().height();
    }

    return height;
}

QString QUIHelper::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
    }

    return name;
}

QString QUIHelper::appPath()
{
#ifdef Q_OS_ANDROID
    //return QString("/sdcard/Android/%1").arg(appName());
    return QString("/storage/emulated/0/%1").arg(appName());
#else
    return qApp->applicationDirPath();
#endif
}

void QUIHelper::initRand()
{
    //初始化随机数种子
    QTime t = QTime::currentTime();
    qsrand(t.msec() + t.second() * 1000);
}

QString QUIHelper::getUuid()
{
    QString uuid = QUuid::createUuid().toString();
    uuid = uuid.replace("{", "");
    uuid = uuid.replace("}", "");
    return uuid;
}

void QUIHelper::initDb(const QString &dbName)
{
    initFile(QString(":/%1.db").arg(appName()), dbName);
}

void QUIHelper::initFile(const QString &sourceName, const QString &targetName)
{
    //判断文件是否存在,不存在则从资源文件复制出来
    QFile file(targetName);
    if (!file.exists() || file.size() == 0) {
        file.remove();
        QUIHelper::copyFile(sourceName, targetName);
    }
}

void QUIHelper::setIconBtn(QAbstractButton *btn, const QString &png, const QChar &str)
{
    int size = 16;
    int width = 18;
    int height = 18;
    QPixmap pix;
    if (QPixmap(png).isNull()) {
        pix = IconHelper::Instance()->getPixmap(QUIConfig::TextColor, str, size, width, height);
    } else {
        pix = QPixmap(png);
    }

    btn->setIconSize(QSize(width, height));
    btn->setIcon(QIcon(pix));
}

void QUIHelper::newDir(const QString &dirName)
{
    QString strDir = dirName;

    //如果路径中包含斜杠字符则说明是绝对路径
    //linux系统路径字符带有 /  windows系统 路径字符带有 :/
    if (!strDir.startsWith("/") && !strDir.contains(":/")) {
        strDir = QString("%1/%2").arg(QUIHelper::appPath()).arg(strDir);
    }

    QDir dir(strDir);
    if (!dir.exists()) {
        dir.mkpath(strDir);
    }
}

void QUIHelper::writeInfo(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runinfo_%4.txt").arg(QUIHelper::appPath())
                       .arg(filePath).arg(QUIHelper::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}

void QUIHelper::writeError(const QString &info, bool needWrite, const QString &filePath)
{
    if (!needWrite) {
        return;
    }

    QString fileName = QString("%1/%2/%3_runerror_%4.txt").arg(QUIHelper::appPath())
                       .arg(filePath).arg(QUIHelper::appName()).arg(QDate::currentDate().toString("yyyyMM"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << info << NEWLINE;
    file.close();
}

void QUIHelper::setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle, QLabel *labIco, QPushButton *btnClose, bool tool)
{
    labIco->setFixedWidth(TitleMinSize);
    btnClose->setFixedWidth(TitleMinSize);
    widgetTitle->setFixedHeight(TitleMinSize);
    widgetTitle->setProperty("form", "title");
    widgetMain->setProperty("form", true);
    widgetMain->setProperty("canMove", true);

#ifdef __arm__
    if (tool) {
        widgetMain->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    } else {
        widgetMain->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    }
#else
    if (tool) {
        widgetMain->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    } else {
        widgetMain->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    }
#endif

    IconHelper::Instance()->setIcon(labIco, QUIConfig::IconMain, QUIConfig::FontSize + 2);
    IconHelper::Instance()->setIcon(btnClose, QUIConfig::IconClose, QUIConfig::FontSize);
}

void QUIHelper::setStyle(QUIWidget::Style style)
{
    QString qssFile = ":/qss/lightblue.css";
    if (style == QUIWidget::Style_Silvery) {
        qssFile = ":/qss/silvery.css";
    } else if (style == QUIWidget::Style_Blue) {
        qssFile = ":/qss/blue.css";
    } else if (style == QUIWidget::Style_LightBlue) {
        qssFile = ":/qss/lightblue.css";
    } else if (style == QUIWidget::Style_DarkBlue) {
        qssFile = ":/qss/darkblue.css";
    } else if (style == QUIWidget::Style_Gray) {
        qssFile = ":/qss/gray.css";
    } else if (style == QUIWidget::Style_LightGray) {
        qssFile = ":/qss/lightgray.css";
    } else if (style == QUIWidget::Style_DarkGray) {
        qssFile = ":/qss/darkgray.css";
    } else if (style == QUIWidget::Style_Black) {
        qssFile = ":/qss/black.css";
    } else if (style == QUIWidget::Style_LightBlack) {
        qssFile = ":/qss/lightblack.css";
    } else if (style == QUIWidget::Style_DarkBlack) {
        qssFile = ":/qss/darkblack.css";
    } else if (style == QUIWidget::Style_PSBlack) {
        qssFile = ":/qss/psblack.css";
    } else if (style == QUIWidget::Style_FlatBlack) {
        qssFile = ":/qss/flatblack.css";
    } else if (style == QUIWidget::Style_FlatWhite) {
        qssFile = ":/qss/flatwhite.css";
    } else if (style == QUIWidget::Style_Purple) {
        qssFile = ":/qss/purple.css";
    } else if (style == QUIWidget::Style_BlackBlue) {
        qssFile = ":/qss/blackblue.css";
    } else if (style == QUIWidget::Style_BlackVideo) {
        qssFile = ":/qss/blackvideo.css";
    }

    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                    QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);

        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void QUIHelper::setStyle(const QString &qssFile, QString &paletteColor, QString &textColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        paletteColor = qss.mid(20, 7);
        textColor = qss.mid(49, 7);
        getQssColor(qss, QUIConfig::TextColor, QUIConfig::PanelColor, QUIConfig::BorderColor, QUIConfig::NormalColorStart,
                    QUIConfig::NormalColorEnd, QUIConfig::DarkColorStart, QUIConfig::DarkColorEnd, QUIConfig::HighColor);

        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void QUIHelper::setStyle(const QString &qssFile, QString &textColor, QString &panelColor, QString &borderColor,
                         QString &normalColorStart, QString &normalColorEnd,
                         QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        getQssColor(qss, textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor);
        qApp->setPalette(QPalette(QColor(panelColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}

void QUIHelper::getQssColor(const QString &qss, QString &textColor, QString &panelColor, QString &borderColor,
                            QString &normalColorStart, QString &normalColorEnd,
                            QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QString str = qss;
    QString flagTextColor = "TextColor:";
    int indexTextColor = str.indexOf(flagTextColor);
    if (indexTextColor >= 0) {
        textColor = str.mid(indexTextColor + flagTextColor.length(), 7);
    }

    QString flagPanelColor = "PanelColor:";
    int indexPanelColor = str.indexOf(flagPanelColor);
    if (indexPanelColor >= 0) {
        panelColor = str.mid(indexPanelColor + flagPanelColor.length(), 7);
    }

    QString flagBorderColor = "BorderColor:";
    int indexBorderColor = str.indexOf(flagBorderColor);
    if (indexBorderColor >= 0) {
        borderColor = str.mid(indexBorderColor + flagBorderColor.length(), 7);
    }

    QString flagNormalColorStart = "NormalColorStart:";
    int indexNormalColorStart = str.indexOf(flagNormalColorStart);
    if (indexNormalColorStart >= 0) {
        normalColorStart = str.mid(indexNormalColorStart + flagNormalColorStart.length(), 7);
    }

    QString flagNormalColorEnd = "NormalColorEnd:";
    int indexNormalColorEnd = str.indexOf(flagNormalColorEnd);
    if (indexNormalColorEnd >= 0) {
        normalColorEnd = str.mid(indexNormalColorEnd + flagNormalColorEnd.length(), 7);
    }

    QString flagDarkColorStart = "DarkColorStart:";
    int indexDarkColorStart = str.indexOf(flagDarkColorStart);
    if (indexDarkColorStart >= 0) {
        darkColorStart = str.mid(indexDarkColorStart + flagDarkColorStart.length(), 7);
    }

    QString flagDarkColorEnd = "DarkColorEnd:";
    int indexDarkColorEnd = str.indexOf(flagDarkColorEnd);
    if (indexDarkColorEnd >= 0) {
        darkColorEnd = str.mid(indexDarkColorEnd + flagDarkColorEnd.length(), 7);
    }

    QString flagHighColor = "HighColor:";
    int indexHighColor = str.indexOf(flagHighColor);
    if (indexHighColor >= 0) {
        highColor = str.mid(indexHighColor + flagHighColor.length(), 7);
    }
}

QPixmap QUIHelper::ninePatch(const QString &picName, int horzSplit, int vertSplit, int dstWidth, int dstHeight)
{
    QPixmap pix(picName);
    return ninePatch(pix, horzSplit, vertSplit, dstWidth, dstHeight);
}

QPixmap QUIHelper::ninePatch(const QPixmap &pix, int horzSplit, int vertSplit, int dstWidth, int dstHeight)
{
    int pixWidth = pix.width();
    int pixHeight = pix.height();

    QPixmap pix1 = pix.copy(0, 0, horzSplit, vertSplit);
    QPixmap pix2 = pix.copy(horzSplit, 0, pixWidth - horzSplit * 2, vertSplit);
    QPixmap pix3 = pix.copy(pixWidth - horzSplit, 0, horzSplit, vertSplit);

    QPixmap pix4 = pix.copy(0, vertSplit, horzSplit, pixHeight - vertSplit * 2);
    QPixmap pix5 = pix.copy(horzSplit, vertSplit, pixWidth - horzSplit * 2, pixHeight - vertSplit * 2);
    QPixmap pix6 = pix.copy(pixWidth - horzSplit, vertSplit, horzSplit, pixHeight - vertSplit * 2);

    QPixmap pix7 = pix.copy(0, pixHeight - vertSplit, horzSplit, vertSplit);
    QPixmap pix8 = pix.copy(horzSplit, pixHeight - vertSplit, pixWidth - horzSplit * 2, pixWidth - horzSplit * 2);
    QPixmap pix9 = pix.copy(pixWidth - horzSplit, pixHeight - vertSplit, horzSplit, vertSplit);

    //保持高度拉宽
    pix2 = pix2.scaled(dstWidth - horzSplit * 2, vertSplit, Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix4 = pix4.scaled(horzSplit, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //宽高都缩放
    pix5 = pix5.scaled(dstWidth - horzSplit * 2, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //保持宽度拉高
    pix6 = pix6.scaled(horzSplit, dstHeight - vertSplit * 2, Qt::IgnoreAspectRatio);
    //保持高度拉宽
    pix8 = pix8.scaled(dstWidth - horzSplit * 2, vertSplit);

    //生成宽高图片并填充透明背景颜色
    QPixmap resultImg(dstWidth, dstHeight);
    resultImg.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&resultImg);
    if (!resultImg.isNull()) {
        painter.drawPixmap(0, 0, pix1);
        painter.drawPixmap(horzSplit, 0, pix2);
        painter.drawPixmap(dstWidth - horzSplit, 0, pix3);

        painter.drawPixmap(0, vertSplit, pix4);
        painter.drawPixmap(horzSplit, vertSplit, pix5);
        painter.drawPixmap(dstWidth - horzSplit, vertSplit, pix6);

        painter.drawPixmap(0, dstHeight - vertSplit, pix7);
        painter.drawPixmap(horzSplit, dstHeight - vertSplit, pix8);
        painter.drawPixmap(dstWidth - horzSplit, dstHeight - vertSplit, pix9);
    }

    painter.end();
    return resultImg;
}

void QUIHelper::setLabStyle(QLabel *lab, quint8 type, const QString &bgColor, const QString &textColor)
{
    QString colorBg = bgColor;
    QString colorText = textColor;

    //如果设置了新颜色则启用新颜色
    if (bgColor.isEmpty() || textColor.isEmpty()) {
        if (type == 0) {
            colorBg = "#D64D54";
            colorText = "#FFFFFF";
        } else if (type == 1) {
            colorBg = "#17A086";
            colorText = "#FFFFFF";
        } else if (type == 2) {
            colorBg = "#47A4E9";
            colorText = "#FFFFFF";
        } else if (type == 3) {
            colorBg = "#282D30";
            colorText = "#FFFFFF";
        } else if (type == 4) {
            colorBg = "#0E99A0";
            colorText = "#FFFFFF";
        } else if (type == 5) {
            colorBg = "#A279C5";
            colorText = "#FFFFFF";
        } else if (type == 6) {
            colorBg = "#8C2957";
            colorText = "#FFFFFF";
        } else if (type == 7) {
            colorBg = "#04567E";
            colorText = "#FFFFFF";
        } else if (type == 8) {
            colorBg = "#FD8B28";
            colorText = "#FFFFFF";
        } else if (type == 9) {
            colorBg = "#5580A2";
            colorText = "#FFFFFF";
        }
    }

    QStringList qss;
    //禁用颜色
    qss << QString("QLabel::disabled{background:none;color:%1;}").arg(QUIConfig::BorderColor);
    //正常颜色
    qss << QString("QLabel{border:none;background-color:%1;color:%2;}").arg(colorBg).arg(colorText);
    lab->setStyleSheet(qss.join(""));
}

void QUIHelper::setFormInCenter(QWidget *frm)
{
    int frmX = frm->width();
    int frmY = frm->height();
    QDesktopWidget w;
    int deskWidth = w.availableGeometry().width();
    int deskHeight = w.availableGeometry().height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    frm->move(movePoint);
}

void QUIHelper::setTranslator(const QString &qmFile)
{
    QTranslator *translator = new QTranslator(qApp);
    translator->load(qmFile);
    qApp->installTranslator(translator);
}

void QUIHelper::setCode()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#endif
}

void QUIHelper::setFont(const QString &ttfFile, const QString &fontName, int fontSize)
{
    QFont font;
    font.setFamily(fontName);
    font.setPixelSize(fontSize);

    //如果存在字体文件则设备字体文件中的字体
    //安卓版本和网页版本需要字体文件一起打包单独设置字体
    if (!ttfFile.isEmpty()) {
        QFontDatabase fontDb;
        int fontId = fontDb.addApplicationFont(ttfFile);
        if (fontId != -1) {
            QStringList androidFont = fontDb.applicationFontFamilies(fontId);
            if (androidFont.size() != 0) {
                font.setFamily(androidFont.at(0));
                font.setPixelSize(fontSize);
            }
        }
    }
    qApp->setFont(font);
}

void QUIHelper::sleep(int msec)
{
    if (msec > 0) {
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
        QTime endTime = QTime::currentTime().addMSecs(msec);
        while (QTime::currentTime() < endTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
#else
        QThread::msleep(msec);
#endif
    }
}

void QUIHelper::setSystemDateTime(const QString &year, const QString &month, const QString &day, const QString &hour, const QString &min, const QString &sec)
{
#ifdef Q_OS_WIN
    QProcess p(0);
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
    p.start("cmd");
    p.waitForStarted();
    p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
    p.closeWriteChannel();
    p.waitForFinished(1000);
    p.close();
#else
    QString cmd = QString("date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
    system(cmd.toLatin1());
    system("hwclock -w");
#endif
}

void QUIHelper::runWithSystem(const QString &strName, const QString &strPath, bool autoRun)
{
#ifdef Q_OS_WIN
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    reg.setValue(strName, autoRun ? strPath : "");
#endif
}

QString QUIHelper::getIP(const QString &url)
{
    //取出IP地址
    QRegExp regExp("((?:(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d)\\.){3}(?:25[0-5]|2[0-4]\\d|[01]?\\d?\\d))");
    regExp.indexIn(url);
    return url.mid(url.indexOf(regExp), regExp.matchedLength());
}

bool QUIHelper::isIP(const QString &ip)
{
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return regExp.exactMatch(ip);
}

bool QUIHelper::isMac(const QString &mac)
{
    QRegExp regExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
    return regExp.exactMatch(mac);
}

bool QUIHelper::isTel(const QString &tel)
{
    if (tel.length() != 11) {
        return false;
    }

    if (!tel.startsWith("13") && !tel.startsWith("14") && !tel.startsWith("15") && !tel.startsWith("18")) {
        return false;
    }

    return true;
}

bool QUIHelper::isEmail(const QString &email)
{
    if (!email.contains("@") || !email.contains(".com")) {
        return false;
    }

    return true;
}

QString QUIHelper::ipv4IntToString(quint32 ip)
{
    QString result = QString("%1.%2.%3.%4").arg((ip >> 24) & 0xFF).arg((ip >> 16) & 0xFF).arg((ip >> 8) & 0xFF).arg(ip & 0xFF);
    return result;
}

quint32 QUIHelper::ipv4StringToInt(const QString &ip)
{
    int result = 0;
    if (isIP(ip)) {
        QStringList list = ip.split(".");
        int ip0 = list.at(0).toInt();
        int ip1 = list.at(1).toInt();
        int ip2 = list.at(2).toInt();
        int ip3 = list.at(3).toInt();
        result = ip3 | ip2 << 8 | ip1 << 16 | ip0 << 24;
    }
    return result;
}

int QUIHelper::strHexToDecimal(const QString &strHex)
{
    bool ok;
    return strHex.toInt(&ok, 16);
}

int QUIHelper::strDecimalToDecimal(const QString &strDecimal)
{
    bool ok;
    return strDecimal.toInt(&ok, 10);
}

int QUIHelper::strBinToDecimal(const QString &strBin)
{
    bool ok;
    return strBin.toInt(&ok, 2);
}

QString QUIHelper::strHexToStrBin(const QString &strHex)
{
    uchar decimal = strHexToDecimal(strHex);
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len < 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrBin1(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 8) {
        for (int i = 0; i < 8 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrBin2(int decimal)
{
    QString bin = QString::number(decimal, 2);
    uchar len = bin.length();

    if (len <= 16) {
        for (int i = 0; i < 16 - len; i++) {
            bin = "0" + bin;
        }
    }

    return bin;
}

QString QUIHelper::decimalToStrHex(int decimal)
{
    QString temp = QString::number(decimal, 16);
    if (temp.length() == 1) {
        temp = "0" + temp;
    }

    return temp;
}

QByteArray QUIHelper::intToByte(int i)
{
    QByteArray result;
    result.resize(4);
    result[3] = (uchar)(0x000000ff & i);
    result[2] = (uchar)((0x0000ff00 & i) >> 8);
    result[1] = (uchar)((0x00ff0000 & i) >> 16);
    result[0] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

QByteArray QUIHelper::intToByteRec(int i)
{
    QByteArray result;
    result.resize(4);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    result[2] = (uchar)((0x00ff0000 & i) >> 16);
    result[3] = (uchar)((0xff000000 & i) >> 24);
    return result;
}

int QUIHelper::byteToInt(const QByteArray &data)
{
    int i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

int QUIHelper::byteToIntRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

quint32 QUIHelper::byteToUInt(const QByteArray &data)
{
    quint32 i = data.at(3) & 0x000000ff;
    i |= ((data.at(2) << 8) & 0x0000ff00);
    i |= ((data.at(1) << 16) & 0x00ff0000);
    i |= ((data.at(0) << 24) & 0xff000000);
    return i;
}

quint32 QUIHelper::byteToUIntRec(const QByteArray &data)
{
    quint32 i = data.at(0) & 0x000000ff;
    i |= ((data.at(1) << 8) & 0x0000ff00);
    i |= ((data.at(2) << 16) & 0x00ff0000);
    i |= ((data.at(3) << 24) & 0xff000000);
    return i;
}

QByteArray QUIHelper::ushortToByte(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[1] = (uchar)(0x000000ff & i);
    result[0] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

QByteArray QUIHelper::ushortToByteRec(ushort i)
{
    QByteArray result;
    result.resize(2);
    result[0] = (uchar)(0x000000ff & i);
    result[1] = (uchar)((0x0000ff00 & i) >> 8);
    return result;
}

int QUIHelper::byteToUShort(const QByteArray &data)
{
    int i = data.at(1) & 0x000000FF;
    i |= ((data.at(0) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

int QUIHelper::byteToUShortRec(const QByteArray &data)
{
    int i = data.at(0) & 0x000000FF;
    i |= ((data.at(1) << 8) & 0x0000FF00);

    if (i >= 32768) {
        i = i - 65536;
    }

    return i;
}

QString QUIHelper::getXorEncryptDecrypt(const QString &str, char key)
{
    QByteArray data = str.toLatin1();
    int size = data.size();
    for (int i = 0; i < size; i++) {
        data[i] = data[i] ^ key;
    }

    return QLatin1String(data);
}

uchar QUIHelper::getOrCode(const QByteArray &data)
{
    int len = data.length();
    uchar result = 0;

    for (int i = 0; i < len; i++) {
        result ^= data.at(i);
    }

    return result;
}

uchar QUIHelper::getCheckCode(const QByteArray &data)
{
    int len = data.length();
    uchar temp = 0;

    for (uchar i = 0; i < len; i++) {
        temp += data.at(i);
    }

    return temp % 256;
}

//函数功能：计算CRC16
//参数1：*data 16位CRC校验数据，
//参数2：len   数据流长度
//参数3：init  初始化值
//参数4：table 16位CRC查找表

//逆序CRC计算
quint16 QUIHelper::getRevCrc_16(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 >> 8;
        cRc_16 = (cRc_16 << 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//正序CRC计算
quint16 QUIHelper::getCrc_16(quint8 *data, int len, quint16 init, const quint16 *table)
{
    quint16 cRc_16 = init;
    quint8 temp;
    while (len-- > 0) {
        temp = cRc_16 & 0xff;
        cRc_16 = (cRc_16 >> 8) ^ table[(temp ^ *data++) & 0xff];
    }

    return cRc_16;
}

//Modbus CRC16校验
quint16 QUIHelper::getModbus16(quint8 *data, int len)
{
    //MODBUS CRC-16表 8005 逆序
    const quint16 table_16[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    return getCrc_16(data, len, 0xFFFF, table_16);
}

//CRC16校验
QByteArray QUIHelper::getCRCCode(const QByteArray &data)
{
    quint16 result = getModbus16((quint8 *)data.data(), data.length());
    return QUIHelper::ushortToByteRec(result);
}

static QMap<char, QString> listChar;
void QUIHelper::initAsciiStr()
{
    //0x20为空格,空格以下都是不可见字符
    if (listChar.count() == 0) {
        listChar.insert(0, "\\NUL");
        listChar.insert(1, "\\SOH");
        listChar.insert(2, "\\STX");
        listChar.insert(3, "\\ETX");
        listChar.insert(4, "\\EOT");
        listChar.insert(5, "\\ENQ");
        listChar.insert(6, "\\ACK");
        listChar.insert(7, "\\BEL");
        listChar.insert(8, "\\BS");
        listChar.insert(9, "\\HT");
        listChar.insert(10, "\\LF");
        listChar.insert(11, "\\VT");
        listChar.insert(12, "\\FF");
        listChar.insert(13, "\\CR");
        listChar.insert(14, "\\SO");
        listChar.insert(15, "\\SI");
        listChar.insert(16, "\\DLE");
        listChar.insert(17, "\\DC1");
        listChar.insert(18, "\\DC2");
        listChar.insert(19, "\\DC3");
        listChar.insert(20, "\\DC4");
        listChar.insert(21, "\\NAK");
        listChar.insert(22, "\\SYN");
        listChar.insert(23, "\\ETB");
        listChar.insert(24, "\\CAN");
        listChar.insert(25, "\\EM");
        listChar.insert(26, "\\SUB");
        listChar.insert(27, "\\ESC");
        listChar.insert(28, "\\FS");
        listChar.insert(29, "\\GS");
        listChar.insert(30, "\\RS");
        listChar.insert(31, "\\US");
        listChar.insert(0x5C, "\\");
        listChar.insert(0x7F, "\\DEL");
    }
}

QString QUIHelper::byteArrayToAsciiStr(const QByteArray &data)
{
    initAsciiStr();
    QString temp;
    int len = data.size();

    for (int i = 0; i < len; i++) {        
        char byte = data.at(i);
        QString value = listChar.value(byte);
        if (!value.isEmpty()) {
        } else if (byte >= 0 && byte <= 0x7F) {
            value = QString("%1").arg(byte);
        } else {
            value = decimalToStrHex((quint8)byte);
            value = QString("\\x%1").arg(value.toUpper());
        }

        temp += value;
    }

    return temp.trimmed();
}

QByteArray QUIHelper::asciiStrToByteArray(const QString &data)
{
    initAsciiStr();
    QByteArray buffer;
    QStringList list = data.split("\\");

    int count = list.count();
    for (int i = 1; i < count; i++) {
        QString str = list.at(i);
        int key = 0;
        if (str.contains("x")) {
            key = strHexToDecimal(str.mid(1, 2));
        } else {
            key = listChar.key("\\" + str);
        }

        buffer.append(key);
    }

    return buffer;
}

char QUIHelper::hexStrToChar(char data)
{
    if ((data >= '0') && (data <= '9')) {
        return data - 0x30;
    } else if ((data >= 'A') && (data <= 'F')) {
        return data - 'A' + 10;
    } else if ((data >= 'a') && (data <= 'f')) {
        return data - 'a' + 10;
    } else {
        return (-1);
    }
}

QByteArray QUIHelper::hexStrToByteArray(const QString &data)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = data.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        hstr = data.at(i).toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }

        i++;
        if (i >= len) {
            break;
        }

        lstr = data.at(i).toLatin1();
        hexdata = hexStrToChar(hstr);
        lowhexdata = hexStrToChar(lstr);

        if ((hexdata == 16) || (lowhexdata == 16)) {
            break;
        } else {
            hexdata = hexdata * 16 + lowhexdata;
        }

        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);
    return senddata;
}

QString QUIHelper::byteArrayToHexStr(const QByteArray &data)
{
    QString temp = "";
    QString hex = data.toHex();

    for (int i = 0; i < hex.length(); i = i + 2) {
        temp += hex.mid(i, 2) + " ";
    }

    return temp.trimmed().toUpper();
}

QString QUIHelper::getSaveName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getSaveFileName(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::getFileName(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileName(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::saveFileName(const QString &filter, const QString &defaultDir, const QString &fileName)
{
    QString file;
    QFileDialog dialog;
    dialog.setFixedSize(900, 600);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowTitle("保存文件");
    dialog.setLabelText(QFileDialog::Accept, "保存(&S)");
    dialog.setLabelText(QFileDialog::Reject, "取消(&C)");
    dialog.selectFile(fileName);
    dialog.setNameFilter(filter);
    dialog.setDirectory(defaultDir);

    if (dialog.exec() == 1) {
        file = dialog.selectedFiles().value(0);
        file = QFileInfo(file).suffix().isEmpty() ? "" : file;
    }

    return file;
}

QStringList QUIHelper::getFileNames(const QString &filter, QString defaultDir)
{
    return QFileDialog::getOpenFileNames(0, "选择文件", defaultDir, filter);
}

QString QUIHelper::getFolderName()
{
    return QFileDialog::getExistingDirectory();
}

QString QUIHelper::getFileNameWithExtension(const QString &strFilePath)
{
    QFileInfo fileInfo(strFilePath);
    return fileInfo.fileName();
}

QStringList QUIHelper::getFolderFileNames(const QStringList &filter)
{
    QStringList fileList;
    QString strFolder = QFileDialog::getExistingDirectory();

    if (!strFolder.length() == 0) {
        QDir myFolder(strFolder);

        if (myFolder.exists()) {
            fileList = myFolder.entryList(filter);
        }
    }

    return fileList;
}

bool QUIHelper::folderIsExist(const QString &strFolder)
{
    QDir tempFolder(strFolder);
    return tempFolder.exists();
}

bool QUIHelper::fileIsExist(const QString &strFile)
{
    QFile tempFile(strFile);
    return tempFile.exists();
}

bool QUIHelper::copyFile(const QString &sourceFile, const QString &targetFile)
{
    bool ok;
    ok = QFile::copy(sourceFile, targetFile);
    //将复制过去的文件只读属性取消
    ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
    return ok;
}

void QUIHelper::deleteDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();

    foreach (QFileInfo fi, fileList) {
        if (fi.isFile()) {
            fi.dir().remove(fi.fileName());
        } else {
            deleteDirectory(fi.absoluteFilePath());
            dir.rmdir(fi.absoluteFilePath());
        }
    }
}

bool QUIHelper::ipLive(const QString &ip, int port, int timeout)
{
    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(ip, port);
    eventLoop.exec();
    bool ok = (tcpSocket.state() == QAbstractSocket::ConnectedState);
    return ok;
}

QString QUIHelper::getHtml(const QString &url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    responseData = reply->readAll();
    return QString(responseData);
}

QString QUIHelper::getNetIP(const QString &webCode)
{
    QString web = webCode;
    web = web.replace(' ', "");
    web = web.replace("\r", "");
    web = web.replace("\n", "");
    QStringList list = web.split("<br/>");
    QString tar = list.at(3);
    QStringList ip = tar.split("=");
    return ip.at(1);
}

QString QUIHelper::getLocalIP()
{
    //优先取192开头的IP,如果获取不到IP则取127.0.0.1
    QString ip = "127.0.0.1";
    QStringList ips = getLocalIPs();
    foreach (QString str, ips) {
        if (str.startsWith("192.168.1") || str.startsWith("192")) {
            ip = str;
            break;
        }
    }

    return ip;
}

QStringList QUIHelper::getLocalIPs()
{
    static QStringList ips;
    if (ips.count() == 0) {
#ifdef emsdk
        ips << "127.0.0.1";
#else
        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface  &netInterface, netInterfaces) {
            //移除虚拟机和抓包工具的虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") || humanReadableName.startsWith("npcap loopback adapter")) {
                continue;
            }

            //过滤当前网络接口
            bool flag = (netInterface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast | QNetworkInterface::CanMulticast));
            if (flag) {
                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                foreach (QNetworkAddressEntry addr, addrs) {
                    //只取出IPV4的地址
                    if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                        QString ip4 = addr.ip().toString();
                        if (ip4 != "127.0.0.1") {
                            ips.append(ip4);
                        }
                    }
                }
            }
        }
#endif
    }

    return ips;
}

QString QUIHelper::urlToIP(const QString &url)
{
    QHostInfo host = QHostInfo::fromName(url);
    return host.addresses().at(0).toString();
}

QString QUIHelper::getValue(quint8 value)
{
    QString result = QString::number(value);
    if (result.length() <= 1) {
        result = QString("0%1").arg(result);
    }
    return result;
}

bool QUIHelper::isWebOk()
{
    //能接通百度IP说明可以通外网
    return ipLive("115.239.211.112", 80);
}

void QUIHelper::initTableView(QTableView *tableView, int rowHeight, bool headVisible, bool edit, bool stretchLast)
{
    //取消自动换行
    tableView->setWordWrap(false);
    //超出文本不显示省略号
    tableView->setTextElideMode(Qt::ElideNone);
    //奇数偶数行颜色交替
    tableView->setAlternatingRowColors(false);
    //垂直表头是否可见
    tableView->verticalHeader()->setVisible(headVisible);
    //选中一行表头是否加粗
    tableView->horizontalHeader()->setHighlightSections(false);
    //最后一行拉伸填充
    tableView->horizontalHeader()->setStretchLastSection(stretchLast);
    //行标题最小宽度尺寸
    tableView->horizontalHeader()->setMinimumSectionSize(0);
    //行标题最大高度
    tableView->horizontalHeader()->setMaximumHeight(rowHeight);
    //默认行高
    tableView->verticalHeader()->setDefaultSectionSize(rowHeight);
    //选中时一行整体选中
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只允许选择单个
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //表头不可单击
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    tableView->horizontalHeader()->setSectionsClickable(false);
#else
    tableView->horizontalHeader()->setClickable(false);
#endif

    //鼠标按下即进入编辑模式
    if (edit) {
        tableView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked);
    } else {
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

int QUIHelper::showMessageBox(const QString &info, int type, int closeSec, bool exec)
{
    int result = 0;
    if (type == 0) {
        showMessageBoxInfo(info, closeSec, exec);
    } else if (type == 1) {
        showMessageBoxError(info, closeSec, exec);
    } else if (type == 2) {
        result = showMessageBoxQuestion(info);
    }

    return result;
}

void QUIHelper::showMessageBoxInfo(const QString &info, int closeSec, bool exec)
{
    if (exec) {
        QUIMessageBox msg;
        msg.setMessage(info, 0, closeSec);
        msg.exec();
    } else {
        QUIMessageBox::Instance()->setMessage(info, 0, closeSec);
        QUIMessageBox::Instance()->show();
    }
}

void QUIHelper::showMessageBoxError(const QString &info, int closeSec, bool exec)
{
    if (exec) {
        QUIMessageBox msg;
        msg.setMessage(info, 2, closeSec);
        msg.exec();
    } else {
        QUIMessageBox::Instance()->setMessage(info, 2, closeSec);
        QUIMessageBox::Instance()->show();
    }
}

int QUIHelper::showMessageBoxQuestion(const QString &info)
{
    QUIMessageBox msg;
    msg.setMessage(info, 1);
    return msg.exec();
}

void QUIHelper::showTipBox(const QString &title, const QString &tip, bool fullScreen, bool center, int closeSec)
{
    QUITipBox::Instance()->setTip(title, tip, fullScreen, center, closeSec);
    QUITipBox::Instance()->show();
}

void QUIHelper::hideTipBox()
{
    QUITipBox::Instance()->hide();
}

QString QUIHelper::showInputBox(const QString &title, int type, int closeSec,
                                const QString &placeholderText, bool pwd,
                                const QString &defaultValue)
{
    QUIInputBox input;
    input.setParameter(title, type, closeSec, placeholderText, pwd, defaultValue);
    input.exec();
    return input.getValue();
}

void QUIHelper::showDateSelect(QString &dateStart, QString &dateEnd, const QString &format)
{
    QUIDateSelect select;
    select.setFormat(format);
    select.exec();
    dateStart = select.getStartDateTime();
    dateEnd = select.getEndDateTime();
}

QString QUIHelper::setPushButtonQss(QPushButton *btn, int radius, int padding,
                                    const QString &normalColor,
                                    const QString &normalTextColor,
                                    const QString &hoverColor,
                                    const QString &hoverTextColor,
                                    const QString &pressedColor,
                                    const QString &pressedTextColor)
{
    QStringList list;
    list.append(QString("QPushButton{border-style:none;padding:%1px;border-radius:%2px;color:%3;background:%4;}")
                .arg(padding).arg(radius).arg(normalTextColor).arg(normalColor));
    list.append(QString("QPushButton:hover{color:%1;background:%2;}")
                .arg(hoverTextColor).arg(hoverColor));
    list.append(QString("QPushButton:pressed{color:%1;background:%2;}")
                .arg(pressedTextColor).arg(pressedColor));

    QString qss = list.join("");
    btn->setStyleSheet(qss);
    return qss;
}

QString QUIHelper::setLineEditQss(QLineEdit *txt, int radius, int borderWidth,
                                  const QString &normalColor,
                                  const QString &focusColor)
{
    QStringList list;
    list.append(QString("QLineEdit{border-style:none;padding:3px;border-radius:%1px;border:%2px solid %3;}")
                .arg(radius).arg(borderWidth).arg(normalColor));
    list.append(QString("QLineEdit:focus{border:%1px solid %2;}")
                .arg(borderWidth).arg(focusColor));

    QString qss = list.join("");
    txt->setStyleSheet(qss);
    return qss;
}

QString QUIHelper::setProgressBarQss(QProgressBar *bar, int barHeight,
                                     int barRadius, int fontSize,
                                     const QString &normalColor,
                                     const QString &chunkColor)
{

    QStringList list;
    list.append(QString("QProgressBar{font:%1pt;background:%2;max-height:%3px;border-radius:%4px;text-align:center;border:1px solid %2;}")
                .arg(fontSize).arg(normalColor).arg(barHeight).arg(barRadius));
    list.append(QString("QProgressBar:chunk{border-radius:%2px;background-color:%1;}")
                .arg(chunkColor).arg(barRadius));

    QString qss = list.join("");
    bar->setStyleSheet(qss);
    return qss;
}

QString QUIHelper::setSliderQss(QSlider *slider, int sliderHeight,
                                const QString &normalColor,
                                const QString &grooveColor,
                                const QString &handleBorderColor,
                                const QString &handleColor,
                                const QString &textColor)
{
    int sliderRadius = sliderHeight / 2;
    int handleSize = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleSize / 2;
    int handleOffset = handleRadius / 2;

    QStringList list;
    int handleWidth = handleSize + sliderHeight / 5 - 1;
    list.append(QString("QSlider::horizontal{min-height:%1px;color:%2;}").arg(sliderHeight * 2).arg(textColor));
    list.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:horizontal{width:%3px;margin-top:-%4px;margin-bottom:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    //偏移一个像素
    handleWidth = handleSize + sliderHeight / 5;
    list.append(QString("QSlider::vertical{min-width:%1px;color:%2;}").arg(sliderHeight * 2).arg(textColor));
    list.append(QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(grooveColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:vertical{height:%3px;margin-left:-%4px;margin-right:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.6 %1,stop:0.8 %2);}")
                .arg(handleColor).arg(handleBorderColor).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    QString qss = list.join("");
    slider->setStyleSheet(qss);
    return qss;
}

QString QUIHelper::setRadioButtonQss(QRadioButton *rbtn, int indicatorRadius,
                                     const QString &normalColor,
                                     const QString &checkColor)
{
    int indicatorWidth = indicatorRadius * 2;

    QStringList list;
    list.append(QString("QRadioButton::indicator{border-radius:%1px;width:%2px;height:%2px;}")
                .arg(indicatorRadius).arg(indicatorWidth));
    list.append(QString("QRadioButton::indicator::unchecked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                        "stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(normalColor));
    list.append(QString("QRadioButton::indicator::checked{background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,"
                        "stop:0 %1,stop:0.3 %1,stop:0.4 #FFFFFF,stop:0.6 #FFFFFF,stop:0.7 %1);}").arg(checkColor));

    QString qss = list.join("");
    rbtn->setStyleSheet(qss);
    return qss;
}

QString QUIHelper::setScrollBarQss(QWidget *scroll, int radius, int min, int max,
                                   const QString &bgColor,
                                   const QString &handleNormalColor,
                                   const QString &handleHoverColor,
                                   const QString &handlePressedColor)
{
    //滚动条离背景间隔
    int padding = 0;

    QStringList list;

    //handle:指示器,滚动条拉动部分 add-page:滚动条拉动时增加的部分 sub-page:滚动条拉动时减少的部分 add-line:递增按钮 sub-line:递减按钮

    //横向滚动条部分
    list.append(QString("QScrollBar:horizontal{background:%1;padding:%2px;border-radius:%3px;min-height:%4px;max-height:%4px;}")
                .arg(bgColor).arg(padding).arg(radius).arg(max));
    list.append(QString("QScrollBar::handle:horizontal{background:%1;min-width:%2px;border-radius:%3px;}")
                .arg(handleNormalColor).arg(min).arg(radius));
    list.append(QString("QScrollBar::handle:horizontal:hover{background:%1;}")
                .arg(handleHoverColor));
    list.append(QString("QScrollBar::handle:horizontal:pressed{background:%1;}")
                .arg(handlePressedColor));
    list.append(QString("QScrollBar::add-page:horizontal{background:none;}"));
    list.append(QString("QScrollBar::sub-page:horizontal{background:none;}"));
    list.append(QString("QScrollBar::add-line:horizontal{background:none;}"));
    list.append(QString("QScrollBar::sub-line:horizontal{background:none;}"));

    //纵向滚动条部分
    list.append(QString("QScrollBar:vertical{background:%1;padding:%2px;border-radius:%3px;min-width:%4px;max-width:%4px;}")
                .arg(bgColor).arg(padding).arg(radius).arg(max));
    list.append(QString("QScrollBar::handle:vertical{background:%1;min-height:%2px;border-radius:%3px;}")
                .arg(handleNormalColor).arg(min).arg(radius));
    list.append(QString("QScrollBar::handle:vertical:hover{background:%1;}")
                .arg(handleHoverColor));
    list.append(QString("QScrollBar::handle:vertical:pressed{background:%1;}")
                .arg(handlePressedColor));
    list.append(QString("QScrollBar::add-page:vertical{background:none;}"));
    list.append(QString("QScrollBar::sub-page:vertical{background:none;}"));
    list.append(QString("QScrollBar::add-line:vertical{background:none;}"));
    list.append(QString("QScrollBar::sub-line:vertical{background:none;}"));

    QString qss = list.join("");
    scroll->setStyleSheet(qss);
    return qss;
}


QChar QUIConfig::IconMain = QChar(0xf072);
QChar QUIConfig::IconMenu = QChar(0xf0d7);
QChar QUIConfig::IconMin = QChar(0xf068);
QChar QUIConfig::IconMax = QChar(0xf2d2);
QChar QUIConfig::IconNormal = QChar(0xf2d0);
QChar QUIConfig::IconClose = QChar(0xf00d);

#ifdef __arm__
QString QUIConfig::FontName = "WenQuanYi Micro Hei";
int QUIConfig::FontSize = 18;
#else
QString QUIConfig::FontName = "Microsoft Yahei";
int QUIConfig::FontSize = 12;
#endif

QString QUIConfig::TextColor = "#F0F0F0";
QString QUIConfig::PanelColor = "#F0F0F0";
QString QUIConfig::BorderColor = "#F0F0F0";
QString QUIConfig::NormalColorStart = "#F0F0F0";
QString QUIConfig::NormalColorEnd = "#F0F0F0";
QString QUIConfig::DarkColorStart = "#F0F0F0";
QString QUIConfig::DarkColorEnd = "#F0F0F0";
QString QUIConfig::HighColor = "#F0F0F0";
