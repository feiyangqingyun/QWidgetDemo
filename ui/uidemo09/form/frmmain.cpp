#pragma execution_character_set("utf-8")

#include "frmmain.h"
#include "ui_frmmain.h"

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    this->setWindowTitle("九宫格主界面");

    bg = "main2.jpg";
    QList<QToolButton *> btns = this->findChildren<QToolButton *>();

    foreach (QToolButton *btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }
}

void frmMain::buttonClick()
{
    QToolButton *btn = (QToolButton *)sender();
    QString objName = btn->objectName();

    if (objName == "btnCOMTool") {
        if (bg == "main1.jpg") {
            bg = "main2.jpg";
        } else if (bg == "main2.jpg") {
            bg = "main3.jpg";
        } else if (bg == "main3.jpg") {
            bg = "main4.jpg";
        } else if (bg == "main4.jpg") {
            bg = "main5.jpg";
        } else if (bg == "main5.jpg") {
            bg = "main1.jpg";
        }

        QString qss = QString("QWidget#frm{background-image: url(:/image/%1);}").arg(bg);
        qss += "QToolButton{color:#E7ECF0;background-color:rgba(0,0,0,0);border-style:none;}";
        this->setStyleSheet(qss);
    } else if (objName == "btnAddressTool") {

    } else if (objName == "btnTCPTool") {
        this->close();
    } else if (objName == "btnCOMTCPTool") {

    } else if (objName == "btnDefence") {

    } else if (objName == "btnHostTool") {

    } else if (objName == "btnLinkTool") {

    } else if (objName == "btnMaiChongTool") {

    } else if (objName == "btnPlot") {

    } else if (objName == "btnZhangLi4Tool") {

    } else if (objName == "btnZhangLi5Tool") {

    } else if (objName == "btnZhangLiTool") {

    }
}
