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

#include "metadatabase_p.h"
#include "widgetdatabase_p.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>

// Qt
#include <QtGui/QWidget>
#include <QtCore/qalgorithms.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace {
    const bool debugMetaDatabase = false;
}

namespace qdesigner_internal {

MetaDataBaseItem::MetaDataBaseItem(QObject *object)
    : m_object(object),
      m_enabled(true)
{
}

MetaDataBaseItem::~MetaDataBaseItem()
{
}

QString MetaDataBaseItem::name() const
{
    Q_ASSERT(m_object);
    return m_object->objectName();
}

void MetaDataBaseItem::setName(const QString &name)
{
    Q_ASSERT(m_object);
    m_object->setObjectName(name);
}

QString MetaDataBaseItem::customClassName() const
{
    return m_customClassName;
}
void MetaDataBaseItem::setCustomClassName(const QString &customClassName)
{
    m_customClassName = customClassName;
}


MetaDataBaseItem::TabOrder  MetaDataBaseItem::tabOrder() const
{
    return m_tabOrder;
}

void MetaDataBaseItem::setTabOrder(const TabOrder &tabOrder)
{
    m_tabOrder = tabOrder;
}

bool MetaDataBaseItem::enabled() const
{
    return m_enabled;
}

void MetaDataBaseItem::setEnabled(bool b)
{
    m_enabled = b;
}

QString MetaDataBaseItem::script() const
{
    return m_script;
}

void MetaDataBaseItem::setScript(const QString &script)
{
    m_script = script;
}

QStringList MetaDataBaseItem::fakeSlots() const
{
    return m_fakeSlots;
}

void MetaDataBaseItem::setFakeSlots(const QStringList &fs)
{
    m_fakeSlots = fs;
}

QStringList MetaDataBaseItem::fakeSignals() const
{
     return m_fakeSignals;
}

void MetaDataBaseItem::setFakeSignals(const QStringList &fs)
{
    m_fakeSignals = fs;
}

// -----------------------------------------------------
MetaDataBase::MetaDataBase(QDesignerFormEditorInterface *core, QObject *parent)
    : QDesignerMetaDataBaseInterface(parent),
      m_core(core)
{
}

MetaDataBase::~MetaDataBase()
{
    qDeleteAll(m_items);
}

MetaDataBaseItem *MetaDataBase::metaDataBaseItem(QObject *object) const
{
    MetaDataBaseItem *i = m_items.value(object);
    if (i == 0 || !i->enabled())
        return 0;
    return i;
}

void MetaDataBase::add(QObject *object)
{
    MetaDataBaseItem *item = m_items.value(object);
    if (item != 0) {
        item->setEnabled(true);
        if (debugMetaDatabase) {
            qDebug() << "MetaDataBase::add: Existing item for " << object->metaObject()->className() << item->name();
        }
        return;
    }

    item = new MetaDataBaseItem(object);
    m_items.insert(object, item);
    if (debugMetaDatabase) {
        qDebug() << "MetaDataBase::add: New item " << object->metaObject()->className() << item->name();
    }
    connect(object, SIGNAL(destroyed(QObject*)),
        this, SLOT(slotDestroyed(QObject*)));

    emit changed();
}

void MetaDataBase::remove(QObject *object)
{
    Q_ASSERT(object);

    if (MetaDataBaseItem *item = m_items.value(object)) {
        item->setEnabled(false);
        emit changed();
    }
}

QList<QObject*> MetaDataBase::objects() const
{
    QList<QObject*> result;

    ItemMap::const_iterator it = m_items.begin();
    for (; it != m_items.end(); ++it) {
        if (it.value()->enabled())
            result.append(it.key());
    }

    return result;
}

QDesignerFormEditorInterface *MetaDataBase::core() const
{
    return m_core;
}

void MetaDataBase::slotDestroyed(QObject *object)
{
    if (m_items.contains(object)) {
        MetaDataBaseItem *item = m_items.value(object);
        delete item;
        m_items.remove(object);
    }
}

// promotion convenience
QDESIGNER_SHARED_EXPORT bool promoteWidget(QDesignerFormEditorInterface *core,QWidget *widget,const QString &customClassName)
{

    MetaDataBase *db = qobject_cast<MetaDataBase *>(core->metaDataBase());
    if (!db)
        return false;
    MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item) {
        db ->add(widget);
        item = db->metaDataBaseItem(widget);
    }
    // Recursive promotion occurs if there is a plugin missing.
    const QString oldCustomClassName = item->customClassName();
    if (!oldCustomClassName.isEmpty()) {
        qDebug() << "WARNING: Recursive promotion of " << oldCustomClassName << " to " << customClassName
            << ". A plugin is missing.";
    }
    item->setCustomClassName(customClassName);  
    if (debugMetaDatabase) {
        qDebug() << "Promoting " << widget->metaObject()->className() << " to " << customClassName;
    }
    return true;
}

QDESIGNER_SHARED_EXPORT void demoteWidget(QDesignerFormEditorInterface *core,QWidget *widget)
{
    MetaDataBase *db = qobject_cast<MetaDataBase *>(core->metaDataBase());
    if (!db)
        return;
    MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    item->setCustomClassName(QString());
    if (debugMetaDatabase) {
        qDebug() << "Demoting " << widget;
    }
}

QDESIGNER_SHARED_EXPORT bool isPromoted(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const MetaDataBase *db = qobject_cast<const MetaDataBase *>(core->metaDataBase());
    if (!db)
        return false;
    const MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item)
        return false;
    return !item->customClassName().isEmpty();
}

QDESIGNER_SHARED_EXPORT QString promotedCustomClassName(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const MetaDataBase *db = qobject_cast<const MetaDataBase *>(core->metaDataBase());
    if (!db)
        return QString();
    const MetaDataBaseItem *item = db->metaDataBaseItem(widget);
    if (!item)
        return QString();
    return item->customClassName();
}
 
QDESIGNER_SHARED_EXPORT QString promotedExtends(QDesignerFormEditorInterface *core, QWidget* widget)
{
    const QString customClassName = promotedCustomClassName(core,widget);
    if (customClassName.isEmpty())
        return QString();
    const int i = core->widgetDataBase()->indexOfClassName(customClassName);
    if (i == -1)
        return QString();
    return core->widgetDataBase()->item(i)->extends();
}


} // namespace qdesigner_internal

QT_END_NAMESPACE
