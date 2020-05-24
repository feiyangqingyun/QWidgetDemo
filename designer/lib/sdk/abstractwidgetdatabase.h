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

#ifndef ABSTRACTWIDGETDATABASE_H
#define ABSTRACTWIDGETDATABASE_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIcon;
class QString;
class QDesignerFormEditorInterface;
class QDebug;

class QDesignerWidgetDataBaseItemInterface
{
public:
    virtual ~QDesignerWidgetDataBaseItemInterface() {}

    virtual QString name() const = 0;
    virtual void setName(const QString &name) = 0;

    virtual QString group() const = 0;
    virtual void setGroup(const QString &group) = 0;

    virtual QString toolTip() const = 0;
    virtual void setToolTip(const QString &toolTip) = 0;

    virtual QString whatsThis() const = 0;
    virtual void setWhatsThis(const QString &whatsThis) = 0;

    virtual QString includeFile() const = 0;
    virtual void setIncludeFile(const QString &includeFile) = 0;

    virtual QIcon icon() const = 0;
    virtual void setIcon(const QIcon &icon) = 0;

    virtual bool isCompat() const = 0;
    virtual void setCompat(bool compat) = 0;

    virtual bool isContainer() const = 0;
    virtual void setContainer(bool container) = 0;

    virtual bool isCustom() const = 0;
    virtual void setCustom(bool custom) = 0;

    virtual QString pluginPath() const = 0;
    virtual void setPluginPath(const QString &path) = 0;

    virtual bool isPromoted() const = 0;
    virtual void setPromoted(bool b) = 0;

    virtual QString extends() const = 0;
    virtual void setExtends(const QString &s) = 0;

    virtual void setDefaultPropertyValues(const QList<QVariant> &list) = 0;
    virtual QList<QVariant> defaultPropertyValues() const = 0;
};

class QDESIGNER_SDK_EXPORT QDesignerWidgetDataBaseInterface: public QObject
{
    Q_OBJECT
public:
    QDesignerWidgetDataBaseInterface(QObject *parent = 0);
    virtual ~QDesignerWidgetDataBaseInterface();

    virtual int count() const;
    virtual QDesignerWidgetDataBaseItemInterface *item(int index) const;

    virtual int indexOf(QDesignerWidgetDataBaseItemInterface *item) const;
    virtual void insert(int index, QDesignerWidgetDataBaseItemInterface *item);
    virtual void append(QDesignerWidgetDataBaseItemInterface *item);

    virtual int indexOfObject(QObject *object, bool resolveName = true) const;
    virtual int indexOfClassName(const QString &className, bool resolveName = true) const;

    virtual QDesignerFormEditorInterface *core() const;

    bool isContainer(QObject *object, bool resolveName = true) const;
    bool isCustom(QObject *object, bool resolveName = true) const;

Q_SIGNALS:
    void changed();

protected:
    QList<QDesignerWidgetDataBaseItemInterface *> m_items;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTWIDGETDATABASE_H
