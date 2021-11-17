#pragma execution_character_set("utf-8")

#include "frmmaskwidget.h"
#include "ui_frmmaskwidget.h"
#include "maskwidget.h"
#include "qdialog.h"
#include "qtimer.h"
#include "qdebug.h"

frmMaskWidget::frmMaskWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmMaskWidget)
{
    ui->setupUi(this);
    QTimer::singleShot(1000, this, SLOT(initForm()));
}

frmMaskWidget::~frmMaskWidget()
{
    delete ui;
}

void frmMaskWidget::initForm()
{
    MaskWidget::Instance()->setMainWidget(this->topLevelWidget());
    MaskWidget::Instance()->setDialogNames(QStringList() << "frmTest");
}

void frmMaskWidget::on_pushButton_clicked()
{
    QDialog d;
    d.setObjectName("frmTest");
    d.setWindowTitle("遮罩层弹出窗体");
    d.resize(400, 300);
    d.exec();
}
