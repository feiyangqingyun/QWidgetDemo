#include "quitipbox.h"

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
    this->setFixedSize(QUIDialogMinWidth, QUIDialogMinHeight);

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

void QUITipBox::setIconMain(int icon, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, icon, size);
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

    QRect rect = QUIHelper::getScreenRect(!fullScreen);
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width() + rect.x();
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
    QRect rect = QUIHelper::getScreenRect(!fullScreen);
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width() + rect.x();
    int y = height - this->height();

    //启动动画
    animation->stop();
    animation->setStartValue(QPoint(x, y));
    animation->setEndValue(QPoint(x, QUIHelper::getScreenRect(false).height()));
    animation->start();
}

