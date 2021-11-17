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

#include "promotionmodel_p.h"
#include "widgetdatabase_p.h"

#include <QtDesigner/QDesignerWidgetDataBaseInterface>
#include <QtDesigner/QDesignerPromotionInterface>
#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtGui/QStandardItem>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

namespace {
    typedef QList<QStandardItem *> StandardItemList;

    // Model columns.
    enum { ClassNameColumn, IncludeFileColumn, IncludeTypeColumn, ReferencedColumn, NumColumns };

    // Create a model row.
    StandardItemList modelRow() {
        StandardItemList rc;
        for (int i = 0; i < NumColumns; i++) {
            rc.push_back(new QStandardItem());
        }
        return rc;
    }

    // Create a model row for a base class (read-only, cannot be selected).
    StandardItemList baseModelRow(const QDesignerWidgetDataBaseItemInterface *dbItem) {
        StandardItemList rc =  modelRow();

        rc[ClassNameColumn]->setText(dbItem->name());
        for (int i = 0; i < NumColumns; i++) {
            rc[i]->setFlags(Qt::ItemIsEnabled);
        }
        return rc;
    }

    // Create an editable model row for a promoted class.
    StandardItemList promotedModelRow(const QDesignerWidgetDataBaseInterface *widgetDataBase,
                                      QDesignerWidgetDataBaseItemInterface *dbItem,
                                      bool referenced = false) {

        const int index = widgetDataBase->indexOf(dbItem);

        // Associate user data: database index and enabled flag
        QVariantList userDataList;
        userDataList.push_back(QVariant(index));
        userDataList.push_back(QVariant(referenced));
        const QVariant userData(userDataList);

        StandardItemList rc =  modelRow();
        // name
        rc[ClassNameColumn]->setText(dbItem->name());
        rc[ClassNameColumn]->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
        rc[ClassNameColumn]->setData(userData);
        // header
        const qdesigner_internal::IncludeSpecification spec = qdesigner_internal::includeSpecification(dbItem->includeFile());
        rc[IncludeFileColumn]->setText(spec.first);
        rc[IncludeFileColumn]->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
        rc[IncludeFileColumn]->setData(userData);
        // global include
        rc[IncludeTypeColumn]->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsUserCheckable);
        rc[IncludeTypeColumn]->setData(userData);
        rc[IncludeTypeColumn]->setCheckState(spec.second == qdesigner_internal::IncludeGlobal ? Qt::Checked : Qt::Unchecked);
        // referenced
        rc[ReferencedColumn]->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        rc[ClassNameColumn]->setData(userData);
        if (!referenced) {
            //: Usage of promoted widgets
            static const QString notUsed = QCoreApplication::translate("PromotionModel", "Not used");
            rc[ReferencedColumn]->setText(notUsed);
        }
        return rc;
    }
}

namespace qdesigner_internal {

    PromotionModel::PromotionModel(QDesignerFormEditorInterface *core) :
        m_core(core)
    {
        connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotItemChanged(QStandardItem*)));
    }

    void PromotionModel::initializeHeaders() {
        setColumnCount(NumColumns);
        QStringList horizontalLabels(tr("Name"));
        horizontalLabels += tr("Header file");
        horizontalLabels += tr("Global include");
        horizontalLabels += tr("Usage");
        setHorizontalHeaderLabels (horizontalLabels);
    }

    void PromotionModel::updateFromWidgetDatabase() {
        typedef QDesignerPromotionInterface::PromotedClasses PromotedClasses;

        clear();
        initializeHeaders();

        // retrieve list of pairs from DB and convert into a tree structure.
        // Set the item index as user data on the item.
        const PromotedClasses promotedClasses = m_core->promotion()->promotedClasses();

        if (promotedClasses.empty())
            return;

        const QSet<QString> usedPromotedClasses = m_core->promotion()->referencedPromotedClassNames();

        QDesignerWidgetDataBaseInterface *widgetDataBase = m_core->widgetDataBase();
        QDesignerWidgetDataBaseItemInterface *baseClass = 0;
        QStandardItem *baseItem = 0;

        const PromotedClasses::const_iterator bcend = promotedClasses.constEnd();
        for (PromotedClasses::const_iterator it = promotedClasses.constBegin(); it !=  bcend; ++it) {
            // Start a new base class?
            if (baseClass !=  it->baseItem) {
                baseClass =  it->baseItem;
                const StandardItemList baseRow = baseModelRow(it->baseItem);
                baseItem = baseRow.front();
                appendRow(baseRow);
            }
            Q_ASSERT(baseItem);
            // Append derived
            baseItem->appendRow(promotedModelRow(widgetDataBase, it->promotedItem, usedPromotedClasses.contains(it->promotedItem->name())));
        }
    }

    void PromotionModel::slotItemChanged(QStandardItem * changedItem) {
        // Retrieve DB item
        bool referenced;
        QDesignerWidgetDataBaseItemInterface *dbItem = databaseItem(changedItem, &referenced);
        Q_ASSERT(dbItem);
        // Change header or type
        switch (changedItem->column()) {
        case ClassNameColumn:
            emit classNameChanged(dbItem,  changedItem->text());
            break;
        case IncludeTypeColumn:
        case IncludeFileColumn: {
            // Get both file and type items via parent.
            const QStandardItem *baseClassItem = changedItem->parent();
            const QStandardItem *fileItem = baseClassItem->child(changedItem->row(), IncludeFileColumn);
            const QStandardItem *typeItem =  baseClassItem->child(changedItem->row(), IncludeTypeColumn);
            emit includeFileChanged(dbItem, buildIncludeFile(fileItem->text(), typeItem->checkState() == Qt::Checked ? IncludeGlobal : IncludeLocal));
        }
            break;
        }
    }

    QDesignerWidgetDataBaseItemInterface *PromotionModel::databaseItemAt(const QModelIndex &index, bool *referenced) const {
        if (const QStandardItem *item = itemFromIndex (index))
            return databaseItem(item, referenced);

        *referenced = false;
        return 0;
    }

    QDesignerWidgetDataBaseItemInterface *PromotionModel::databaseItem(const QStandardItem * item, bool *referenced) const {
        // Decode user data associated with item.
        const QVariant data =  item->data();
        if (data.type() != QVariant::List) {
            *referenced = false;
            return 0;
        }

        const QVariantList dataList = data.toList();
        const int index = dataList[0].toInt();
        *referenced     = dataList[1].toBool();
        return  m_core->widgetDataBase()->item(index);
    }

    QModelIndex PromotionModel::indexOfClass(const QString &className) const {
        const StandardItemList matches = findItems (className, Qt::MatchFixedString|Qt::MatchCaseSensitive|Qt::MatchRecursive);
        return matches.empty() ? QModelIndex() : indexFromItem (matches.front());
    }
} // namespace qdesigner_internal

QT_END_NAMESPACE
