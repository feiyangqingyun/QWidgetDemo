#pragma execution_character_set("utf-8")

#include "FrmAStackWidget.h"

#include <QButtonGroup>
#include <QLabel>

FrmAStackWidget::FrmAStackWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QList<QString> colorlst;
    colorlst << "#1abc9c";
    colorlst << "#2ecc71";
    colorlst << "#3498db";
    colorlst << "#9b59b6";
    colorlst << "#e74c3c";

    QList<QPushButton *> btnlst;
    btnlst << ui.pushButton_1;
    btnlst << ui.pushButton_2;
    btnlst << ui.pushButton_3;
    btnlst << ui.pushButton_4;
    btnlst << ui.pushButton_5;

    QButtonGroup *btnGroup = new QButtonGroup(this);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(btnGroup, SIGNAL(idClicked(int)), ui.aStackwidget, SLOT(setCurrentIndex(int)));
#else
    connect(btnGroup, SIGNAL(buttonClicked(int)), ui.aStackwidget, SLOT(setCurrentIndex(int)));
#endif

    for (int i = 0; i < 5; i++) {
        QLabel *label = new QLabel(ui.aStackwidget);
        label->setStyleSheet(QString("background-color:%1;color:#ffffff;").arg(colorlst.at(i)));
        label->setText(QString::number(i + 1));
        label->setAlignment(Qt::AlignCenter);
        int index = ui.aStackwidget->addWidget(label);
        btnGroup->addButton(btnlst.at(i), index);
    }
}
