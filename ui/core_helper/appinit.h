#ifndef APPINIT_H
#define APPINIT_H

#include <QObject>
#include "singleton.h"

class AppInit : public QObject
{
    Q_OBJECT SINGLETON_DECL(AppInit)
public:
    explicit AppInit(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void start();
};

#endif // APPINIT_H
