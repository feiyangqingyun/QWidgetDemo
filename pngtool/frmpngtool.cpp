#pragma execution_character_set("utf-8")
#include "frmpngtool.h"
#include "ui_frmpngtool.h"
#include "qfile.h"
#include "qfiledialog.h"
#include "qdatetime.h"
#include "qdebug.h"

#define TIMEMS QTime::currentTime().toString("hh:mm:ss zzz")

frmPngTool::frmPngTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmPngTool)
{
    ui->setupUi(this);
    ui->progress->setRange(0, 0);
    ui->progress->setValue(0);
}

frmPngTool::~frmPngTool()
{
    delete ui;
}

void frmPngTool::on_btnFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "选择png文件", qApp->applicationDirPath(), "png图片文件(*.png)");
    if (!file.isEmpty()) {
        ui->txtFile->setText(file);
        ui->progress->setValue(0);
    }
}

void frmPngTool::on_btnDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择目录");
    if (!dir.isEmpty()) {
        ui->txtDir->setText(dir);
        ui->progress->setValue(0);
    }
}

void frmPngTool::on_btnOk_clicked()
{
    files.clear();

    //将单个文件加入队列
    QString currentFile = ui->txtFile->text().trimmed();
    if (!currentFile.isEmpty()) {
        files.append(currentFile);
    }

    //将该目录下的所有png文件存入链表
    QString currentDir = ui->txtDir->text().trimmed();
    if (!currentDir.isEmpty()) {
        QDir imagePath(currentDir);
        QStringList filter;
        filter << "*.png";
        QStringList list = imagePath.entryList(filter);
        foreach (QString str, list) {
            files.append(currentDir + "/" + str);
        }
    }

    ui->progress->setRange(0, files.count());
    ui->progress->setValue(0);

    ui->txtMain->clear();
    int count = 0;
    foreach (QString file, files) {
        ui->txtMain->append(QString("%1 -> %2").arg(TIMEMS).arg(file));
        QImage image(file);
        image.save(file, "png");
        count++;
        ui->progress->setValue(count);
        qApp->processEvents();
    }

    ui->txtMain->append(QString("%1 -> 处理完成, 共 %2 个文件").arg(TIMEMS).arg(files.count()));
}
