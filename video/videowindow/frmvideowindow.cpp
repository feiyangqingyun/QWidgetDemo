#pragma execution_character_set("utf-8")

#include "frmvideowindow.h"
#include "ui_frmvideowindow.h"

frmVideoWindow::frmVideoWindow(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWindow)
{
    ui->setupUi(this);
    this->initForm();
}

frmVideoWindow::~frmVideoWindow()
{
    delete ui;
}

void frmVideoWindow::initForm()
{
    ui->videoWindow1->setFlowEnable(true);
    ui->videoWindow2->setFlowEnable(true);
    ui->videoWindow3->setFlowEnable(true);
    ui->videoWindow4->setFlowEnable(true);

    connect(ui->videoWindow1, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWindow2, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWindow3, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWindow4, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
}

void frmVideoWindow::btnClicked(const QString &objName)
{
    VideoWindow *videoWindow = (VideoWindow *)sender();
    QString str = QString("当前单击了控件 %1 的按钮 %2").arg(videoWindow->objectName()).arg(objName);
    ui->label->setText(str);
}
