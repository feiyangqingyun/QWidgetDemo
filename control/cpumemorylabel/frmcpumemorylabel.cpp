#pragma execution_character_set("utf-8")

#include "frmcpumemorylabel.h"
#include "ui_frmcpumemorylabel.h"

frmCpuMemoryLabel::frmCpuMemoryLabel(QWidget *parent) : QWidget(parent), ui(new Ui::frmCpuMemoryLabel)
{
    ui->setupUi(this);
    this->initForm();
}

frmCpuMemoryLabel::~frmCpuMemoryLabel()
{
    delete ui;
}

void frmCpuMemoryLabel::initForm()
{
    QFont font;
    font.setPixelSize(16);
    setFont(font);

    QString qss1 = QString("QLabel{background-color:rgb(0,0,0);color:rgb(%1);}").arg("100,184,255");
    QString qss2 = QString("QLabel{background-color:rgb(0,0,0);color:rgb(%1);}").arg("255,107,107");
    QString qss3 = QString("QLabel{background-color:rgb(0,0,0);color:rgb(%1);}").arg("24,189,155");

    ui->label1->setStyleSheet(qss1);
    ui->label2->setStyleSheet(qss2);
    ui->label3->setStyleSheet(qss3);

    connect(ui->label1, SIGNAL(textChanged(QString)), ui->label2, SLOT(setText(QString)));
    connect(ui->label1, SIGNAL(valueChanged(quint64, quint64, quint64, quint64, quint64)),
            this, SLOT(valueChanged(quint64, quint64, quint64, quint64, quint64)));
    ui->label1->start(1000);
}

void frmCpuMemoryLabel::valueChanged(quint64 cpuPercent, quint64 memoryPercent, quint64 memoryAll, quint64 memoryUse, quint64 memoryFree)
{
    //重新组织文字
    QString msg = QString("CPU: %1%  内存: %2% ( %3 MB / %4 MB )").arg(cpuPercent).arg(memoryPercent).arg(memoryUse).arg(memoryAll);
    ui->label3->setText(msg);
}
