#include "frmmain.h"
#include "ui_frmmain.h"
#include "head.h"

frmMain::frmMain(QWidget *parent) : QMainWindow(parent), ui(new Ui::frmMain)
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
    ui->rbtn1->setChecked(true);
    ui->ck2->setChecked(true);
    ui->ck3->setCheckState(Qt::PartiallyChecked);

    ui->textEdit->setText("QQ：517216493\nWX：feiyangqingyun");
    ui->treeWidget->expandAll();

    int rowHeight = 23;
    ui->tableWidget->horizontalHeader()->setFixedHeight(rowHeight);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(rowHeight);

    //加载鼠标右键菜单翻译文件
    QTranslator *translator1 = new QTranslator(qApp);
    if (translator1->load(":/qm/qt_zh_CN.qm")) {
        qApp->installTranslator(translator1);
    }

    //加载富文本框鼠标右键菜单翻译文件
    QTranslator *translator2 = new QTranslator(qApp);
    if (translator2->load(":/qm/widgets.qm")) {
        qApp->installTranslator(translator2);
    }

    on_btnStyle1_clicked();
}

void frmMain::loadStyle(const QString &qssFile)
{
    //开启计时
    QElapsedTimer time;
    time.start();

    //加载样式表
    QString qss;
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        //in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }

        file.close();
        qss = list.join("\n");
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        //用时主要在下面这句
        qApp->setStyleSheet(qss);
    }

    qDebug() << "用时:" << time.elapsed();
}

void frmMain::on_btnStyle1_clicked()
{
    loadStyle(":/qss/lightblue.css");
}

void frmMain::on_btnStyle2_clicked()
{
    loadStyle(":/qss/flatgray.css");
}

void frmMain::on_btnStyle3_clicked()
{
    loadStyle(":/qss/blacksoft.css");
}

void frmMain::on_btnInfo_clicked()
{
    //QFileDialog::getOpenFileName(this, "", "", "", 0, QFileDialog::DontUseNativeDialog);
    //QFileDialog::getExistingDirectory(0, "", "", QFileDialog::DontUseNativeDialog);
}

