#pragma execution_character_set("utf-8")

#include "frmmovewidget.h"
#include "ui_frmmovewidget.h"
#include "qpushbutton.h"
#include "qprogressbar.h"
#include "movewidget.h"

frmMoveWidget::frmMoveWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmMoveWidget)
{
    ui->setupUi(this);
    this->initForm();
}

frmMoveWidget::~frmMoveWidget()
{
    delete ui;
}

void frmMoveWidget::initForm()
{
    QPushButton *btn1 = new QPushButton(this);
    btn1->setGeometry(10, 10, 250, 25);
    btn1->setText("按住我拖动(仅限左键拖动)");
    MoveWidget *moveWidget1 = new MoveWidget(this);
    moveWidget1->setWidget(btn1);

    QPushButton *btn2 = new QPushButton(this);
    btn2->setGeometry(10, 40, 250, 25);
    btn2->setText("按住我拖动(支持右键拖动)");
    MoveWidget *moveWidget2 = new MoveWidget(this);
    moveWidget2->setWidget(btn2);
    moveWidget2->setLeftButton(false);

    QProgressBar *bar = new QProgressBar(this);
    bar->setGeometry(10, 70, 250, 25);
    bar->setRange(0, 0);
    bar->setTextVisible(false);
    MoveWidget *moveWidget3 = new MoveWidget(this);
    moveWidget3->setWidget(bar);
}