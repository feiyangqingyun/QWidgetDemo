/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FORMBUILDER_H
#define FORMBUILDER_H

#include <QtDesigner/uilib_global.h>
#include <QtDesigner/QAbstractFormBuilder>

#include <QtCore/QStringList>
#include <QtCore/QMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
#if 0
// pragma for syncqt, don't remove.

#pragma qt_class(QFormBuilder)
#endif

class QDesignerCustomWidgetInterface;

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal
{
#endif

class QDESIGNER_UILIB_EXPORT QFormBuilder: public QAbstractFormBuilder
{
public:
    QFormBuilder();
    virtual ~QFormBuilder();

    QStringList pluginPaths() const;

    void clearPluginPaths();
    void addPluginPath(const QString &pluginPath);
    void setPluginPath(const QStringList &pluginPaths);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const;

protected:
    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);
    virtual QLayout *create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget);
    virtual QLayoutItem *create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget);
    virtual QAction *create(DomAction *ui_action, QObject *parent);
    virtual QActionGroup *create(DomActionGroup *ui_action_group, QObject *parent);

    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
    virtual QLayout *createLayout(const QString &layoutName, QObject *parent, const QString &name);

    virtual void createConnections(DomConnections *connections, QWidget *widget);

    virtual bool addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout);
    virtual bool addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

    virtual void updateCustomWidgets();
    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);

    static QWidget *widgetByName(QWidget *topLevel, const QString &name);

private:
    QStringList m_pluginPaths;
    QMap<QString, QDesignerCustomWidgetInterface*> m_customWidgets;
};

#ifdef QFORMINTERNAL_NAMESPACE
}
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // FORMBUILDER_H
