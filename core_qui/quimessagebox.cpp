#include "quimessagebox.h"

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
}

QUIMessageBox::~QUIMessageBox()
{
    delete widgetMain;
}

void QUIMessageBox::showEvent(QShowEvent *)
{
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnCancel, ":/image/btn_close.png", 0xf00d);
    QUIHelper::setFormInCenter(this);
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
    labInfo->setMinimumSize(QSize(0, QUITitleMinSize));
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

    btnOk->setText("确 定");
    btnCancel->setText("取 消");

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIMessageBox::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());
    this->setFixedSize(QUIDialogMinWidth, QUIDialogMinHeight);
    labIcoMain->setFixedSize(QUITitleMinSize, QUITitleMinSize);

    QList<QPushButton *> btns = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(QUIBtnMinWidth);
        btn->setIconSize(QSize(QUIIconWidth, QUIIconHeight));
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

void QUIMessageBox::setIconMain(const QChar &icon, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, icon, size);
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
        this->setFixedSize(QUIDialogMinWidth, QUIDialogMinHeight);
    } else {
        this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    }
}
