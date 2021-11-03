#pragma execution_character_set("utf-8")

#include "frmnavbutton.h"
#include "ui_frmnavbutton.h"
#include "navbutton.h"
#include "iconhelper.h"
#include "qdebug.h"

frmNavButton::frmNavButton(QWidget *parent) : QWidget(parent), ui(new Ui::frmNavButton)
{
    ui->setupUi(this);
    this->initForm();
    this->initBtn1();
    this->initBtn2();
    this->initBtn3();
    this->initBtn4();
    this->initBtn5();
    this->initBtn6();
    this->initBtn7();
}

frmNavButton::~frmNavButton()
{
    delete ui;
}

void frmNavButton::initForm()
{
    icons << 0xf17b << 0xf002 << 0xf013 << 0xf021 << 0xf0e0 << 0xf135;

    ui->navButton11->setChecked(true);
    ui->navButton23->setChecked(true);
    ui->navButton31->setChecked(true);
    ui->navButton44->setChecked(true);
    ui->navButton53->setChecked(true);
    ui->navButton61->setChecked(true);
    ui->navButton75->setChecked(true);

    //设置整体圆角
    ui->widgetNav5->setStyleSheet(".QWidget{background:#292929;border:1px solid #292929;border-radius:20px;}");
}

void frmNavButton::initBtn1()
{
    quint32 size = 15;
    quint32 pixWidth = 15;
    quint32 pixHeight = 15;

    //从图形字体获得图片,也可以从资源文件或者路径文件获取
    int icon = 0xf061;
    QPixmap iconNormal = IconHelper::getPixmap(QColor(100, 100, 100).name(), icon, size, pixWidth, pixHeight);
    QPixmap iconHover = IconHelper::getPixmap(QColor(255, 255, 255).name(), icon, size, pixWidth, pixHeight);
    QPixmap iconCheck = IconHelper::getPixmap(QColor(255, 255, 255).name(), icon, size, pixWidth, pixHeight);

    btns1 << ui->navButton11 << ui->navButton12 << ui->navButton13 << ui->navButton14;
    for (int i = 0; i < btns1.count(); i++) {
        NavButton *btn = btns1.at(i);
        btn->setPaddingLeft(32);
        btn->setLineSpace(6);

        btn->setShowIcon(true);
        btn->setIconSpace(15);
        btn->setIconSize(QSize(10, 10));
        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick1()));
    }
}

void frmNavButton::initBtn2()
{
    quint32 size = 15;
    quint32 pixWidth = 20;
    quint32 pixHeight = 20;

    QColor normalBgColor = QColor("#2D9191");
    QColor hoverBgColor = QColor("#187294");
    QColor checkBgColor = QColor("#145C75");
    QColor normalTextColor = QColor("#FFFFFF");
    QColor hoverTextColor = QColor("#FFFFFF");
    QColor checkTextColor = QColor("#FFFFFF");

    btns2 << ui->navButton21 << ui->navButton22 << ui->navButton23 << ui->navButton24;
    for (int i = 0; i < btns2.count(); i++) {
        NavButton *btn = btns2.at(i);
        btn->setPaddingLeft(35);
        btn->setLineSpace(0);
        btn->setLineWidth(8);
        btn->setLineColor(QColor(255, 107, 107));
        btn->setShowTriangle(true);

        btn->setShowIcon(true);
        btn->setIconSpace(10);
        btn->setIconSize(QSize(22, 22));

        //分开设置图标
        int icon = icons.at(i);
        QPixmap iconNormal = IconHelper::getPixmap(normalTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconHover = IconHelper::getPixmap(hoverTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconCheck = IconHelper::getPixmap(checkTextColor.name(), icon, size, pixWidth, pixHeight);

        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        btn->setNormalBgColor(normalBgColor);
        btn->setHoverBgColor(hoverBgColor);
        btn->setCheckBgColor(checkBgColor);
        btn->setNormalTextColor(normalTextColor);
        btn->setHoverTextColor(hoverTextColor);
        btn->setCheckTextColor(checkTextColor);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick2()));
    }
}

void frmNavButton::initBtn3()
{
    quint32 size = 15;
    quint32 pixWidth = 20;
    quint32 pixHeight = 20;

    QColor normalBgColor = QColor("#292F38");
    QColor hoverBgColor = QColor("#1D2025");
    QColor checkBgColor = QColor("#1D2025");
    QColor normalTextColor = QColor("#54626F");
    QColor hoverTextColor = QColor("#FDFDFD");
    QColor checkTextColor = QColor("#FDFDFD");

    btns3 << ui->navButton31 << ui->navButton32 << ui->navButton33 << ui->navButton34;
    for (int i = 0; i < btns3.count(); i++) {
        NavButton *btn = btns3.at(i);
        btn->setPaddingLeft(35);
        btn->setLineWidth(10);
        btn->setLineColor(QColor("#029FEA"));
        btn->setShowTriangle(true);
        btn->setTextAlign(NavButton::TextAlign_Left);
        btn->setTrianglePosition(NavButton::TrianglePosition_Left);
        btn->setLinePosition(NavButton::LinePosition_Right);

        btn->setShowIcon(true);
        btn->setIconSpace(10);
        btn->setIconSize(QSize(22, 22));

        //分开设置图标
        int icon = icons.at(i);
        QPixmap iconNormal = IconHelper::getPixmap(normalTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconHover = IconHelper::getPixmap(hoverTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconCheck = IconHelper::getPixmap(checkTextColor.name(), icon, size, pixWidth, pixHeight);

        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        btn->setNormalBgColor(normalBgColor);
        btn->setHoverBgColor(hoverBgColor);
        btn->setCheckBgColor(checkBgColor);
        btn->setNormalTextColor(normalTextColor);
        btn->setHoverTextColor(hoverTextColor);
        btn->setCheckTextColor(checkTextColor);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick3()));
    }
}

void frmNavButton::initBtn4()
{
    quint32 size = 15;
    quint32 pixWidth = 15;
    quint32 pixHeight = 15;

    int icon = 0xf105;
    QPixmap iconNormal = IconHelper::getPixmap(QColor(100, 100, 100).name(), icon, size, pixWidth, pixHeight);
    QPixmap iconHover = IconHelper::getPixmap(QColor(255, 255, 255).name(), icon, size, pixWidth, pixHeight);
    QPixmap iconCheck = IconHelper::getPixmap(QColor(255, 255, 255).name(), icon, size, pixWidth, pixHeight);

    btns4 << ui->navButton41 << ui->navButton42 << ui->navButton43 << ui->navButton44;
    for (int i = 0; i < btns4.count(); i++) {
        NavButton *btn = btns4.at(i);
        btn->setLineSpace(10);
        btn->setLineWidth(10);
        btn->setPaddingRight(35);
        btn->setShowTriangle(true);
        btn->setTextAlign(NavButton::TextAlign_Right);
        btn->setTrianglePosition(NavButton::TrianglePosition_Left);
        btn->setLinePosition(NavButton::LinePosition_Right);

        btn->setShowIcon(true);
        btn->setIconSpace(20);
        btn->setIconSize(QSize(15, 15));
        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick4()));
    }
}

void frmNavButton::initBtn5()
{
    QFont font;
    font.setPixelSize(15);
    font.setBold(true);

    quint32 size = 15;
    quint32 pixWidth = 20;
    quint32 pixHeight = 20;

    QColor normalBgColor = QColor("#292929");
    QColor hoverBgColor = QColor("#064077");
    QColor checkBgColor = QColor("#10689A");
    QColor normalTextColor = QColor("#FFFFFF");
    QColor hoverTextColor = Qt::yellow;
    QColor checkTextColor = QColor("#FFFFFF");

    btns5 << ui->navButton51 << ui->navButton52 << ui->navButton53 << ui->navButton54 << ui->navButton55;
    for (int i = 0; i < btns5.count(); i++) {
        NavButton *btn = btns5.at(i);
        btn->setFont(font);
        btn->setPaddingLeft(20);
        btn->setShowLine(false);
        btn->setTextAlign(NavButton::TextAlign_Center);
        btn->setLinePosition(NavButton::LinePosition_Bottom);

        btn->setShowIcon(true);
        btn->setIconSpace(15);
        btn->setIconSize(QSize(22, 22));

        //分开设置图标
        int icon = icons.at(i);
        QPixmap iconNormal = IconHelper::getPixmap(normalTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconHover = IconHelper::getPixmap(hoverTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconCheck = IconHelper::getPixmap(checkTextColor.name(), icon, size, pixWidth, pixHeight);

        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        btn->setNormalBgColor(normalBgColor);
        btn->setHoverBgColor(hoverBgColor);
        btn->setCheckBgColor(checkBgColor);
        btn->setNormalTextColor(normalTextColor);
        btn->setHoverTextColor(hoverTextColor);
        btn->setCheckTextColor(checkTextColor);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick5()));
    }
}

void frmNavButton::initBtn6()
{
    QFont font;
    font.setPixelSize(15);
    font.setBold(true);

    quint32 size = 15;
    quint32 pixWidth = 20;
    quint32 pixHeight = 20;

    QColor normalBgColor = QColor("#E6393D");
    QColor hoverBgColor = QColor("#EE0000");
    QColor checkBgColor = QColor("#A40001");
    QColor normalTextColor = QColor("#FFFFFF");
    QColor hoverTextColor = QColor("#FFFFFF");
    QColor checkTextColor = QColor("#FFFFFF");

    btns6 << ui->navButton61 << ui->navButton62 << ui->navButton63 << ui->navButton64 << ui->navButton65;
    for (int i = 0; i < btns6.count(); i++) {
        NavButton *btn = btns6.at(i);
        btn->setFont(font);
        btn->setPaddingLeft(20);
        btn->setShowLine(false);
        btn->setTextAlign(NavButton::TextAlign_Center);
        btn->setLinePosition(NavButton::LinePosition_Bottom);

        btn->setShowIcon(true);
        btn->setIconSpace(15);
        btn->setIconSize(QSize(22, 22));

        //分开设置图标
        int icon = icons.at(i);
        QPixmap iconNormal = IconHelper::getPixmap(normalTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconHover = IconHelper::getPixmap(hoverTextColor.name(), icon, size, pixWidth, pixHeight);
        QPixmap iconCheck = IconHelper::getPixmap(checkTextColor.name(), icon, size, pixWidth, pixHeight);

        btn->setIconNormal(iconNormal);
        btn->setIconHover(iconHover);
        btn->setIconCheck(iconCheck);

        btn->setNormalBgColor(normalBgColor);
        btn->setHoverBgColor(hoverBgColor);
        btn->setCheckBgColor(checkBgColor);
        btn->setNormalTextColor(normalTextColor);
        btn->setHoverTextColor(hoverTextColor);
        btn->setCheckTextColor(checkTextColor);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick6()));
    }
}

void frmNavButton::initBtn7()
{
    QFont font;
    font.setPixelSize(15);
    font.setBold(true);

    QColor normalTextColor = QColor("#FFFFFF");
    QColor hoverTextColor = QColor("#FFFFFF");
    QColor checkTextColor = QColor("#FFFFFF");

    //设置背景色为画刷
    QLinearGradient normalBgBrush(0, 0, 0, ui->navButton71->height());
    normalBgBrush.setColorAt(0.0, QColor("#3985BF"));
    normalBgBrush.setColorAt(0.5, QColor("#2972A9"));
    normalBgBrush.setColorAt(1.0, QColor("#1C6496"));

    QLinearGradient hoverBgBrush(0, 0, 0, ui->navButton71->height());
    hoverBgBrush.setColorAt(0.0, QColor("#4897D1"));
    hoverBgBrush.setColorAt(0.5, QColor("#3283BC"));
    hoverBgBrush.setColorAt(1.0, QColor("#3088C3"));

    btns7 << ui->navButton71 << ui->navButton72 << ui->navButton73 << ui->navButton74 << ui->navButton75 << ui->navButton76;
    for (int i = 0; i < btns7.count(); i++) {
        NavButton *btn = btns7.at(i);
        btn->setFont(font);
        btn->setPaddingLeft(0);
        btn->setLineSpace(0);
        btn->setShowTriangle(true);
        btn->setTextAlign(NavButton::TextAlign_Center);
        btn->setTrianglePosition(NavButton::TrianglePosition_Bottom);
        btn->setLinePosition(NavButton::LinePosition_Top);

        btn->setNormalTextColor(normalTextColor);
        btn->setHoverTextColor(hoverTextColor);
        btn->setCheckTextColor(checkTextColor);

        btn->setNormalBgBrush(normalBgBrush);
        btn->setHoverBgBrush(hoverBgBrush);
        btn->setCheckBgBrush(hoverBgBrush);

        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClick7()));
    }
}

void frmNavButton::buttonClick1()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns1.count(); i++) {
        NavButton *btn = btns1.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick2()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns2.count(); i++) {
        NavButton *btn = btns2.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick3()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns3.count(); i++) {
        NavButton *btn = btns3.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick4()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns4.count(); i++) {
        NavButton *btn = btns4.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick5()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns5.count(); i++) {
        NavButton *btn = btns5.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick6()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns6.count(); i++) {
        NavButton *btn = btns6.at(i);
        btn->setChecked(b == btn);
    }
}

void frmNavButton::buttonClick7()
{
    NavButton *b = (NavButton *)sender();
    qDebug() << "当前按下" << b->text();
    for (int i = 0; i < btns7.count(); i++) {
        NavButton *btn = btns7.at(i);
        btn->setChecked(b == btn);
    }
}
