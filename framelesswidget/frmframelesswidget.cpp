#pragma execution_character_set("utf-8")

#include "frmframelesswidget.h"
#include "ui_frmframelesswidget.h"
#include "qpushbutton.h"
#include "framelesswidget.h"

frmFramelessWidget::frmFramelessWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmFramelessWidget)
{
    ui->setupUi(this);
}

frmFramelessWidget::~frmFramelessWidget()
{
    delete ui;
}

void frmFramelessWidget::on_btnOpen_clicked()
{
    QWidget *w = new QWidget;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowTitle("自由拉伸无边框窗体");
    w->resize(480, 320);

    //设置下背景颜色区别看
    QPalette palette = w->palette();
    palette.setBrush(QPalette::Background, QColor(162, 121, 197));
    w->setPalette(palette);

    QPushButton *btn = new QPushButton(w);
    connect(btn, SIGNAL(clicked(bool)), w, SLOT(close()));
    btn->setGeometry(10, 10, 100, 25);
    btn->setText("关闭");

    FramelessWidget *f = new FramelessWidget(w);
    f->setWidget(w);
    w->show();
}
