#include "quiwidget.h"

QUIWidget::QUIWidget(QWidget *parent) : QDialog(parent)
{
    this->initControl();
    this->initForm();    
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

QPushButton *QUIWidget::getBtnMenuClose() const
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
    setIcon(QUIWidget::Lab_Ico, QUIConfig::IconMain, QUIConfig::FontSize + 1);
    setIcon(QUIWidget::BtnMenu, QUIConfig::IconMenu);
    setIcon(QUIWidget::BtnMenu_Min, QUIConfig::IconMin);
    setIcon(QUIWidget::BtnMenu_Normal, QUIConfig::IconNormal);
    setIcon(QUIWidget::BtnMenu_Close, QUIConfig::IconClose);

    this->widgetTitle->setProperty("form", "title");
    QUIHelper::setFramelessForm(this);

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
    QStringList styleNames, styleFiles;
    QUIStyle::getStyle(styleNames, styleFiles);
    int count = styleNames.count();
    for (int i = 0; i < count; i++) {
        QAction *action = new QAction(this);
        action->setText(styleNames.at(i));
        action->setData(styleFiles.at(i));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle()));
        this->btnMenu->addAction(action);
    }
}

void QUIWidget::changeStyle()
{
    QAction *action = (QAction *)sender();
    QString qssFile = action->data().toString();
    QUIStyle::setStyle(qssFile);
    emit changeStyle(qssFile);
}

void QUIWidget::setIcon(QUIWidget::Widget widget, int icon, quint32 size)
{
    if (widget == QUIWidget::Lab_Ico) {
        setIconMain(icon, size);
    } else if (widget == QUIWidget::BtnMenu) {
        QUIConfig::IconMenu = icon;
        IconHelper::setIcon(this->btnMenu, icon, size);
    } else if (widget == QUIWidget::BtnMenu_Min) {
        QUIConfig::IconMin = icon;
        IconHelper::setIcon(this->btnMenu_Min, icon, size);
    } else if (widget == QUIWidget::BtnMenu_Max) {
        QUIConfig::IconMax = icon;
        IconHelper::setIcon(this->btnMenu_Max, icon, size);
    } else if (widget == QUIWidget::BtnMenu_Normal) {
        QUIConfig::IconNormal = icon;
        IconHelper::setIcon(this->btnMenu_Max, icon, size);
    } else if (widget == QUIWidget::BtnMenu_Close) {
        QUIConfig::IconClose = icon;
        IconHelper::setIcon(this->btnMenu_Close, icon, size);
    }
}

void QUIWidget::setIconMain(int icon, quint32 size)
{
    QUIConfig::IconMain = icon;
    IconHelper::setIcon(this->labIco, icon, size);
    QUIMessageBox::Instance()->setIconMain(icon, size);
    QUIInputBox::Instance()->setIconMain(icon, size);
    QUIDateSelect::Instance()->setIconMain(icon, size);
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
        QUIHelper::setFormInCenter(this);
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
        this->setGeometry(QUIHelper::getScreenRect());
        setIcon(QUIWidget::BtnMenu_Max, QUIConfig::IconMax);
    }

    this->setProperty("canMove", max);
    max = !max;
}

void QUIWidget::on_btnMenu_Close_clicked()
{
    //先发送关闭信号
    emit closing();
    if (exitAll) {
        mainWidget->close();
        this->close();
    }
}
