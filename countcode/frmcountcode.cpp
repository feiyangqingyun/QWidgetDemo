#pragma execution_character_set("utf-8")

#include "frmcountcode.h"
#include "ui_frmcountcode.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qfiledialog.h"
#include "qfileinfo.h"
#include "qdebug.h"

frmCountCode::frmCountCode(QWidget *parent) : QWidget(parent), ui(new Ui::frmCountCode)
{
    ui->setupUi(this);
    this->initForm();
    on_btnClear_clicked();
}

frmCountCode::~frmCountCode()
{
    delete ui;
}

void frmCountCode::initForm()
{
    QStringList headText;
    headText << "文件名" << "类型" << "大小" << "总行数" << "代码行数" << "注释行数" << "空白行数" << "路径";
    QList<int> columnWidth;
    columnWidth << 130 << 50 << 70 << 80 << 70 << 70 << 70 << 150;

    int columnCount = headText.count();
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(headText);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(20);
    ui->tableWidget->verticalHeader()->setHighlightSections(false);

    for (int i = 0; i < columnCount; i++) {
        ui->tableWidget->setColumnWidth(i, columnWidth.at(i));
    }

    //设置前景色
    ui->txtCount->setStyleSheet("color:#17A086;");
    ui->txtSize->setStyleSheet("color:#CA5AA6;");
    ui->txtRow->setStyleSheet("color:#CD1B19;");
    ui->txtCode->setStyleSheet("color:#22A3A9;");
    ui->txtNote->setStyleSheet("color:#D64D54;");
    ui->txtBlank->setStyleSheet("color:#A279C5;");

    //设置字体加粗
    QFont font;
    font.setBold(true);
    if (font.pointSize() > 0) {
        font.setPointSize(font.pointSize() + 1);
    } else {
        font.setPixelSize(font.pixelSize() + 2);
    }

    ui->txtCount->setFont(font);
    ui->txtSize->setFont(font);
    ui->txtRow->setFont(font);
    ui->txtCode->setFont(font);
    ui->txtNote->setFont(font);
    ui->txtBlank->setFont(font);

#if (QT_VERSION > QT_VERSION_CHECK(4,7,0))
    ui->txtFilter->setPlaceholderText("中间空格隔开,例如 *.h *.cpp *.c");
#endif
}

bool frmCountCode::checkFile(const QString &fileName)
{
    if (fileName.startsWith("moc_") || fileName.startsWith("ui_") || fileName.startsWith("qrc_")) {
        return false;
    }

    QFileInfo file(fileName);
    QString suffix = "*." + file.suffix();
    QString filter = ui->txtFilter->text().trimmed();
    QStringList filters = filter.split(" ");
    return filters.contains(suffix);
}

void frmCountCode::countCode(const QString &filePath)
{
    QDir dir(filePath);
    QFileInfoList fileInfos = dir.entryInfoList();
    foreach (QFileInfo fileInfo, fileInfos) {
        QString fileName = fileInfo.fileName();
        if (fileInfo.isFile()) {
            if (checkFile(fileName)) {
                listFile << fileInfo.filePath();
            }
        } else {
            if (fileName == "." || fileName == "..") {
                continue;
            }

            //递归找出文件
            countCode(fileInfo.absoluteFilePath());
        }
    }
}

void frmCountCode::countCode(const QStringList &files)
{
    int lineCode;
    int lineBlank;
    int lineNotes;
    int count = files.count();
    on_btnClear_clicked();
    ui->tableWidget->setRowCount(count);

    quint32 totalLines = 0;
    quint32 totalBytes = 0;
    quint32 totalCodes = 0;
    quint32 totalNotes = 0;
    quint32 totalBlanks = 0;

    for (int i = 0; i < count; i++) {
        QFileInfo fileInfo(files.at(i));
        countCode(fileInfo.filePath(), lineCode, lineBlank, lineNotes);
        int lineAll = lineCode + lineBlank + lineNotes;

        QTableWidgetItem *itemName = new QTableWidgetItem;
        itemName->setText(fileInfo.fileName());

        QTableWidgetItem *itemSuffix = new QTableWidgetItem;
        itemSuffix->setText(fileInfo.suffix());

        QTableWidgetItem *itemSize = new QTableWidgetItem;
        itemSize->setText(QString::number(fileInfo.size()));

        QTableWidgetItem *itemLine = new QTableWidgetItem;
        itemLine->setText(QString::number(lineAll));

        QTableWidgetItem *itemCode = new QTableWidgetItem;
        itemCode->setText(QString::number(lineCode));

        QTableWidgetItem *itemNote = new QTableWidgetItem;
        itemNote->setText(QString::number(lineNotes));

        QTableWidgetItem *itemBlank = new QTableWidgetItem;
        itemBlank->setText(QString::number(lineBlank));

        QTableWidgetItem *itemPath = new QTableWidgetItem;
        itemPath->setText(fileInfo.filePath());

        itemSuffix->setTextAlignment(Qt::AlignCenter);
        itemSize->setTextAlignment(Qt::AlignCenter);
        itemLine->setTextAlignment(Qt::AlignCenter);
        itemCode->setTextAlignment(Qt::AlignCenter);
        itemNote->setTextAlignment(Qt::AlignCenter);
        itemBlank->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(i, 0, itemName);
        ui->tableWidget->setItem(i, 1, itemSuffix);
        ui->tableWidget->setItem(i, 2, itemSize);
        ui->tableWidget->setItem(i, 3, itemLine);
        ui->tableWidget->setItem(i, 4, itemCode);
        ui->tableWidget->setItem(i, 5, itemNote);
        ui->tableWidget->setItem(i, 6, itemBlank);
        ui->tableWidget->setItem(i, 7, itemPath);

        totalBytes  += fileInfo.size();
        totalLines  += lineAll;
        totalCodes  += lineCode;
        totalNotes  += lineNotes;
        totalBlanks += lineBlank;

        if (i % 100 == 0) {
            qApp->processEvents();
        }
    }

    //显示统计结果
    listFile.clear();
    ui->txtCount->setText(QString::number(count));
    ui->txtSize->setText(QString::number(totalBytes));
    ui->txtRow->setText(QString::number(totalLines));
    ui->txtCode->setText(QString::number(totalCodes));
    ui->txtNote->setText(QString::number(totalNotes));
    ui->txtBlank->setText(QString::number(totalBlanks));

    //计算百分比
    double percent = 0.0;
    //代码行所占百分比
    percent = ((double)totalCodes / totalLines) * 100;
    ui->labPercentCode->setText(QString("%1%").arg(percent, 5, 'f', 2, QChar(' ')));
    //注释行所占百分比
    percent = ((double)totalNotes / totalLines) * 100;
    ui->labPercentNote->setText(QString("%1%").arg(percent, 5, 'f', 2, QChar(' ')));
    //空行所占百分比
    percent = ((double)totalBlanks / totalLines) * 100;
    ui->labPercentBlank->setText(QString("%1%").arg(percent, 5, 'f', 2, QChar(' ')));
}

void frmCountCode::countCode(const QString &fileName, int &lineCode, int &lineBlank, int &lineNotes)
{
    lineCode = lineBlank = lineNotes = 0;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream out(&file);
        QString line;
        bool isNote = false;
        while (!out.atEnd()) {
            line = out.readLine();

            //移除前面的空行
            if (line.startsWith(" ")) {
                line.remove(" ");
            }

            //判断当前行是否是注释
            if (line.startsWith("/*")) {
                isNote = true;
            }

            //注释部分
            if (isNote) {
                lineNotes++;
            } else {
                if (line.startsWith("//")) {    //注释行
                    lineNotes++;
                } else if (line.isEmpty()) {    //空白行
                    lineBlank++;
                } else {                        //代码行
                    lineCode++;
                }
            }

            //注释结束
            if (line.endsWith("*/")) {
                isNote = false;
            }
        }
    }
}

void frmCountCode::on_btnOpenFile_clicked()
{
    QString filter = QString("代码文件(%1)").arg(ui->txtFilter->text().trimmed());
    QStringList files = QFileDialog::getOpenFileNames(this, "选择文件", "./", filter);
    if (files.size() > 0) {
        ui->txtFile->setText(files.join("|"));
        countCode(files);
    }
}

void frmCountCode::on_btnOpenPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择目录", "./",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        ui->txtPath->setText(path);
        listFile.clear();
        countCode(path);
        countCode(listFile);
    }
}

void frmCountCode::on_btnClear_clicked()
{
    ui->txtCount->setText("0");
    ui->txtSize->setText("0");
    ui->txtRow->setText("0");

    ui->txtCode->setText("0");
    ui->txtNote->setText("0");
    ui->txtBlank->setText("0");

    ui->labPercentCode->setText("0%");
    ui->labPercentNote->setText("0%");
    ui->labPercentBlank->setText("0%");
    ui->tableWidget->setRowCount(0);
}
