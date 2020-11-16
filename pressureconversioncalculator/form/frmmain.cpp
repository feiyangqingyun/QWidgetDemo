#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"
#include "iconhelper.h"
#include <qdebug.h>

frmMain::frmMain(QWidget *parent) : QDialog(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
}

frmMain::~frmMain()
{
    delete ui;
}

bool frmMain::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (watched == ui->widgetTitle) {
            on_btnMenu_Max_clicked();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmMain::initForm()
{
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                         Qt::WindowMinMaxButtonsHint);
    IconHelper::Instance()->setIcon(ui->btnMenu_Min, QChar(0xf068));
    IconHelper::Instance()->setIcon(ui->btnMenu_Max, QChar(0xf067));
    IconHelper::Instance()->setIcon(ui->btnMenu_Close, QChar(0xf00d));
    //ui->widgetMenu->setVisible(false);
    ui->widgetTitle->installEventFilter(this);
    ui->widgetTitle->setProperty("form", "title");
    ui->widgetTop->setProperty("nav", "top");
    ui->labTitle->setText("压力单位转换工具");
    ui->labTitle->setFont(QFont("Microsoft Yahei", 20));
    this->setWindowTitle(ui->labTitle->text());
    ui->stackedWidget->setStyleSheet("QLabel{font:20pt;}QLineEdit{font:20pt;}QPushButton{font:20pt;}QSpinBox{font:20pt;}");
    //单独设置指示器大小
    int addWidth = 20;
    int addHeight = 10;
    int rbtnWidth = 15;
    int ckWidth = 13;
    int scrWidth = 12;
    int borderWidth = 3;
    QStringList qss;
    qss.append(
        QString("QComboBox::drop-down,QDateEdit::drop-down,QTimeEdit::drop-down,QDateTimeEdit::drop-down{width:%1px;}").arg(
            addWidth));
    qss.append(
        QString("QComboBox::down-arrow,QDateEdit[calendarPopup=\"true\"]::down-arrow,QTimeEdit[calendarPopup=\"true\"]::down-arrow,"
                "QDateTimeEdit[calendarPopup=\"true\"]::down-arrow{width:%1px;height:%1px;right:2px;}").arg(
            addHeight));
    qss.append(QString("QRadioButton::indicator{width:%1px;height:%1px;}").arg(rbtnWidth));
    qss.append(
        QString("QCheckBox::indicator,QGroupBox::indicator,QTreeWidget::indicator,QListWidget::indicator{width:%1px;height:%1px;}").arg(
            ckWidth));
    qss.append(
        QString("QScrollBar:horizontal{min-height:%1px;border-radius:%2px;}QScrollBar::handle:horizontal{border-radius:%2px;}"
                "QScrollBar:vertical{min-width:%1px;border-radius:%2px;}QScrollBar::handle:vertical{border-radius:%2px;}").arg(
            scrWidth).arg(scrWidth / 2));
    qss.append(QString("QWidget#widget_top>QToolButton:pressed,QWidget#widget_top>QToolButton:hover,"
                       "QWidget#widget_top>QToolButton:checked,QWidget#widget_top>QLabel:hover{"
                       "border-width:0px 0px %1px 0px;}").arg(borderWidth));
    qss.append(QString("QWidget#widgetleft>QPushButton:checked,QWidget#widgetleft>QToolButton:checked,"
                       "QWidget#widgetleft>QPushButton:pressed,QWidget#widgetleft>QToolButton:pressed{"
                       "border-width:0px 0px 0px %1px;}").arg(borderWidth));
    this->setStyleSheet(qss.join(""));
    QSize icoSize(32, 32);
    int icoWidth = 85;
    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();

    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }

    QList<QLineEdit *> ledits = ui->page1->findChildren<QLineEdit *>();

    foreach (QLineEdit *ledit, ledits) {
        connect(ledit, SIGNAL(textEdited(const QString)), this, SLOT(doTextEdited(const QString)));
    }

    ui->btnMain->click();
    on_spinBox_rbit_valueChanged(1);
}

void frmMain::buttonClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();

    foreach (QToolButton *btn, tbtns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }

    if (name == "主界面") {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (name == "用户退出") {
        exit(0);
    }
}

void frmMain::doTextEdited(const QString str)
{
    QLineEdit *l = (QLineEdit *)sender();
    QList<QLineEdit *> ledits = ui->page1->findChildren<QLineEdit *>();
    //转换前单位
    QString beforeName = l->objectName();
    //转换前数值
    double beforeValue = l->text().toDouble();

    foreach (QLineEdit *ledit, ledits) {
        //转换后单位
        QString curName = ledit->objectName();

        if (ledit == l) {
        } else if (curName == "lineEdit_bar") {
            //巴 (bar)
            if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.01, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.001, 'f', numOfDecimalPoints);
                qDebug() << outStr;
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 1.01325, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.0004788, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.03386388, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.00009807, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 10, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.00001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.00133322, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.06894757, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到巴 (bar)
                QString outStr = QString::number(beforeValue * 0.00009807, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_kpa") {
            //千帕 (kPa)
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 100.0, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 101.325, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.04788026, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 3.38638816, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.00980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 1000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.13332237, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 6.894757, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 98.0665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到千帕 (kPa)
                QString outStr = QString::number(beforeValue * 0.00980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_mbar") {
            //毫巴 (mbar)
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 1000.0, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 10, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 1013.25, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 0.47880257, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 33.86388158, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 0.0980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 10000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 0.01, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 1.33322368, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 68.94757, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 980.665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到毫巴(mbar)
                QString outStr = QString::number(beforeValue * 0.0980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_unit") {
            //标准大气压
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.98692327, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00986923, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00098692, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00047254, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.03342105, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00009678, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到标准大气压
                QString outStr = QString::number(beforeValue * 9.86923267, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00098692, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00000987, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00131579, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.06804596, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.96784111, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到标准大气压
                QString outStr = QString::number(beforeValue * 0.00009678, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_feet") {
            //磅力/英尺2
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2088.54351212, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 20.88543512, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2.08854351, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2116.21671366, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 70.72619017, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 0.20481615, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 20885.43512121, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2.08854351, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 0.02088544, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2.78449568, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 144, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 2048.16152331, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到磅力/英尺2
                QString outStr = QString::number(beforeValue * 0.20481615, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_british_hg") {
            //英吋汞柱
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 29.52998751, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.29529988, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.02952999, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 29.92125984, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.01413903, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.0028959, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 295.29987508, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.02952999, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.0002953, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.03937008, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 2.03602088, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 28.9590252, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 0.0028959, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_kilogram_force") {
            //公斤力/米2
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 10197.16212978, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 101.9716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 10.19716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 10332.274528, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 4.88242743, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 345.3154908, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 101971.62129779, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 10.19716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 0.10197162, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 13.59509806, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 703.06954974, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到公斤力/米2
                QString outStr = QString::number(beforeValue * 10000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到英吋汞柱
                QString outStr = QString::number(beforeValue * 1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_mpa") {
            //兆帕(MPa)
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.0001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.101325, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.00004788, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.00338639, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.00000981, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.0001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.000001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.00013332, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.00689476, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 0.0980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到兆帕(MPa)
                QString outStr = QString::number(beforeValue * 1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_hpa") {
            //百帕 (hPa)
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到百帕 (hPa)
                QString outStr = QString::number(beforeValue * 1000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 10, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 1013.25, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 0.47880257, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 33.86388158, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 0.0980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 10000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 0.01, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 1.33322368, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 68.94757, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 980.665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到百帕(hPa)
                QString outStr = QString::number(beforeValue * 0.0980665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_pascal") {
            //帕斯卡
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 100000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 1000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 100, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到帕斯卡
                QString outStr = QString::number(beforeValue * 101325, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到帕斯卡
                QString outStr = QString::number(beforeValue * 47.88025694, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到帕斯卡
                QString outStr = QString::number(beforeValue * 3386.38815789, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到帕斯卡
                QString outStr = QString::number(beforeValue * 9.80665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 1000000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 100, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到帕斯卡
                QString outStr = QString::number(beforeValue * 133.32236842, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到帕斯卡
                QString outStr = QString::number(beforeValue * 6894.757, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到帕斯卡
                QString outStr = QString::number(beforeValue * 98066.5, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到帕斯卡
                QString outStr = QString::number(beforeValue * 9.80665, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_mmhg") {
            //毫米汞柱 (托)
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 750.0616827, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 7.50061683, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 0.75006168, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 760, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 0.35913146, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 25.4, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 0.07355592, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 7500.61682704, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 0.75006168, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到毫米汞柱(托)
                QString outStr = QString::number(beforeValue * 0.00750062, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 51.71493037, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 735.55924007, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到毫米汞柱 (托)
                QString outStr = QString::number(beforeValue * 0.07355592, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_cmz") {
            //磅力/英寸2
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 14.50377439, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.14503774, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.01450377, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 14.6959494, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.00694444, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.4911541, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.00142233, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 145.0377439, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.01450377, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.00014504, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.01933678, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 14.22334391, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 0.00142233, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_kgf_cm2") {
            //公斤力/厘米2
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 1.01971621, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.01019716, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.00101972, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 1.03322745, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.00048824, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.03453155, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.0001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 10.19716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.00101972, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.0000102, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.00135951, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.07030695, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mm_water_column") {
                //毫米水柱装换到公斤力/厘米2
                QString outStr = QString::number(beforeValue * 0.0001, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        } else if (curName == "lineEdit_mm_water_column") {
            //毫米水柱
            if (beforeName == "lineEdit_bar") {
                //从巴 (bar)装换到磅力/英寸2
                QString outStr = QString::number(beforeValue * 10197.16212978, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kpa") {
                //从千帕 (kPa)装换到毫米水柱
                QString outStr = QString::number(beforeValue * 101.9716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mbar") {
                //从毫巴 (mbar)装换到毫米水柱
                QString outStr = QString::number(beforeValue * 10.19716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_unit") {
                //从标准大气压装换到毫米水柱
                QString outStr = QString::number(beforeValue * 10332.274528, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_feet") {
                //从磅力/英尺2装换到毫米水柱
                QString outStr = QString::number(beforeValue * 4.88242743, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_british_hg") {
                //英吋汞柱装换到毫米水柱
                QString outStr = QString::number(beforeValue * 345.3154908, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kilogram_force") {
                //公斤力/米2装换到毫米水柱
                QString outStr = QString::number(beforeValue * 1, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mpa") {
                //兆帕(MPa)装换到毫米水柱
                QString outStr = QString::number(beforeValue * 101971.62129779, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_hpa") {
                //百帕(hPa)装换到毫米水柱
                QString outStr = QString::number(beforeValue * 10.19716213, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_pascal") {
                //帕斯卡装换到毫米水柱
                QString outStr = QString::number(beforeValue * 0.10197162, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_mmhg") {
                //毫米汞柱 (托)装换到毫米水柱
                QString outStr = QString::number(beforeValue * 13.59509806, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_cmz") {
                //磅力/英寸2装换到毫米水柱
                QString outStr = QString::number(beforeValue * 703.06954974, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            } else if (beforeName == "lineEdit_kgf_cm2") {
                //公斤力/厘米2装换到毫米水柱
                QString outStr = QString::number(beforeValue * 10000, 'f', numOfDecimalPoints);
                ledit->setText(outStr);
            }
        }
    }
}

void frmMain::on_btnMenu_Min_clicked()
{
    showMinimized();
}
void frmMain::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
    }

    this->setProperty("canMove", max);
    max = !max;
}
void frmMain::on_btnMenu_Close_clicked()
{
    close();
}
void frmMain::on_spinBox_rbit_valueChanged(int arg1)
{
    numOfDecimalPoints = arg1;
    QList<QLineEdit *> ledits = ui->page1->findChildren<QLineEdit *>();

    foreach (QLineEdit *ledit, ledits) {
        ledit->setValidator(new QDoubleValidator(0, __DBL_MAX__, arg1, this));
    }
}
