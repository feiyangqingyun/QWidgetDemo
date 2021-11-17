#pragma execution_character_set("utf-8")
#include "widget.h"
#include "ui_widget.h"
#include "qmessagebox.h"
#include "qdebug.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnOk_clicked()
{
    //当前多少钱
    int moneyCurrent = ui->txtMoneyCurrent->text().toInt();
    //利息
    float rate = ui->txtRate->text().toFloat();
    //定期期限
    int year = ui->cboxYear->currentText().left(1).toInt();
    //总年份 必须是定期期限的倍数
    int years = ui->txtYears->text().toInt();
    //最终多少钱
    int moneyAll = 0;

    if (years % year != 0) {
        ui->txtYears->setFocus();
        QMessageBox::critical(this, "错误", "总年份必须是期限的整数倍数!");
        return;
    }

    if (ui->cboxType->currentIndex() == 0) {
        //傻瓜场景 直接计算
        moneyAll = moneyCurrent + (moneyCurrent * rate * years);
    } else {
        //真实场景 复利计算
        int count = years / year;
        for (int i = 0; i < count; ++i) {
            moneyCurrent = moneyCurrent + (moneyCurrent * rate * year);
        }
        moneyAll = moneyCurrent;
    }

    //计算下来3年期定期存款30年总金额翻2番到最初本金3倍 100W本金3年期自动续期30年=321W
    ui->txtMoneyAll->setText(QString::number(moneyAll));
}
