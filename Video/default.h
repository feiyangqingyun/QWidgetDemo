#ifndef DEFAULT_H
#define DEFAULT_H
#include <QString>
#include <QVariant>
#include "HCNetSDK.h"

struct loginInfo{
    QString username;
    QString RealName;
    QString Addr;
    QString PhoneNumber;
    QString RootNodeID;
    int     Limit;
};

struct TreeNodeData{
    QString Code;
    QString NodeTable;
};
Q_DECLARE_METATYPE(TreeNodeData)
struct Camera{
    QString CameraCode;
    QString OutNetIPV4;
    QString OutNetIPV6;
    quint16 OutNetPort;
    QString InNetIPV4;
    QString InNetIPV6;
    quint16 InNetPort;
    QString UserName;
    QString Password;
    int     LoginID;
    //int     PlayID;
    int     Factory;
    NET_DVR_DEVICEINFO_V30 CameraLoginInfo;
    Camera(){
        memset(&CameraLoginInfo,0,sizeof(NET_DVR_DEVICEINFO_V30));
    }
};
Q_DECLARE_METATYPE(Camera)
#endif // DEFAULT_H
