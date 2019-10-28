#include "widget.h"
#include "ui_widget.h"
#include "qlineedit.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->lineEdit1, SIGNAL(returnPressed()), this, SLOT(next()));
    connect(ui->lineEdit2, SIGNAL(returnPressed()), this, SLOT(next()));
    connect(ui->lineEdit3, SIGNAL(returnPressed()), this, SLOT(next()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::next()
{
    QLineEdit *lineEdit = (QLineEdit *)sender();
    if (lineEdit == ui->lineEdit1) {
        ui->lineEdit2->setFocus();
    } else if (lineEdit == ui->lineEdit2) {
        ui->lineEdit3->setFocus();
    } else if (lineEdit == ui->lineEdit3) {
        ui->lineEdit1->setFocus();
    }
}
