#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    userID=-1;
    previewID=-1;
    ui->setupUi(this);
    if(initSDK())
        ui->label->setText("SDK init Success!");
}

Widget::~Widget()
{
    if(uninitSDK())
        qDebug()<<"SDK uninit Success!"<<endl;
    delete ui;
}

bool Widget::initSDK()
{
    bool isok = NET_DVR_Init();
    if(isok == false)
    {
        qDebug()<< "NET_DVR_Init error;error number is " <<NET_DVR_GetLastError();
        QMessageBox::warning(this,"error","NET_DVR_Init error;error number is "+QString::number(NET_DVR_GetLastError()));
        return isok;
    }
    //设置连接时间与重连时间
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    return isok;
}

bool Widget::uninitSDK()
{
    bool isok = NET_DVR_Cleanup();
    if(isok == false)
    {
        qDebug()<< "NET_DVR_Cleanup error;error number is " <<NET_DVR_GetLastError();
        QMessageBox::warning(this,"error","NET_DVR_Cleanup error;error number "+QString::number(NET_DVR_GetLastError()));
        return isok;
    }
    return isok;
}

bool Widget::getCameraInfo()
{
    host = ui->lineEdit_ip->text();
    port = ui->spinBox_port->value();
    username = ui->lineEdit_username->text();
    password = ui->lineEdit_password->text();

    if(port <= 0){return false;}
    if(host.isEmpty()||username.isEmpty()||password.isEmpty()){
        return false;
    }
    return true;
}

long Widget::play(HWND hWnd, NET_DVR_PREVIEWINFO struPlayInfo)
{
    struPlayInfo={0};
    struPlayInfo.hPlayWnd = hWnd;//需要 SDK 解码时句柄设为有效值，仅取流不解码时可设为空
    struPlayInfo.lChannel = 1;//预览通道号
    struPlayInfo.dwStreamType = 1;//码流类型：0-主码流，1-子码流，2-三码流，3-虚拟码流，以此类推
    struPlayInfo.dwLinkMode = 0;//0- TCP 方式，1- UDP 方式，2- 多播方式，3- RTP 方式，4-RTP/RTSP，5-RSTP/HTTP
    struPlayInfo.bBlocked = 1;//0- 非阻塞取流，1- 阻塞取流

    long IRealPlayHandle = NET_DVR_RealPlay_V40(userID,&struPlayInfo,NULL,NULL);

    if(IRealPlayHandle <0)
    {
        qDebug() << "NET_DVR_RealPlay_V40 error;error number " <<NET_DVR_GetLastError();
        QMessageBox::warning(this,"error","NET_DVR_RealPlay_V40 error;error number "+QString::number(NET_DVR_GetLastError()));
        NET_DVR_Logout(userID);
        userID=-1;
        return -1;
    }

    return IRealPlayHandle;
}

void Widget::OnClickedLogin()
{
    NET_DVR_DEVICEINFO_V30 DeviceInfoTmp;
    userID=NET_DVR_Login_V30(QString_2_char(host),(WORD)port,
                             QString_2_char(username),
                             QString_2_char(password),&DeviceInfoTmp);
    if(userID<0)
    {
        qDebug() << "NET_DVR_Login_V30 error;" << "error number is " << NET_DVR_GetLastError();
        QMessageBox::warning(this,"error","NET_DVR_Login_V30 error;error number "+QString::number(NET_DVR_GetLastError()));
        return;
    }
    qDebug()<<"Login Success,userID:" << userID<<endl;
}

void Widget::on_pushButton_ok_clicked()
{
    if(!getCameraInfo()){return;}
    OnClickedLogin();
    QTimer::singleShot(2000,this, [=](){
        OnClickedPreview();//预览
    });
}

void Widget::on_pushButton_close_clicked()
{

    OnClickedStopPreview();//停止预览
    QTimer::singleShot(1000,this, [=](){
        OnClickedLogout();//注销
    });
}

void Widget::closeEvent(QCloseEvent *e)
{

}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        OnClickedCapture();//抓图
        qDebug()<<"抓图";
    }
}
void Widget::OnClickedLogout()
{
    if(userID!=-1)
    {
        if(NET_DVR_Logout(userID)==false)
        {
            qDebug() << "NET_DVR_Logout error;" << "error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this,"error","NET_DVR_Logout error;error number "+QString::number(NET_DVR_GetLastError()));
            return;
        }
        userID=-1;
        qDebug() <<"Logout Success!"<<endl;
    }
}

void Widget::OnClickedPreview()
{
    if(userID==-1)
    {
        qDebug() <<"Need Login!"<<endl;
        QMessageBox::warning(this,"error","Need Login!");
        return;
    }

    HWND hWnd = (HWND)ui->label->winId();
    NET_DVR_PREVIEWINFO struPlayInfo;
    previewID = play(hWnd,struPlayInfo);
    if(previewID==-1)
    {
        return;
    }
    qDebug()<<"Preview Success!" << userID<<endl;
}

void Widget::OnClickedStopPreview()
{
    if(previewID!=-1)
    {
        if(NET_DVR_StopRealPlay(previewID)==false)
        {
            qDebug() << "NET_DVR_StopRealPlay error;" << "error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this,"error","NET_DVR_StopRealPlay error;error number "+QString::number(NET_DVR_GetLastError()));
            return;
        }
        previewID=-1;
        qDebug() <<"Stop Preview Success!"<<endl;
    }
}

void Widget::OnClickedCapture()
{

    if(userID!=-1)
    {
        NET_DVR_JPEGPARA IpJpegPara2;
        IpJpegPara2.wPicQuality=0;
        IpJpegPara2.wPicSize=0xff;

        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy-MM-dd_hh-mm-ss_zzz");
        current_date= current_date+".jpg";
        char*  fileName;
        QByteArray ba = current_date.toLatin1(); // must
        fileName=ba.data();

        if(NET_DVR_CaptureJPEGPicture(userID,1,&IpJpegPara2,fileName)==false)
        {
            qDebug() << "NET_DVR_CaptureJPEGPicture error;" << "error number is " << NET_DVR_GetLastError();
            QMessageBox::warning(this,"error","NET_DVR_CaptureJPEGPicture error;error number "+QString::number(NET_DVR_GetLastError()));
            return;
        }
        qDebug() <<"Capture Success!"<<endl;
    }
}

char *Widget::QString_2_char(QString &uu)
{
    return uu.toLatin1().data();
}
