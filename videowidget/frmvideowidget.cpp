#pragma execution_character_set("utf-8")

#include "frmvideowidget.h"
#include "ui_frmvideowidget.h"

frmVideoWidget::frmVideoWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoWidget)
{
    ui->setupUi(this);
    this->initForm();
}

frmVideoWidget::~frmVideoWidget()
{
    delete ui;
}

void frmVideoWidget::initForm()
{
    ui->videoWidget1->setFlowEnable(true);
    ui->videoWidget2->setFlowEnable(true);
    ui->videoWidget3->setFlowEnable(true);
    ui->videoWidget4->setFlowEnable(true);

    connect(ui->videoWidget1, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWidget2, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWidget3, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
    connect(ui->videoWidget4, SIGNAL(btnClicked(QString)), this, SLOT(btnClicked(QString)));
}

void frmVideoWidget::btnClicked(const QString &objName)
{
    VideoWidget *videoWidget = (VideoWidget *)sender();
    QString str = QString("当前单击了控件 %1 的按钮 %2").arg(videoWidget->objectName()).arg(objName);
    ui->label->setText(str);
}
