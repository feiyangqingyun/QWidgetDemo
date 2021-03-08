#pragma execution_character_set("utf-8")

#include "frmframelesswidget.h"
#include "ui_frmframelesswidget.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "framelesswidget.h"

frmFramelessWidget::frmFramelessWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmFramelessWidget)
{
    ui->setupUi(this);
    w = 0;
}

frmFramelessWidget::~frmFramelessWidget()
{
    delete ui;
}

void frmFramelessWidget::closeEvent(QCloseEvent *)
{
    exit(0);
}

void frmFramelessWidget::on_pushButton_clicked()
{
    if (w == 0) {
        w = new QWidget;
        //设置无边框属性
        w->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
        //w->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
        w->setWindowTitle("自由拉伸无边框窗体");
        w->setMinimumSize(200, 120);
        w->resize(480, 320);

        //设置下背景颜色区别看
        QPalette palette = w->palette();
        palette.setBrush(QPalette::Background, QColor(162, 121, 197));
        w->setPalette(palette);

        QPushButton *btn = new QPushButton(w);
        btn->setText("关闭");
        btn->setGeometry(10, 10, 130, 25);
        connect(btn, SIGNAL(clicked(bool)), w, SLOT(close()));

        QCheckBox *cboxMove = new QCheckBox(w);
        cboxMove->setText("可移动");
        cboxMove->setChecked(true);
        cboxMove->setGeometry(10, 40, 70, 25);
        connect(cboxMove, SIGNAL(stateChanged(int)), this, SLOT(stateChanged1(int)));

        QCheckBox *cboxResize = new QCheckBox(w);
        cboxResize->setText("可拉伸");
        cboxResize->setChecked(true);
        cboxResize->setGeometry(80, 40, 70, 25);
        connect(cboxResize, SIGNAL(stateChanged(int)), this, SLOT(stateChanged2(int)));

        frameless = new FramelessWidget(w);
        frameless->setWidget(w);
    }

    w->show();
}

void frmFramelessWidget::stateChanged1(int arg1)
{
    frameless->setMoveEnable(arg1 != 0);
}

void frmFramelessWidget::stateChanged2(int arg1)
{
    frameless->setResizeEnable(arg1 != 0);
}
