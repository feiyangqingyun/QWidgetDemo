#pragma execution_character_set("utf-8")

#include "frmiconhelper.h"
#include "ui_frmiconhelper.h"
#include "iconhelper.h"

frmIconHelper::frmIconHelper(QWidget *parent) : QWidget(parent), ui(new Ui::frmIconHelper)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget1();
    this->initWidget2();
    this->initWidget3();
    this->initWidget4();
    this->initWidget5();
    this->initWidget6();
    QTimer::singleShot(100, this, SLOT(initPanel()));
}

frmIconHelper::~frmIconHelper()
{
    delete ui;
}

bool frmIconHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        QLabel *lab = (QLabel *)watched;
        if (lab != 0) {
            //由于有图形字体的范围值冲突需要手动切换索引
            if (ui->rbtnFontAwesome6->isChecked()) {
                IconHelper::setIconFontIndex(2);
            } else if (ui->rbtnFontWeather->isChecked()) {
                IconHelper::setIconFontIndex(3);
            } else {
                IconHelper::setIconFontIndex(-1);
            }

            //对应图形字体的16进制值已经赋值给了 toolTip
            QString value = lab->toolTip();
            ui->labValue->setText(value);
            int icon = value.toInt(NULL, 16);

            IconHelper::setIcon(ui->labIcon, icon, iconSize);
            IconHelper::setIcon(ui->btnIcon, icon, iconSize);

            //万能大法直接从指定标识获取图片文件
            QPixmap pix = IconHelper::getPixmap("#753775", icon, iconSize, iconSize, iconSize);
            ui->labImage->setPixmap(pix);

            //设置图标  以下方法二选一都可以
            //ui->btnImage->setIcon(QIcon(pix));
            IconHelper::setPixmap(ui->btnImage, "#FD8B28", icon, iconSize, iconSize, iconSize);

            //取出对应图形字体类
            QFont font = IconHelper::getIconFontAwesome();
            if (ui->rbtnFontAliBaBa->isChecked()) {
                font = IconHelper::getIconFontAliBaBa();
            } else if (ui->rbtnFontAwesome6->isChecked()) {
                font = IconHelper::getIconFontAwesome6();
            } else if (ui->rbtnFontWeather->isChecked()) {
                font = IconHelper::getIconFontWeather();
            }

            //直接设置图标+文本到按钮
            font.setPixelSize(15);
            ui->btnTest1->setFont(font);
            ui->btnTest1->setText(QString("%1 测试图标").arg((QChar)icon));

            //分别设置图标+文本到按钮
            ui->btnTest2->setIcon(QIcon(pix));
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmIconHelper::initForm()
{
    iconSize = 40;
    //图标对应图形字体值
    icons << 0xf2ba << 0xf002 << 0xf013 << 0xf021;

    //设置样式表
    QStringList qss;
    qss << QString("#labIcon{color:#32B9CF;}");
    qss << QString("#btnIcon{color:#C13256;}");
    qss << QString("#labValue,#labCount,#labInfo1,#labInfo2,#labInfo3,#labInfo4{font-weight:bold;font-size:20px;}");
    qss << QString("QWidget#widget1 QAbstractButton{min-height:%1px;max-height:%1px;}").arg(35);
    qss << QString("QWidget#widget2 QAbstractButton{min-height:%1px;max-height:%1px;}").arg(35);
    qss << QString("QWidget#widget3 QAbstractButton{min-height:%1px;max-height:%1px;}").arg(70);
    qss << QString("QWidget#widget4 QAbstractButton{min-height:%1px;max-height:%1px;}").arg(70);
    qss << QString("QWidget#widget5 QAbstractButton{min-width:%1px;max-width:%1px;}").arg(90);
    qss << QString("QWidget#widget6 QAbstractButton{min-width:%1px;max-width:%1px;}").arg(90);
    this->setStyleSheet(qss.join(""));

    //设置图形字体
    IconHelper::setIcon(ui->labIcon, 0xf067, iconSize);
    IconHelper::setIcon(ui->btnIcon, 0xf067, iconSize);
    QPixmap pix = IconHelper::getPixmap("#753775", 0xf067, iconSize, iconSize, iconSize);
    ui->labImage->setPixmap(pix);
    ui->btnImage->setIconSize(QSize(iconSize, iconSize));
    ui->btnImage->setIcon(QIcon(pix));

    //关联单选框切换
    QList<QRadioButton *> rbtns = ui->widgetRight->findChildren<QRadioButton *>();
    foreach (QRadioButton *rbtn, rbtns) {
        connect(rbtn, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
    }

    ui->tabWidget->setCurrentIndex(0);
}

void frmIconHelper::initPanel()
{
    //清空原有对象
    qDeleteAll(labs);
    labs.clear();

    //选择不同的图形字体
    int start = 0xf000;
    int end = 0xf2e0;
    QFont iconFont = IconHelper::getIconFontAwesome();
    IconHelper::setIconFontIndex(-1);

    if (ui->rbtnFontAliBaBa->isChecked()) {
        start = 0xe500;
        end = 0xec00;
        iconFont = IconHelper::getIconFontAliBaBa();
    } else if (ui->rbtnFontAwesome6->isChecked()) {
        start = 0xe000;
        end = 0xf8ff;
        iconFont = IconHelper::getIconFontAwesome6();
        IconHelper::setIconFontIndex(2);
    } else if (ui->rbtnFontWeather->isChecked()) {
        start = 0xe900;
        end = 0xe9cf;
        iconFont = IconHelper::getIconFontWeather();
        IconHelper::setIconFontIndex(3);
    }

    //设置字体大小
    iconFont.setPixelSize(15);

    //加载图形字体面板
    QStringList list;
    for (int icon = start; icon <= end; icon++) {
        //中间有一段是空的,可以自行屏蔽下面这段代码查看这段空的值对应的文字
        if (ui->rbtnFontAliBaBa->isChecked()) {
            if (icon >= 0xe76c && icon <= 0xe8f8) {
                continue;
            }
        } else if (ui->rbtnFontAwesome6->isChecked()) {
            if (icon >= 0xe33d && icon <= 0xefff) {
                continue;
            }
        }

        QString tip = "0x" + QString::number(icon, 16);
        if (!checkIcon(icon)) {
            list << tip;
            continue;
        }

        QLabel *lab = new QLabel;
        lab->installEventFilter(this);
        lab->setAlignment(Qt::AlignCenter);
        lab->setFont(iconFont);
        lab->setText((QChar)icon);
        lab->setToolTip(tip);
        lab->setMinimumSize(30, 30);
        labs << lab;
    }

    //qDebug() << "no text font" << list.count() << list;
    ui->widgetFontPanel->setAutoWidth(true);
    ui->widgetFontPanel->setMargin(3);
    ui->widgetFontPanel->setSpace(3);
    ui->widgetFontPanel->setColumnCount(18);
    ui->widgetFontPanel->setWidgets(labs);

    //设置图形字体面板鼠标悬停时的效果
    QString qss = QString("QLabel:hover,QLabel:focus{color:%1;border:1px solid %1;}").arg("#00BB9E");
    ui->widgetFontPanel->setStyleSheet(qss);

    int count = end - start + 1;
    ui->labCount->setText(QString("%1/%2").arg(labs.count()).arg(count));
}

bool frmIconHelper::checkIcon(int icon)
{
    //从图形字体对应值生成一个指定颜色的图片
    QPixmap pix = IconHelper::getPixmap("#FF0000", icon, 120, 120, 120);
    QImage img = pix.toImage();
    int width = img.width();
    int height = img.height();

    //过滤不存在的图形字体
    //对该图片逐个扫描像素点,都是空白则意味着当前图形字体不存在
    for (int i = 0; i < height; ++i) {
        uchar *lineByte = img.scanLine(i);
        for (int j = 0; j < width; j++) {
            uchar tp = lineByte[j];
            if (tp > 0x00) {
                return true;
            }
        }
    }

    //保存下图片看下
    //QString fileName = QString("%1/icon/%2.jpg").arg(qApp->applicationDirPath()).arg(icon);
    //pix.save(fileName, "jpg");
    return false;
}

void frmIconHelper::toggled(bool checked)
{
    //单选框按下后自动重新加载对应的图形字体
    if (checked) {
        initPanel();
    }
}

void frmIconHelper::initBtn(QButtonGroup *btnGroup, bool textBesideIcon)
{
    QList<QAbstractButton *> btns = btnGroup->buttons();
    foreach (QAbstractButton *btn, btns) {
        QToolButton *b = (QToolButton *)btn;
        //关联按钮单击事件
        connect(b, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
        b->setCheckable(true);
        b->setToolButtonStyle(textBesideIcon ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonTextUnderIcon);
    }
}

void frmIconHelper::btnClicked()
{
    QAbstractButton *btn = (QAbstractButton *)sender();
    QString objName = btn->parent()->objectName();
    if (objName == "widget1") {
        ui->labInfo1->setText(btn->text());
    } else if (objName == "widget2") {
        ui->labInfo2->setText(btn->text());
    } else if (objName == "widget3") {
        ui->labInfo3->setText(btn->text());
    } else if (objName == "widget4") {
        ui->labInfo4->setText(btn->text());
    }
}

void frmIconHelper::initWidget1()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn11);
    btnGroup->addButton(ui->btn12);
    btnGroup->addButton(ui->btn13);
    btnGroup->addButton(ui->btn14);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, true);
    //设置弱属性以便应用样式
    ui->widget1->setProperty("flag", "left");

    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    styleColor.position = "left";
    styleColor.iconSize = 18;
    styleColor.iconWidth = 30;
    styleColor.iconHeight = 25;
    styleColor.borderWidth = 4;
    IconHelper::setStyle(ui->widget1, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn11->click();
}

void frmIconHelper::initWidget2()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn21);
    btnGroup->addButton(ui->btn22);
    btnGroup->addButton(ui->btn23);
    btnGroup->addButton(ui->btn24);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, true);
    //设置弱属性以便应用样式
    ui->widget2->setProperty("flag", "right");

    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    styleColor.position = "right";
    styleColor.iconSize = 18;
    styleColor.iconWidth = 25;
    styleColor.iconHeight = 20;
    styleColor.borderWidth = 4;
    styleColor.borderColor = "#32B9CF";
    styleColor.setColor("#187294", "#B6D7E3", "#145C75", "#F0F0F0");
    IconHelper::setStyle(ui->widget2, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn22->click();
}

void frmIconHelper::initWidget3()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn31);
    btnGroup->addButton(ui->btn32);
    btnGroup->addButton(ui->btn33);
    btnGroup->addButton(ui->btn34);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, false);
    //设置弱属性以便应用样式
    ui->widget3->setProperty("flag", "left");

    IconHelper::StyleColor styleColor;
    styleColor.position = "left";
    styleColor.iconSize = 30;
    styleColor.iconWidth = 40;
    styleColor.iconHeight = 40;
    styleColor.borderWidth = 3;
    styleColor.borderColor = "#609EE9";
    IconHelper::setStyle(ui->widget3, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn33->click();
}

void frmIconHelper::initWidget4()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn41);
    btnGroup->addButton(ui->btn42);
    btnGroup->addButton(ui->btn43);
    btnGroup->addButton(ui->btn44);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, false);
    //设置弱属性以便应用样式
    ui->widget4->setProperty("flag", "right");

    IconHelper::StyleColor styleColor;
    styleColor.position = "right";
    styleColor.iconSize = 30;
    styleColor.iconWidth = 40;
    styleColor.iconHeight = 40;
    styleColor.borderWidth = 3;
    styleColor.borderColor = "#F7AE13";
    styleColor.setColor("#FCDC97", "#54626F", "#FFF0BC", "#54626F");
    IconHelper::setStyle(ui->widget4, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn44->click();
}

void frmIconHelper::initWidget5()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn51);
    btnGroup->addButton(ui->btn52);
    btnGroup->addButton(ui->btn53);
    btnGroup->addButton(ui->btn54);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, false);
    //设置弱属性以便应用样式
    ui->widget5->setProperty("flag", "top");

    //设置整体按钮组样式
    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    styleColor.position = "top";
    styleColor.iconSize = 25;
    styleColor.iconWidth = 25;
    styleColor.iconHeight = 25;
    styleColor.borderWidth = 3;
    IconHelper::setStyle(ui->widget5, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn51->click();
}

void frmIconHelper::initWidget6()
{
    //加入按钮组自动互斥
    QButtonGroup *btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btn61);
    btnGroup->addButton(ui->btn62);
    btnGroup->addButton(ui->btn63);
    btnGroup->addButton(ui->btn64);

    //设置按钮可选中以及图标样式
    initBtn(btnGroup, false);
    //设置弱属性以便应用样式
    ui->widget6->setProperty("flag", "bottom");

    //设置整体按钮组样式
    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    styleColor.position = "bottom";
    styleColor.iconSize = 25;
    styleColor.iconWidth = 25;
    styleColor.iconHeight = 25;
    styleColor.borderWidth = 3;
    styleColor.borderColor = "#A279C5";
    styleColor.setColor("#292929", "#B6D7E3", "#10689A", "#F0F0F0");
    IconHelper::setStyle(ui->widget6, btnGroup->buttons(), icons, styleColor);

    //默认选中某个按钮
    ui->btn63->click();
}
