#include "quidateselect.h"

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
}

QUIDateSelect::~QUIDateSelect()
{
    delete widgetMain;
}

void QUIDateSelect::showEvent(QShowEvent *)
{
    QUIHelper::setIconBtn(btnOk, ":/image/btn_ok.png", 0xf00c);
    QUIHelper::setIconBtn(btnClose, ":/image/btn_close.png", 0xf00d);
    QUIHelper::setFormInCenter(this);
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
    btnOk->setText("确 定");
    btnClose->setText("关 闭");

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
    this->setFixedSize(QUIDialogMinWidth + 50, QUIDialogMinHeight);

    QList<QPushButton *> btns  = this->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(QUIBtnMinWidth);
        btn->setIconSize(QSize(QUIIconWidth, QUIIconHeight));
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

void QUIDateSelect::setIconMain(const QChar &icon, quint32 size)
{
    IconHelper::Instance()->setIcon(this->labIco, icon, size);
}

void QUIDateSelect::setFormat(const QString &format)
{
    this->format = format;
    this->dateStart->setDisplayFormat(format);
    this->dateEnd->setDisplayFormat(format);
}
