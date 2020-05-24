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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_TOOLBOX_H
#define QDESIGNER_TOOLBOX_H

#include "shared_global_p.h"
#include "qdesigner_propertysheet_p.h"
#include "qdesigner_utils_p.h"
#include <QtGui/QPalette>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
    class PromotionTaskMenu;
}

class QToolBox;

class QAction;
class QMenu;

class QDESIGNER_SHARED_EXPORT QToolBoxHelper : public QObject
{
    Q_OBJECT

    explicit QToolBoxHelper(QToolBox *toolbox);
public:
    // Install helper on QToolBox
    static void install(QToolBox *toolbox);
    static QToolBoxHelper *helperOf(const QToolBox *toolbox);
    // Convenience to add a menu on a toolbox
    static QMenu *addToolBoxContextMenuActions(const QToolBox *toolbox, QMenu *popup);

    QPalette::ColorRole currentItemBackgroundRole() const;
    void setCurrentItemBackgroundRole(QPalette::ColorRole role);

    bool eventFilter(QObject *watched, QEvent *event);
    // Add context menu and return page submenu or 0.

    QMenu *addContextMenuActions(QMenu *popup) const;

private slots:
    void removeCurrentPage();
    void addPage();
    void addPageAfter();
    void changeOrder();

private:
    QToolBox *m_toolbox;
    QAction *m_actionDeletePage;
    QAction *m_actionInsertPage;
    QAction *m_actionInsertPageAfter;
    QAction *m_actionChangePageOrder;
    qdesigner_internal::PromotionTaskMenu* m_pagePromotionTaskMenu;
};

// PropertySheet to handle the page properties
class QDESIGNER_SHARED_EXPORT QToolBoxWidgetPropertySheet : public QDesignerPropertySheet {
public:
    explicit QToolBoxWidgetPropertySheet(QToolBox *object, QObject *parent = 0);

    virtual void setProperty(int index, const QVariant &value);
    virtual QVariant property(int index) const;
    virtual bool reset(int index);
    virtual bool isEnabled(int index) const;

    // Check whether the property is to be saved. Returns false for the page
    // properties (as the property sheet has no concept of 'stored')
    static bool checkProperty(const QString &propertyName);

private:
    enum ToolBoxProperty { PropertyCurrentItemText, PropertyCurrentItemName, PropertyCurrentItemIcon,
                           PropertyCurrentItemToolTip,  PropertyTabSpacing, PropertyToolBoxNone };

    static ToolBoxProperty toolBoxPropertyFromName(const QString &name);
    QToolBox *m_toolBox;
    struct PageData
        {
        qdesigner_internal::PropertySheetStringValue text;
        qdesigner_internal::PropertySheetStringValue tooltip;
        qdesigner_internal::PropertySheetIconValue icon;
        };
    QMap<QWidget *, PageData> m_pageToData;
};

typedef QDesignerPropertySheetFactory<QToolBox, QToolBoxWidgetPropertySheet> QToolBoxWidgetPropertySheetFactory;

QT_END_NAMESPACE

#endif // QDESIGNER_TOOLBOX_H
