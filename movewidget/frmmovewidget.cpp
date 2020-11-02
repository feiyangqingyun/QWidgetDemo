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
    moveWidget1->setLeftButton(true);
    moveWidget1->setRightButton(false);

    QPushButton *btn2 = new QPushButton(this);
    btn2->setGeometry(10, 40, 250, 25);
    btn2->setText("按住我拖动(仅限右键拖动)");
    MoveWidget *moveWidget2 = new MoveWidget(this);
    moveWidget2->setWidget(btn2);
    moveWidget2->setLeftButton(false);
    moveWidget2->setRightButton(true);

    QPushButton *btn3 = new QPushButton(this);
    btn3->setGeometry(10, 70, 250, 25);
    btn3->setText("按住我拖动(支持左右键拖动)");
    MoveWidget *moveWidget3 = new MoveWidget(this);
    moveWidget3->setWidget(btn3);
    moveWidget3->setLeftButton(true);
    moveWidget3->setRightButton(true);

    QProgressBar *bar = new QProgressBar(this);
    bar->setGeometry(10, 100, 250, 25);
    bar->setRange(0, 0);
    bar->setTextVisible(false);
    MoveWidget *moveWidget4 = new MoveWidget(this);
    moveWidget4->setWidget(bar);
}
