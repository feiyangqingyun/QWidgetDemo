#ifndef MDKINCLUDE_H
#define MDKINCLUDE_H

//引入头文件
#include "qglobal.h"
#include "qdebug.h"
#include "qstringlist.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,5,0))
#include <QOpenGLWidget>
#define OpenGLWidget QOpenGLWidget
#else
#include <QGLWidget>
#define OpenGLWidget QGLWidget
#endif

#include "Player.h"
//using namespace mdk;

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

#include "qdatetime.h"
#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif

#endif // MDKINCLUDE_H
