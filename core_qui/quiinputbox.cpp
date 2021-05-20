#include "quiinputbox.h"

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
}

QUIInputBox::~QUIInputBox()
{
    delete widgetMain;
}

void QUIInputBox::showEvent(QShowEvent *)
{
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnCancel, ":/image/btn_close.png", 0xf00d);
    QUIHelper::setFormInCenter(this);
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
    btnOk->setText("确 定");
    btnCancel->setText("取 消");

    connect(btnOk, SIGNAL(clicked()), this, SLOT(on_btnOk_clicked()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
    connect(btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnMenu_Close_clicked()));
}

void QUIInputBox::initForm()
{
    QUIHelper::setFramelessForm(this, widgetTitle, labIco, btnMenu_Close);
    this->setWindowTitle(this->labTitle->text());
    this->setFixedSize(QUIDialogMinWidth, QUIDialogMinHeight + 10);

    QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
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

void QUIInputBox::setIconMain(const QChar &icon, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, icon, size);
}
