#include "frmshortcut.h"
#include "ui_frmshortcut.h"
#include "qxtglobalshortcut.h"
#include "qdatetime.h"
#include "qdebug.h"

frmShortCut::frmShortCut(QWidget *parent) : QWidget(parent), ui(new Ui::frmShortCut)
{
    ui->setupUi(this);
    this->initForm();
}

frmShortCut::~frmShortCut()
{
    delete ui;
}

void frmShortCut::initForm()
{
    //实例化热键类
    QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(QKeySequence("ctrl+x"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(shortcut()));
}

void frmShortCut::shortcut()
{
#if 1
    //如果是最小化则显示,否则最小化
    if (this->isMinimized()) {
        this->showNormal();
        this->activateWindow();
    } else {
        this->showMinimized();
    }
#else
    ui->label->setText("activated  " + QTime::currentTime().toString("hh:mm:ss zzz"));
#endif
}
