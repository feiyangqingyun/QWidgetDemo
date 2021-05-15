#ifndef QUIHEAD_H
#define QUIHEAD_H

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef TIME
#define TIME qPrintable(QTime::currentTime().toString("HH:mm:ss"))
#endif
#ifndef QDATE
#define QDATE qPrintable(QDate::currentDate().toString("yyyy-MM-dd"))
#endif
#ifndef QTIME
#define QTIME qPrintable(QTime::currentTime().toString("HH-mm-ss"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif
#ifndef STRDATETIME
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#endif
#ifndef STRDATETIMEMS
#define STRDATETIMEMS qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))
#endif

//数据 0
#define data_zero (char)0x00

//定义换行符
#ifdef Q_OS_WIN
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

//定义无边框标题栏高度+对话框最小宽高
#ifdef __arm__
#define QUITitleMinSize 40
#define QUIDialogMinWidth 350
#define QUIDialogMinHeight 180
#define QUIBtnMinWidth 90
#define QUIIconWidth 22
#define QUIIconHeight 22
#else
#define QUITitleMinSize 30
#define QUIDialogMinWidth 280
#define QUIDialogMinHeight 150
#define QUIBtnMinWidth 80
#define QUIIconWidth 18
#define QUIIconHeight 18
#endif

#include "head.h"
#include "iconhelper.h"
#include "iconfont.h"

#include "quihelper.h"
#include "quiconfig.h"
#include "quistyle.h"
#include "quihead.h"
#include "quimessagebox.h"
#include "quitipbox.h"
#include "quidateselect.h"
#include "quiinputbox.h"

#endif // QUIHEAD_H
