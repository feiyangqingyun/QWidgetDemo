#include "frmmain.h"
#include "ui_frmmain.h"
#include "quihelper.h"

#include "frmtcpclient.h"
#include "frmtcpserver.h"
#include "frmudpclient.h"
#include "frmudpserver.h"
#ifdef websocket
#include "frmwebclient.h"
#include "frmwebserver.h"
#endif

frmMain::frmMain(QWidget *parent) : QWidget(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::initForm()
{
    ui->tabWidget->addTab(new frmTcpClient, "TCP客户端");
    ui->tabWidget->addTab(new frmTcpServer, "TCP服务端");
    ui->tabWidget->addTab(new frmUdpClient, "UDP客户端");
    ui->tabWidget->addTab(new frmUdpServer, "UDP服务端");
#ifdef websocket
    ui->tabWidget->addTab(new frmWebClient, "WEB客户端");
    ui->tabWidget->addTab(new frmWebServer, "WEB服务端");
#endif
#ifdef Q_OS_WASM
    AppConfig::CurrentIndex = 4;
#endif
}

void frmMain::initConfig()
{
    ui->tabWidget->setCurrentIndex(AppConfig::CurrentIndex);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(saveConfig()));
}

void frmMain::saveConfig()
{
    AppConfig::CurrentIndex = ui->tabWidget->currentIndex();
    AppConfig::writeConfig();
}
