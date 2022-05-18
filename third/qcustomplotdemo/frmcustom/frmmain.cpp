#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "iconhelper.h"
#include "qdebug.h"

#include "frmquadratic.h"
#include "frmsimple.h"
#include "frmsincscatter.h"
#include "frmscatterstyle.h"
#include "frmscatterpixmap.h"
#include "frmlinestyle.h"
#include "frmdate.h"
#include "frmtexturebrush.h"
#include "frmmultiaxis.h"
#include "frmlogarithmic.h"
#include "frmrealtimedata.h"
#include "frmparametriccurve.h"
#include "frmbarchart.h"
#include "frmstatistical.h"
#include "frmsimpleitem.h"
#include "frmitem.h"
#include "frmstyled.h"
#include "frmadvancedaxes.h"
#include "frmcolormap.h"
#include "frmfinancial.h"
#include "frmpolarplot.h"

#include "frmaxistag.h"
#include "frminteraction.h"
#include "frmscrollbar.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initWidget();
    this->initNav();
    this->initIcon();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::showEvent(QShowEvent *)
{
    //滚动到底部
    QScrollBar *bar = ui->scrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
}

QList<QColor> frmMain::colors = QList<QColor>();
void frmMain::initForm()
{
    //颜色集合供其他界面使用
    colors << QColor(211, 78, 78) << QColor(29, 185, 242) << QColor(170, 162, 119) << QColor(255, 192, 1);
    colors << QColor(0, 176, 180) << QColor(0, 113, 193) << QColor(255, 192, 0);
    colors << QColor(72, 103, 149) << QColor(185, 87, 86) << QColor(0, 177, 125);
    colors << QColor(214, 77, 84) << QColor(71, 164, 233) << QColor(34, 163, 169);
    colors << QColor(59, 123, 156) << QColor(162, 121, 197) << QColor(72, 202, 245);
    colors << QColor(0, 150, 121) << QColor(111, 9, 176) << QColor(250, 170, 20);

    ui->scrollArea->setFixedWidth(170);
    ui->widgetLeft->setProperty("flag", "left");
}

void frmMain::initWidget()
{
    //ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->stackedWidget->addWidget(new frmQuadratic);
    ui->stackedWidget->addWidget(new frmSimple);
    ui->stackedWidget->addWidget(new frmSincScatter);
    ui->stackedWidget->addWidget(new frmScatterStyle);
    ui->stackedWidget->addWidget(new frmScatterPixmap);

    ui->stackedWidget->addWidget(new frmLineStyle);
    ui->stackedWidget->addWidget(new frmDate);
    ui->stackedWidget->addWidget(new frmTextureBrush);
    ui->stackedWidget->addWidget(new frmMultiAxis);
    ui->stackedWidget->addWidget(new frmLogarithmic);

    ui->stackedWidget->addWidget(new frmRealtimeData);
    ui->stackedWidget->addWidget(new frmParametricCurve);
    ui->stackedWidget->addWidget(new frmBarChart);
    ui->stackedWidget->addWidget(new frmStatistical);
    ui->stackedWidget->addWidget(new frmSimpleItem);

    ui->stackedWidget->addWidget(new frmItem);
    ui->stackedWidget->addWidget(new frmStyled);
    ui->stackedWidget->addWidget(new frmAdvancedAxes);
    ui->stackedWidget->addWidget(new frmColorMap);
    ui->stackedWidget->addWidget(new frmFinancial);
    ui->stackedWidget->addWidget(new frmPolarPlot);

    ui->stackedWidget->addWidget(new frmAxisTag);
    ui->stackedWidget->addWidget(new frmInterAction);
    ui->stackedWidget->addWidget(new frmScrollBar);
}

void frmMain::initNav()
{
    //按钮文字集合
    QStringList names;
    names << "平方二次元" << "简单曲线图" << "正弦散点图" << "数据点样式" << "数据点图片"
          << "曲线条样式" << "日期数据图" << "纹理画刷图" << "双坐标曲线" << "对数曲线图"
          << "动态正弦图" << "环形曲线图" << "垂直柱状图" << "箱形盒须图" << "静态指示线"
          << "动态指示线" << "曲线样式图" << "高级坐标轴" << "颜色热力图" << "金融曲线图"
          << "南丁格尔图" << "坐标轴指示" << "相互作用图" << "滚动条曲线";

    //自动生成按钮
    for (int i = 0; i < names.count(); i++) {
        QToolButton *btn = new QToolButton;
        //设置按钮固定高度
        btn->setFixedHeight(35);
        //设置按钮的文字
        btn->setText(QString("%1. %2").arg(i + 1, 2, 10, QChar('0')).arg(names.at(i)));
        //设置按钮可选中按下类似复选框的功能
        btn->setCheckable(true);
        //设置按钮图标在左侧文字在右侧
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        //设置按钮拉伸策略为横向填充
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        //关联按钮单击事件
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        //将按钮加入到布局
        ui->widgetLeft->layout()->addWidget(btn);
        //可以指定不同的图标
        icons << 0xf061;
        btns << btn;
    }

    //底部加个弹簧
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->widgetLeft->layout()->addItem(verticalSpacer);
    btns.at(names.count() - 1)->click();
}

void frmMain::initIcon()
{
    //左侧导航样式,可以设置各种牛逼的参数,超级棒
    IconHelper::StyleColor styleColor;
    styleColor.defaultBorder = true;
    IconHelper::setStyle(ui->widgetLeft, btns, icons, styleColor);
}

void frmMain::buttonClicked()
{
    QAbstractButton *b = (QAbstractButton *)sender();
    int count = btns.count();
    int index = btns.indexOf(b);
    ui->stackedWidget->setCurrentIndex(index);

    for (int i = 0; i < count; i++) {
        QAbstractButton *btn = btns.at(i);
        btn->setChecked(btn == b);
    }
}
