#include "frmgifwidget.h"
#include "ui_frmgifwidget.h"
#include "gifwidget.h"

frmGifWidget::frmGifWidget(QWidget *parent) : QWidget(parent), ui(new Ui::frmGifWidget)
{
    ui->setupUi(this);
}

frmGifWidget::~frmGifWidget()
{
    delete ui;
}

void frmGifWidget::on_pushButton_clicked()
{
    //设置截图窗口置顶显示
    GifWidget::Instance()->setWindowFlags(GifWidget::Instance()->windowFlags() | Qt::WindowStaysOnTopHint);
    GifWidget::Instance()->show();
}
