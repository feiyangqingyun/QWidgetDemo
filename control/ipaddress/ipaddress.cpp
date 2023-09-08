#pragma execution_character_set("utf-8")

#include "ipaddress.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qboxlayout.h"
#include "qregexp.h"
#include "qvalidator.h"
#include "qevent.h"
#include "qdebug.h"

IPAddress::IPAddress(QWidget *parent) : QWidget(parent)
{
    bgColor = "#FFFFFF";
    borderColor = "#A6B5B8";
    borderRadius = 3;

    //用于显示小圆点的标签,居中对齐
    labDot1 = new QLabel;
    labDot1->setAlignment(Qt::AlignCenter);
    labDot1->setText(".");

    labDot2 = new QLabel;
    labDot2->setAlignment(Qt::AlignCenter);
    labDot2->setText(".");

    labDot3 = new QLabel;
    labDot3->setAlignment(Qt::AlignCenter);
    labDot3->setText(".");

    //用于输入IP地址的文本框,居中对齐
    txtIP1 = new QLineEdit;
    txtIP1->setObjectName("txtIP1");
    txtIP1->setAlignment(Qt::AlignCenter);
    txtIP1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(txtIP1, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    txtIP2 = new QLineEdit;
    txtIP2->setObjectName("txtIP2");
    txtIP2->setAlignment(Qt::AlignCenter);
    txtIP2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(txtIP2, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    txtIP3 = new QLineEdit;
    txtIP3->setObjectName("txtIP3");
    txtIP3->setAlignment(Qt::AlignCenter);
    txtIP3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(txtIP3, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    txtIP4 = new QLineEdit;
    txtIP4->setObjectName("txtIP4");
    txtIP4->setAlignment(Qt::AlignCenter);
    txtIP4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(txtIP4, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    //设置IP地址校验过滤
    QString pattern = "(2[0-5]{2}|2[0-4][0-9]|1?[0-9]{1,2})";
    //确切的说 QRegularExpression QRegularExpressionValidator 从5.0 5.1开始就有
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    QRegularExpression regExp(pattern);
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
#else
    QRegExp regExp(pattern);
    QRegExpValidator *validator = new QRegExpValidator(regExp, this);
#endif

    txtIP1->setValidator(validator);
    txtIP2->setValidator(validator);
    txtIP3->setValidator(validator);
    txtIP4->setValidator(validator);

    //绑定事件过滤器,识别键盘按下
    txtIP1->installEventFilter(this);
    txtIP2->installEventFilter(this);
    txtIP3->installEventFilter(this);
    txtIP4->installEventFilter(this);

    QFrame *frame = new QFrame;
    frame->setObjectName("frameIP");

    QStringList qss;
    qss.append(QString("QFrame#frameIP{border:1px solid %1;border-radius:%2px;}").arg(borderColor).arg(borderRadius));
    qss.append(QString("QLabel{min-width:15px;background-color:%1;}").arg(bgColor));
    qss.append(QString("QLineEdit{background-color:%1;border:none;}").arg(bgColor));
    qss.append(QString("QLineEdit#txtIP1{border-top-left-radius:%1px;border-bottom-left-radius:%1px;}").arg(borderRadius));
    qss.append(QString("QLineEdit#txtIP4{border-top-right-radius:%1px;border-bottom-right-radius:%1px;}").arg(borderRadius));
    frame->setStyleSheet(qss.join(""));

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);
    verticalLayout->addWidget(frame);

    //将控件按照横向布局排列
    QHBoxLayout *layout = new QHBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(txtIP1);
    layout->addWidget(labDot1);
    layout->addWidget(txtIP2);
    layout->addWidget(labDot2);
    layout->addWidget(txtIP3);
    layout->addWidget(labDot3);
    layout->addWidget(txtIP4);
}

bool IPAddress::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QLineEdit *txt = (QLineEdit *)watched;
        if (txt == txtIP1 || txt == txtIP2 || txt == txtIP3 || txt == txtIP4) {
            QKeyEvent *key = (QKeyEvent *)event;

            //如果当前按下了小数点则移动焦点到下一个输入框
            if (key->text() == ".") {
                this->focusNextChild();
            }

            //如果按下了退格键并且当前文本框已经没有了内容则焦点往前移
            if (key->key() == Qt::Key_Backspace) {
                if (txt->text().length() <= 1) {
                    this->focusNextPrevChild(false);
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void IPAddress::textChanged(const QString &text)
{
    int len = text.length();
    int value = text.toInt();

    //判断当前是否输入完成一个网段,是的话则自动移动到下一个输入框
    if (len == 3) {
        if (value >= 100 && value <= 255) {
            this->focusNextChild();
        }
    }

    //拼接成完整IP地址
    ip = QString("%1.%2.%3.%4").arg(txtIP1->text()).arg(txtIP2->text()).arg(txtIP3->text()).arg(txtIP4->text());
}

QSize IPAddress::sizeHint() const
{
    return QSize(250, 20);
}

QSize IPAddress::minimumSizeHint() const
{
    return QSize(30, 10);
}

QString IPAddress::getIP() const
{
    return this->ip;
}

void IPAddress::setIP(const QString &ip)
{
    //先检测IP地址是否合法
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if (!regExp.exactMatch(ip)) {
        return;
    }

    if (this->ip != ip) {
        this->ip = ip;

        //将IP地址填入各个网段
        QStringList list = ip.split(".");
        txtIP1->setText(list.at(0));
        txtIP2->setText(list.at(1));
        txtIP3->setText(list.at(2));
        txtIP4->setText(list.at(3));
    }
}

void IPAddress::clear()
{
    txtIP1->clear();
    txtIP2->clear();
    txtIP3->clear();
    txtIP4->clear();
    txtIP1->setFocus();
}

void IPAddress::setBgColor(const QString &bgColor)
{
    if (this->bgColor != bgColor) {
        this->bgColor = bgColor;
    }
}

void IPAddress::setBorderColor(const QString &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
    }
}

void IPAddress::setBorderRadius(int borderRadius)
{
    if (this->borderRadius != borderRadius) {
        this->borderRadius = borderRadius;
    }
}
