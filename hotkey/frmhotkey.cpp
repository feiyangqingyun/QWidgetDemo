#include "frmhotkey.h"
#include "ui_frmhotkey.h"
#include "qhotkey.h"
#include "qdatetime.h"
#include "qdebug.h"

frmHotKey::frmHotKey(QWidget *parent) : QWidget(parent), ui(new Ui::frmHotKey)
{
    ui->setupUi(this);
    this->initForm();
}

frmHotKey::~frmHotKey()
{
    delete ui;
}

void frmHotKey::initForm()
{
    //实例化热键类
    QHotkey *hotkey = new QHotkey(QKeySequence("ctrl+x"), true, this);
    connect(hotkey, SIGNAL(activated()), this, SLOT(shortcut()));
}

void frmHotKey::shortcut()
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
