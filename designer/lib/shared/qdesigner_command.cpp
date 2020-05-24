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

#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "qdesigner_utils_p.h"
#include "layout_p.h"
#include "qlayout_widget_p.h"
#include "qdesigner_widget_p.h"
#include "qdesigner_menu_p.h"
#include "shared_enums_p.h"
#include "metadatabase_p.h"
#include "formwindowbase_p.h"
#include <abstractformbuilder.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerActionEditorInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerLayoutDecorationExtension>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerObjectInspectorInterface>
#include <QtDesigner/QDesignerIntegrationInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtCore/qdebug.h>
#include <QtCore/QTextStream>
#include <QtCore/QQueue>

#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QListWidget>
#include <QtGui/QComboBox>
#include <QtGui/QSplitter>
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QWizardPage>
#include <QtGui/QApplication>
#include <QtGui/QFormLayout>

Q_DECLARE_METATYPE(QWidgetList)

QT_BEGIN_NAMESPACE

static inline void setPropertySheetWindowTitle(const QDesignerFormEditorInterface *core, QObject *o, const QString &t)
{
    if (QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), o)) {
        const int idx = sheet->indexOf(QLatin1String("windowTitle"));
        if (idx != -1) {
            sheet->setProperty(idx, t);
            sheet->setChanged(idx, true);
        }
    }
}

namespace qdesigner_internal {

// Helpers for the dynamic properties that store Z/Widget order
static const char *widgetOrderPropertyC = "_q_widgetOrder";
static const char *zOrderPropertyC = "_q_zOrder";

static void addToWidgetListDynamicProperty(QWidget *parentWidget, QWidget *widget, const char *name, int index = -1)
{
    QWidgetList list = qVariantValue<QWidgetList>(parentWidget->property(name));
    list.removeAll(widget);
    if (index >= 0 && index < list.size()) {
        list.insert(index, widget);
    } else {
        list.append(widget);
    }
    parentWidget->setProperty(name, qVariantFromValue(list));
}

static int removeFromWidgetListDynamicProperty(QWidget *parentWidget, QWidget *widget, const char *name)
{
    QWidgetList list = qVariantValue<QWidgetList>(parentWidget->property(name));
    const int firstIndex = list.indexOf(widget);
    if (firstIndex != -1) {
        list.removeAll(widget);
        parentWidget->setProperty(name, qVariantFromValue(list));
    }
    return firstIndex;
}

// ---- InsertWidgetCommand ----
InsertWidgetCommand::InsertWidgetCommand(QDesignerFormWindowInterface *formWindow)  :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_insertMode(QDesignerLayoutDecorationExtension::InsertWidgetMode),
    m_layoutHelper(0),
    m_widgetWasManaged(false)
{
}

InsertWidgetCommand::~InsertWidgetCommand()
{
    delete m_layoutHelper;
}

void InsertWidgetCommand::init(QWidget *widget, bool already_in_form, int layoutRow, int layoutColumn)
{
    m_widget = widget;

    setText(QApplication::translate("Command", "Insert '%1'").arg(widget->objectName()));

    QWidget *parentWidget = m_widget->parentWidget();
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), parentWidget);

    m_insertMode = deco ? deco->currentInsertMode() : QDesignerLayoutDecorationExtension::InsertWidgetMode;
    if (layoutRow >= 0 && layoutColumn >= 0) {
        m_cell.first = layoutRow;
        m_cell.second = layoutColumn;
    } else {
        m_cell = deco ? deco->currentCell() : qMakePair(0, 0);
    }
    m_widgetWasManaged = already_in_form;
}

static void recursiveUpdate(QWidget *w)
{
    w->update();

    const QObjectList &l = w->children();
    const QObjectList::const_iterator cend = l.end();
    for ( QObjectList::const_iterator it = l.begin(); it != cend; ++it) {
        if (QWidget *w = qobject_cast<QWidget*>(*it))
            recursiveUpdate(w);
    }
}

void InsertWidgetCommand::redo()
{
    QWidget *parentWidget = m_widget->parentWidget();
    Q_ASSERT(parentWidget);

    addToWidgetListDynamicProperty(parentWidget, m_widget, widgetOrderPropertyC);
    addToWidgetListDynamicProperty(parentWidget, m_widget, zOrderPropertyC);

    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), parentWidget);

    if (deco != 0) {
        const LayoutInfo::Type type = LayoutInfo::layoutType(core, LayoutInfo::managedLayout(core, parentWidget));
        m_layoutHelper = LayoutHelper::createLayoutHelper(type);
        m_layoutHelper->pushState(core, parentWidget);
        if (type == LayoutInfo::Grid) {
            switch (m_insertMode) {
                case QDesignerLayoutDecorationExtension::InsertRowMode: {
                    deco->insertRow(m_cell.first);
                } break;

                case QDesignerLayoutDecorationExtension::InsertColumnMode: {
                    deco->insertColumn(m_cell.second);
                } break;

                default: break;
            } // end switch
        }
        deco->insertWidget(m_widget, m_cell);
    }

    if (!m_widgetWasManaged)
        formWindow()->manageWidget(m_widget);
    m_widget->show();
    formWindow()->emitSelectionChanged();

    if (parentWidget && parentWidget->layout()) {
        recursiveUpdate(parentWidget);
        parentWidget->layout()->invalidate();
    }

    refreshBuddyLabels();
}

void InsertWidgetCommand::undo()
{
    QWidget *parentWidget = m_widget->parentWidget();

    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), parentWidget);

    if (deco) {
        deco->removeWidget(m_widget);
        m_layoutHelper->popState(core, parentWidget);
    }

    if (!m_widgetWasManaged) {
        formWindow()->unmanageWidget(m_widget);
        m_widget->hide();
    }

    removeFromWidgetListDynamicProperty(parentWidget, m_widget, widgetOrderPropertyC);
    removeFromWidgetListDynamicProperty(parentWidget, m_widget, zOrderPropertyC);

    formWindow()->emitSelectionChanged();

    refreshBuddyLabels();
}

void InsertWidgetCommand::refreshBuddyLabels()
{
    typedef QList<QLabel*> LabelList;

    const LabelList label_list = qFindChildren<QLabel*>(formWindow());
    if (label_list.empty())
        return;

    const QString buddyProperty = QLatin1String("buddy");
    const QByteArray objectNameU8 = m_widget->objectName().toUtf8();
    // Re-set the buddy (The sheet locates the object by name and sets it)
    const LabelList::const_iterator cend = label_list.constEnd();
    for (LabelList::const_iterator it = label_list.constBegin(); it != cend; ++it ) {
        if (QDesignerPropertySheetExtension* sheet = propertySheet(*it)) {
            const int idx = sheet->indexOf(buddyProperty);
            if (idx != -1) {
                const QVariant value = sheet->property(idx);
                if (value.toByteArray() == objectNameU8)
                    sheet->setProperty(idx, value);
            }
        }
    }
}

// ---- ChangeZOrderCommand ----
ChangeZOrderCommand::ChangeZOrderCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QString(), formWindow)
{
}

void ChangeZOrderCommand::init(QWidget *widget)
{
    Q_ASSERT(widget);

    m_widget = widget;

    setText(QApplication::translate("Command", "Change Z-order of '%1'").arg(widget->objectName()));

    m_oldParentZOrder = qVariantValue<QWidgetList>(widget->parentWidget()->property("_q_zOrder"));
    const int index = m_oldParentZOrder.indexOf(m_widget);
    if (index != -1 && index + 1 < m_oldParentZOrder.count())
        m_oldPreceding = m_oldParentZOrder.at(index + 1);
}

void ChangeZOrderCommand::redo()
{
    m_widget->parentWidget()->setProperty("_q_zOrder", qVariantFromValue(reorderWidget(m_oldParentZOrder, m_widget)));

    reorder(m_widget);
}

void ChangeZOrderCommand::undo()
{
    m_widget->parentWidget()->setProperty("_q_zOrder", qVariantFromValue(m_oldParentZOrder));

    if (m_oldPreceding)
        m_widget->stackUnder(m_oldPreceding);
    else
        m_widget->raise();
}

// ---- RaiseWidgetCommand ----
RaiseWidgetCommand::RaiseWidgetCommand(QDesignerFormWindowInterface *formWindow)
    : ChangeZOrderCommand(formWindow)
{
}

void RaiseWidgetCommand::init(QWidget *widget)
{
    ChangeZOrderCommand::init(widget);
    setText(QApplication::translate("Command", "Raise '%1'").arg(widget->objectName()));
}

QWidgetList RaiseWidgetCommand::reorderWidget(const QWidgetList &list, QWidget *widget) const
{
    QWidgetList l = list;
    l.removeAll(widget);
    l.append(widget);
    return l;
}

void RaiseWidgetCommand::reorder(QWidget *widget) const
{
    widget->raise();
}

// ---- LowerWidgetCommand ----
LowerWidgetCommand::LowerWidgetCommand(QDesignerFormWindowInterface *formWindow)
    : ChangeZOrderCommand(formWindow)
{
}

QWidgetList LowerWidgetCommand::reorderWidget(const QWidgetList &list, QWidget *widget) const
{
    QWidgetList l = list;
    l.removeAll(widget);
    l.prepend(widget);
    return l;
}

void LowerWidgetCommand::init(QWidget *widget)
{
    ChangeZOrderCommand::init(widget);
    setText(QApplication::translate("Command", "Lower '%1'").arg(widget->objectName()));
}

void LowerWidgetCommand::reorder(QWidget *widget) const
{
    widget->lower();
}

// ---- ManageWidgetCommandHelper
ManageWidgetCommandHelper::ManageWidgetCommandHelper() :
    m_widget(0)
{
}

void ManageWidgetCommandHelper::init(const QDesignerFormWindowInterface *fw, QWidget *widget)
{
    m_widget = widget;
    m_managedChildren.clear();

    const QWidgetList children = qFindChildren<QWidget *>(m_widget);
    if (children.empty())
        return;

    m_managedChildren.reserve(children.size());
    const QWidgetList::const_iterator lcend = children.constEnd();
    for (QWidgetList::const_iterator it = children.constBegin(); it != lcend; ++it)
        if (fw->isManaged(*it))
            m_managedChildren.push_back(*it);
}

void ManageWidgetCommandHelper::init(QWidget *widget, const WidgetVector &managedChildren)
{
    m_widget = widget;
    m_managedChildren = managedChildren;
}

void ManageWidgetCommandHelper::manage(QDesignerFormWindowInterface *fw)
{
    // Manage the managed children after parent
    fw->manageWidget(m_widget);
    if (!m_managedChildren.empty()) {
        const WidgetVector::const_iterator lcend = m_managedChildren.constEnd();
        for (WidgetVector::const_iterator it = m_managedChildren.constBegin(); it != lcend; ++it)
            fw->manageWidget(*it);
    }
}

void ManageWidgetCommandHelper::unmanage(QDesignerFormWindowInterface *fw)
{
    // Unmanage the managed children first
    if (!m_managedChildren.empty()) {
        const WidgetVector::const_iterator lcend = m_managedChildren.constEnd();
        for (WidgetVector::const_iterator it = m_managedChildren.constBegin(); it != lcend; ++it)
            fw->unmanageWidget(*it);
    }
    fw->unmanageWidget(m_widget);
}

// ---- DeleteWidgetCommand ----
DeleteWidgetCommand::DeleteWidgetCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_layoutType(LayoutInfo::NoLayout),
    m_layoutHelper(0),
    m_flags(0),
    m_splitterIndex(-1),
    m_layoutSimplified(false),
    m_formItem(0),
    m_tabOrderIndex(-1),
    m_widgetOrderIndex(-1),
    m_zOrderIndex(-1)
{
}

DeleteWidgetCommand::~DeleteWidgetCommand()
{
    delete  m_layoutHelper;
}

void DeleteWidgetCommand::init(QWidget *widget, unsigned flags)
{
    m_widget = widget;
    m_parentWidget = widget->parentWidget();
    m_geometry = widget->geometry();
    m_flags = flags;
    m_layoutType = LayoutInfo::NoLayout;
    m_splitterIndex = -1;
    bool isManaged; // Check for a managed layout
    QLayout *layout;
    m_layoutType = LayoutInfo::laidoutWidgetType(formWindow()->core(), m_widget, &isManaged, &layout);
    if (!isManaged)
        m_layoutType = LayoutInfo::NoLayout;
    switch (m_layoutType) {
    case LayoutInfo::HSplitter:
    case LayoutInfo::VSplitter: {
        QSplitter *splitter = qobject_cast<QSplitter *>(m_parentWidget);
        Q_ASSERT(splitter);
        m_splitterIndex = splitter->indexOf(widget);
    }
        break;
    case LayoutInfo::NoLayout:
        break;
    default:
        m_layoutHelper = LayoutHelper::createLayoutHelper(m_layoutType);
        m_layoutPosition = m_layoutHelper->itemInfo(layout, m_widget);
        break;
    }

    m_formItem = formWindow()->core()->metaDataBase()->item(formWindow());
    m_tabOrderIndex = m_formItem->tabOrder().indexOf(widget);

    // Build the list of managed children
    m_manageHelper.init(formWindow(), m_widget);

    setText(QApplication::translate("Command", "Delete '%1'").arg(widget->objectName()));
}

void DeleteWidgetCommand::redo()
{
    formWindow()->clearSelection();
    QDesignerFormEditorInterface *core = formWindow()->core();

    if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_parentWidget)) {
        const int count = c->count();
        for (int i=0; i<count; ++i) {
            if (c->widget(i) == m_widget) {
                c->remove(i);
                return;
            }
        }
    }

    m_widgetOrderIndex = removeFromWidgetListDynamicProperty(m_parentWidget, m_widget, widgetOrderPropertyC);
    m_zOrderIndex = removeFromWidgetListDynamicProperty(m_parentWidget, m_widget, zOrderPropertyC);

    if (QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), m_parentWidget))
        deco->removeWidget(m_widget);

    if (m_layoutHelper)
        switch (m_layoutType) {
        case LayoutInfo::NoLayout:
        case LayoutInfo::HSplitter:
        case LayoutInfo::VSplitter:
            break;
        default:
            // Attempt to simplify grids if a row/column becomes empty
            m_layoutSimplified = (m_flags & DoNotSimplifyLayout) ? false : m_layoutHelper->canSimplify(core, m_parentWidget, m_layoutPosition);
            if (m_layoutSimplified) {
                m_layoutHelper->pushState(core, m_parentWidget);
                m_layoutHelper->simplify(core, m_parentWidget, m_layoutPosition);
            }
            break;
        }

    if (!(m_flags & DoNotUnmanage))
        m_manageHelper.unmanage(formWindow());

    m_widget->setParent(formWindow());
    m_widget->hide();

    if (m_tabOrderIndex != -1) {
        QList<QWidget*> tab_order = m_formItem->tabOrder();
        tab_order.removeAt(m_tabOrderIndex);
        m_formItem->setTabOrder(tab_order);
    }
}

void DeleteWidgetCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    formWindow()->clearSelection();

    m_widget->setParent(m_parentWidget);

    if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_parentWidget)) {
        c->addWidget(m_widget);
        return;
    }

    addToWidgetListDynamicProperty(m_parentWidget, m_widget, widgetOrderPropertyC, m_widgetOrderIndex);
    addToWidgetListDynamicProperty(m_parentWidget, m_widget, zOrderPropertyC, m_zOrderIndex);

    m_widget->setGeometry(m_geometry);

    if (!(m_flags & DoNotUnmanage))
        m_manageHelper.manage(formWindow());
    // ### set up alignment
    switch (m_layoutType) {
    case LayoutInfo::NoLayout:
        break;
    case LayoutInfo::HSplitter:
    case LayoutInfo::VSplitter: {
        QSplitter *splitter = qobject_cast<QSplitter *>(m_widget->parent());
        Q_ASSERT(splitter);
        splitter->insertWidget(m_splitterIndex, m_widget);
    } break;
    default: {
        Q_ASSERT(m_layoutHelper);
        if (m_layoutSimplified)
            m_layoutHelper->popState(core, m_parentWidget);
        QLayout *layout = LayoutInfo::managedLayout(core, m_parentWidget);
        Q_ASSERT(m_layoutType == LayoutInfo::layoutType(core, layout));
        m_layoutHelper->insertWidget(layout, m_layoutPosition, m_widget);
    }
        break;
    }

    m_widget->show();

    if (m_tabOrderIndex != -1) {
        QList<QWidget*> tab_order = m_formItem->tabOrder();
        tab_order.insert(m_tabOrderIndex, m_widget);
        m_formItem->setTabOrder(tab_order);
    }
}

// ---- ReparentWidgetCommand ----
ReparentWidgetCommand::ReparentWidgetCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QString(), formWindow)
{
}

void ReparentWidgetCommand::init(QWidget *widget, QWidget *parentWidget)
{
    Q_ASSERT(widget);

    m_widget = widget;
    m_oldParentWidget = widget->parentWidget();
    m_newParentWidget = parentWidget;

    m_oldPos = m_widget->pos();
    m_newPos = m_newParentWidget->mapFromGlobal(m_oldParentWidget->mapToGlobal(m_oldPos));

    setText(QApplication::translate("Command", "Reparent '%1'").arg(widget->objectName()));

    m_oldParentList = qVariantValue<QWidgetList>(m_oldParentWidget->property("_q_widgetOrder"));
    m_oldParentZOrder = qVariantValue<QWidgetList>(m_oldParentWidget->property("_q_zOrder"));
}

void ReparentWidgetCommand::redo()
{
    m_widget->setParent(m_newParentWidget);
    m_widget->move(m_newPos);

    QWidgetList oldList = m_oldParentList;
    oldList.removeAll(m_widget);
    m_oldParentWidget->setProperty("_q_widgetOrder", qVariantFromValue(oldList));

    QWidgetList newList = qVariantValue<QWidgetList>(m_newParentWidget->property("_q_widgetOrder"));
    newList.append(m_widget);
    m_newParentWidget->setProperty("_q_widgetOrder", qVariantFromValue(newList));

    QWidgetList oldZOrder = m_oldParentZOrder;
    oldZOrder.removeAll(m_widget);
    m_oldParentWidget->setProperty("_q_zOrder", qVariantFromValue(oldZOrder));

    QWidgetList newZOrder = qVariantValue<QWidgetList>(m_newParentWidget->property("_q_zOrder"));
    newZOrder.append(m_widget);
    m_newParentWidget->setProperty("_q_zOrder", qVariantFromValue(newZOrder));

    m_widget->show();
    core()->objectInspector()->setFormWindow(formWindow());
}

void ReparentWidgetCommand::undo()
{
    m_widget->setParent(m_oldParentWidget);
    m_widget->move(m_oldPos);

    m_oldParentWidget->setProperty("_q_widgetOrder", qVariantFromValue(m_oldParentList));

    QWidgetList newList = qVariantValue<QWidgetList>(m_newParentWidget->property("_q_widgetOrder"));
    newList.removeAll(m_widget);
    m_newParentWidget->setProperty("_q_widgetOrder", qVariantFromValue(newList));

    m_oldParentWidget->setProperty("_q_zOrder", qVariantFromValue(m_oldParentZOrder));

    QWidgetList newZOrder = qVariantValue<QWidgetList>(m_newParentWidget->property("_q_zOrder"));
    m_newParentWidget->setProperty("_q_zOrder", qVariantFromValue(newZOrder));

    m_widget->show();
    core()->objectInspector()->setFormWindow(formWindow());
}

PromoteToCustomWidgetCommand::PromoteToCustomWidgetCommand
                                (QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Promote to custom widget"), formWindow)
{
}

void PromoteToCustomWidgetCommand::init(const WidgetList &widgets,const QString &customClassName)
{
    m_widgets = widgets;
    m_customClassName = customClassName;
}

void PromoteToCustomWidgetCommand::redo()
{
    foreach (QWidget *w, m_widgets) {
        if (w)
            promoteWidget(core(), w, m_customClassName);
    }
    updateSelection();
}

void PromoteToCustomWidgetCommand::updateSelection()
{
    // Update class names in ObjectInspector, PropertyEditor
    QDesignerFormWindowInterface *fw = formWindow();
    QDesignerFormEditorInterface *core = fw->core();
    core->objectInspector()->setFormWindow(fw);
    if (QObject *o = core->propertyEditor()->object())
        core->propertyEditor()->setObject(o);
}

void PromoteToCustomWidgetCommand::undo()
{
    foreach (QWidget *w, m_widgets) {
        if (w)
            demoteWidget(core(), w);
    }
    updateSelection();
}

// ---- DemoteFromCustomWidgetCommand ----

DemoteFromCustomWidgetCommand::DemoteFromCustomWidgetCommand
                                    (QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Demote from custom widget"), formWindow),
    m_promote_cmd(formWindow)
{
}

void DemoteFromCustomWidgetCommand::init(const WidgetList &promoted)
{
    m_promote_cmd.init(promoted, promotedCustomClassName(core(), promoted.front()));
}

void DemoteFromCustomWidgetCommand::redo()
{
    m_promote_cmd.undo();
}

void DemoteFromCustomWidgetCommand::undo()
{
    m_promote_cmd.redo();
}

// ----------  CursorSelectionState
CursorSelectionState::CursorSelectionState()
{
}

void CursorSelectionState::save(const QDesignerFormWindowInterface *formWindow)
{
    const QDesignerFormWindowCursorInterface *cursor = formWindow->cursor();
    m_selection.clear();
    m_current = cursor->current();
    if (cursor->hasSelection()) {
        const int count = cursor->selectedWidgetCount();
        for(int i = 0; i < count; i++)
            m_selection.push_back(cursor->selectedWidget(i));
    }
}

void CursorSelectionState::restore(QDesignerFormWindowInterface *formWindow) const
{
    if (m_selection.empty()) {
        formWindow->clearSelection(true);
    } else {
        // Select current as last
        formWindow->clearSelection(false);
        const WidgetPointerList::const_iterator cend = m_selection.constEnd();
        for (WidgetPointerList::const_iterator it = m_selection.constBegin(); it != cend; ++it)
            if (QWidget *w = *it)
                if (w != m_current)
                    formWindow->selectWidget(*it, true);
        if (m_current)
            formWindow->selectWidget(m_current, true);
    }
}

// ---- LayoutCommand ----

LayoutCommand::LayoutCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_setup(false)
{
}

LayoutCommand::~LayoutCommand()
{
    delete m_layout;
}

void LayoutCommand::init(QWidget *parentWidget, const QWidgetList &widgets,
                         LayoutInfo::Type layoutType, QWidget *layoutBase,
                         bool reparentLayoutWidget)
{
    m_parentWidget = parentWidget;
    m_widgets = widgets;
    formWindow()->simplifySelection(&m_widgets);
    m_layout = Layout::createLayout(widgets, parentWidget, formWindow(), layoutBase, layoutType);
    m_layout->setReparentLayoutWidget(reparentLayoutWidget);

    switch (layoutType) {
    case LayoutInfo::Grid:
        setText(QApplication::translate("Command", "Lay out using grid"));
        break;
    case LayoutInfo::VBox:
        setText(QApplication::translate("Command", "Lay out vertically"));
        break;
    case LayoutInfo::HBox:
        setText(QApplication::translate("Command", "Lay out horizontally"));
        break;
    default:
        break;
    }
    // Delayed setup to avoid confusion in case we are chained
    // with a BreakLayout in a morph layout macro
    m_setup = false;
}

void LayoutCommand::redo()
{
    if (!m_setup) {
        m_layout->setup();
        m_cursorSelectionState.save(formWindow());
        m_setup = true;
    }
    m_layout->doLayout();
    core()->objectInspector()->setFormWindow(formWindow());
}

void LayoutCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();

    QWidget *lb = m_layout->layoutBaseWidget();
    QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), lb);
    m_layout->undoLayout();
    delete deco; // release the extension

    // ### generalize (put in function)
    if (!m_layoutBase && lb != 0 && !(qobject_cast<QLayoutWidget*>(lb) || qobject_cast<QSplitter*>(lb))) {
        core->metaDataBase()->add(lb);
        lb->show();
    }
    m_cursorSelectionState.restore(formWindow());
    core->objectInspector()->setFormWindow(formWindow());
}

// ---- BreakLayoutCommand ----
BreakLayoutCommand::BreakLayoutCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Break layout"), formWindow),
    m_layoutHelper(0),
    m_properties(0),
    m_propertyMask(0)
{
}

BreakLayoutCommand::~BreakLayoutCommand()
{
    delete m_layoutHelper;
    delete m_layout;
    delete m_properties;
}

const LayoutProperties *BreakLayoutCommand::layoutProperties() const
{
    return m_properties;
}

int BreakLayoutCommand::propertyMask() const
{
    return m_propertyMask;
}

void BreakLayoutCommand::init(const QWidgetList &widgets, QWidget *layoutBase, bool reparentLayoutWidget)
{
    enum Type { SplitterLayout, LayoutHasMarginSpacing, LayoutHasState };

    const QDesignerFormEditorInterface *core = formWindow()->core();
    m_widgets = widgets;
    m_layoutBase = core->widgetFactory()->containerOfWidget(layoutBase);
    QLayout *layoutToBeBroken;
    const LayoutInfo::Type layoutType = LayoutInfo::managedLayoutType(core, m_layoutBase, &layoutToBeBroken);
    m_layout = Layout::createLayout(widgets, m_layoutBase, formWindow(), layoutBase, layoutType);
    m_layout->setReparentLayoutWidget(reparentLayoutWidget);

    Type type = LayoutHasState;
    switch (layoutType) {
    case LayoutInfo::NoLayout:
    case LayoutInfo::HSplitter:
    case LayoutInfo::VSplitter:
        type = SplitterLayout;
        break;
    case LayoutInfo::HBox:
    case LayoutInfo::VBox: // Margin/spacing need to be saved
        type = LayoutHasMarginSpacing;
        break;
    default: // Margin/spacing need to be saved + has a state (empty rows/columns of a grid)
        type = LayoutHasState;
        break;
    }
    Q_ASSERT(m_layout != 0);
    m_layout->sort();


    if (type >= LayoutHasMarginSpacing) {
        m_properties = new LayoutProperties;
        m_propertyMask = m_properties->fromPropertySheet(core, layoutToBeBroken, LayoutProperties::AllProperties);
    }
    if (type >= LayoutHasState)
         m_layoutHelper = LayoutHelper::createLayoutHelper(layoutType);
    m_cursorSelectionState.save(formWindow());
}

void BreakLayoutCommand::redo()
{
    if (!m_layout)
        return;

    QDesignerFormEditorInterface *core = formWindow()->core();
    QWidget *lb = m_layout->layoutBaseWidget();
    QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), lb);
    formWindow()->clearSelection(false);
    if (m_layoutHelper)
        m_layoutHelper->pushState(core, m_layoutBase);
    m_layout->breakLayout();
    delete deco; // release the extension

    foreach (QWidget *widget, m_widgets) {
        widget->resize(widget->size().expandedTo(QSize(16, 16)));
    }
    // Update unless we are in an intermediate state of morphing layout
    // in which a QLayoutWidget will have no layout at all.
    if (m_layout->reparentLayoutWidget())
        core->objectInspector()->setFormWindow(formWindow());
}

void BreakLayoutCommand::undo()
{
    if (!m_layout)
        return;

    formWindow()->clearSelection(false);
    m_layout->doLayout();
    if (m_layoutHelper)
        m_layoutHelper->popState(formWindow()->core(), m_layoutBase);

    QLayout *layoutToRestored = LayoutInfo::managedLayout(formWindow()->core(), m_layoutBase);
    if (m_properties && m_layoutBase && layoutToRestored)
        m_properties->toPropertySheet(formWindow()->core(), layoutToRestored, m_propertyMask);
     m_cursorSelectionState.restore(formWindow());
    core()->objectInspector()->setFormWindow(formWindow());
}
// ---- SimplifyLayoutCommand
SimplifyLayoutCommand::SimplifyLayoutCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Simplify Grid Layout"), formWindow),
    m_area(0, 0, 32767, 32767),
    m_layoutBase(0),
    m_layoutHelper(0),
    m_layoutSimplified(false)
{
}

SimplifyLayoutCommand::~SimplifyLayoutCommand()
{
    delete m_layoutHelper;
}

bool SimplifyLayoutCommand::canSimplify(QDesignerFormEditorInterface *core, const QWidget *w, int *layoutType)
{
    if (!w)
        return false;
    QLayout *layout;
    const LayoutInfo::Type type = LayoutInfo::managedLayoutType(core, w, &layout);
    if (layoutType)
        *layoutType = type;
    if (!layout)
        return false;
    switch (type) { // Known negatives
    case LayoutInfo::NoLayout:
    case LayoutInfo::UnknownLayout:
    case LayoutInfo::HSplitter:
    case LayoutInfo::VSplitter:
    case LayoutInfo::HBox:
    case LayoutInfo::VBox:
        return false;
    default:
        break;
    }
    switch (type) {
    case LayoutInfo::Grid:
        return QLayoutSupport::canSimplifyQuickCheck(qobject_cast<QGridLayout*>(layout));
    case LayoutInfo::Form:
        return QLayoutSupport::canSimplifyQuickCheck(qobject_cast<const QFormLayout*>(layout));
    default:
        break;
    }
    return false;
}

bool SimplifyLayoutCommand::init(QWidget *layoutBase)
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    m_layoutSimplified = false;
    int type;
    if (canSimplify(core, layoutBase, &type)) {
        m_layoutBase = layoutBase;
        m_layoutHelper = LayoutHelper::createLayoutHelper(type);
        m_layoutSimplified = m_layoutHelper->canSimplify(core, layoutBase, m_area);
    }
    return m_layoutSimplified;
}

void SimplifyLayoutCommand::redo()
{
    const QDesignerFormEditorInterface *core = formWindow()->core();
    if (m_layoutSimplified) {
        m_layoutHelper->pushState(core, m_layoutBase);
        m_layoutHelper->simplify(core, m_layoutBase, m_area);
    }
}
void SimplifyLayoutCommand::undo()
{
    if (m_layoutSimplified)
         m_layoutHelper->popState(formWindow()->core(), m_layoutBase);
}

// ---- ToolBoxCommand ----
ToolBoxCommand::ToolBoxCommand(QDesignerFormWindowInterface *formWindow)  :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_index(-1)
{
}

ToolBoxCommand::~ToolBoxCommand()
{
}

void ToolBoxCommand::init(QToolBox *toolBox)
{
    m_toolBox = toolBox;
    m_index = m_toolBox->currentIndex();
    m_widget = m_toolBox->widget(m_index);
    m_itemText = m_toolBox->itemText(m_index);
    m_itemIcon = m_toolBox->itemIcon(m_index);
}

void ToolBoxCommand::removePage()
{
    m_toolBox->removeItem(m_index);

    m_widget->hide();
    m_widget->setParent(formWindow());
    formWindow()->clearSelection();
    formWindow()->selectWidget(m_toolBox, true);

}

void ToolBoxCommand::addPage()
{
    m_widget->setParent(m_toolBox);
    m_toolBox->insertItem(m_index, m_widget, m_itemIcon, m_itemText);
    m_toolBox->setCurrentIndex(m_index);

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(formWindow()->core()->extensionManager(), m_toolBox);
    if (sheet) {
        qdesigner_internal::PropertySheetStringValue itemText(m_itemText);
        sheet->setProperty(sheet->indexOf(QLatin1String("currentItemText")), qVariantFromValue(itemText));
    }

    m_widget->show();
    formWindow()->clearSelection();
    formWindow()->selectWidget(m_toolBox, true);
}

// ---- MoveToolBoxPageCommand ----
MoveToolBoxPageCommand::MoveToolBoxPageCommand(QDesignerFormWindowInterface *formWindow) :
    ToolBoxCommand(formWindow),
    m_newIndex(-1),
    m_oldIndex(-1)
{
}

MoveToolBoxPageCommand::~MoveToolBoxPageCommand()
{
}

void MoveToolBoxPageCommand::init(QToolBox *toolBox, QWidget *page, int newIndex)
{
    ToolBoxCommand::init(toolBox);
    setText(QApplication::translate("Command", "Move Page"));

    m_widget = page;
    m_oldIndex = m_toolBox->indexOf(m_widget);
    m_itemText = m_toolBox->itemText(m_oldIndex);
    m_itemIcon = m_toolBox->itemIcon(m_oldIndex);
    m_newIndex = newIndex;
}

void MoveToolBoxPageCommand::redo()
{
    m_toolBox->removeItem(m_oldIndex);
    m_toolBox->insertItem(m_newIndex, m_widget, m_itemIcon, m_itemText);
}

void MoveToolBoxPageCommand::undo()
{
    m_toolBox->removeItem(m_newIndex);
    m_toolBox->insertItem(m_oldIndex, m_widget, m_itemIcon, m_itemText);
}

// ---- DeleteToolBoxPageCommand ----
DeleteToolBoxPageCommand::DeleteToolBoxPageCommand(QDesignerFormWindowInterface *formWindow)
    : ToolBoxCommand(formWindow)
{
}

DeleteToolBoxPageCommand::~DeleteToolBoxPageCommand()
{
}

void DeleteToolBoxPageCommand::init(QToolBox *toolBox)
{
    ToolBoxCommand::init(toolBox);
    setText(QApplication::translate("Command", "Delete Page"));
}

void DeleteToolBoxPageCommand::redo()
{
    removePage();
    cheapUpdate();
}

void DeleteToolBoxPageCommand::undo()
{
    addPage();
    cheapUpdate();
}

// ---- AddToolBoxPageCommand ----
AddToolBoxPageCommand::AddToolBoxPageCommand(QDesignerFormWindowInterface *formWindow)
    : ToolBoxCommand(formWindow)
{
}

AddToolBoxPageCommand::~AddToolBoxPageCommand()
{
}

void AddToolBoxPageCommand::init(QToolBox *toolBox)
{
    init(toolBox, InsertBefore);
}

void AddToolBoxPageCommand::init(QToolBox *toolBox, InsertionMode mode)
{
    m_toolBox = toolBox;

    m_index = m_toolBox->currentIndex();
    if (mode == InsertAfter)
        m_index++;
    m_widget = new QDesignerWidget(formWindow(), m_toolBox);
    m_itemText = QApplication::translate("Command", "Page");
    m_itemIcon = QIcon();
    m_widget->setObjectName(QLatin1String("page"));
    formWindow()->ensureUniqueObjectName(m_widget);

    setText(QApplication::translate("Command", "Insert Page"));

    QDesignerFormEditorInterface *core = formWindow()->core();
    core->metaDataBase()->add(m_widget);
}

void AddToolBoxPageCommand::redo()
{
    addPage();
    cheapUpdate();
}

void AddToolBoxPageCommand::undo()
{
    removePage();
    cheapUpdate();
}

// ---- TabWidgetCommand ----
TabWidgetCommand::TabWidgetCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_index(-1)
{
}

TabWidgetCommand::~TabWidgetCommand()
{
}

void TabWidgetCommand::init(QTabWidget *tabWidget)
{
    m_tabWidget = tabWidget;
    m_index = m_tabWidget->currentIndex();
    m_widget = m_tabWidget->widget(m_index);
    m_itemText = m_tabWidget->tabText(m_index);
    m_itemIcon = m_tabWidget->tabIcon(m_index);
}

void TabWidgetCommand::removePage()
{
    m_tabWidget->removeTab(m_index);

    m_widget->hide();
    m_widget->setParent(formWindow());
    m_tabWidget->setCurrentIndex(qMin(m_index, m_tabWidget->count()));

    formWindow()->clearSelection();
    formWindow()->selectWidget(m_tabWidget, true);
}

void TabWidgetCommand::addPage()
{
    m_widget->setParent(0);
    m_tabWidget->insertTab(m_index, m_widget, m_itemIcon, m_itemText);
    m_widget->show();
    m_tabWidget->setCurrentIndex(m_index);

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(formWindow()->core()->extensionManager(), m_tabWidget);
    if (sheet) {
        qdesigner_internal::PropertySheetStringValue itemText(m_itemText);
        sheet->setProperty(sheet->indexOf(QLatin1String("currentTabText")), qVariantFromValue(itemText));
    }

    formWindow()->clearSelection();
    formWindow()->selectWidget(m_tabWidget, true);
}

// ---- DeleteTabPageCommand ----
DeleteTabPageCommand::DeleteTabPageCommand(QDesignerFormWindowInterface *formWindow)
    : TabWidgetCommand(formWindow)
{
}

DeleteTabPageCommand::~DeleteTabPageCommand()
{
}

void DeleteTabPageCommand::init(QTabWidget *tabWidget)
{
    TabWidgetCommand::init(tabWidget);
    setText(QApplication::translate("Command", "Delete Page"));
}

void DeleteTabPageCommand::redo()
{
    removePage();
    cheapUpdate();
}

void DeleteTabPageCommand::undo()
{
    addPage();
    cheapUpdate();
}

// ---- AddTabPageCommand ----
AddTabPageCommand::AddTabPageCommand(QDesignerFormWindowInterface *formWindow)
    : TabWidgetCommand(formWindow)
{
}

AddTabPageCommand::~AddTabPageCommand()
{
}

void AddTabPageCommand::init(QTabWidget *tabWidget)
{
    init(tabWidget, InsertBefore);
}

void AddTabPageCommand::init(QTabWidget *tabWidget, InsertionMode mode)
{
    m_tabWidget = tabWidget;

    m_index = m_tabWidget->currentIndex();
    if (mode == InsertAfter)
        m_index++;
    m_widget = new QDesignerWidget(formWindow(), m_tabWidget);
    m_itemText = QApplication::translate("Command", "Page");
    m_itemIcon = QIcon();
    m_widget->setObjectName(QLatin1String("tab"));
    formWindow()->ensureUniqueObjectName(m_widget);

    setText(QApplication::translate("Command", "Insert Page"));

    QDesignerFormEditorInterface *core = formWindow()->core();
    core->metaDataBase()->add(m_widget);
}

void AddTabPageCommand::redo()
{
    addPage();
    cheapUpdate();
}

void AddTabPageCommand::undo()
{
    removePage();
    cheapUpdate();
}

// ---- MoveTabPageCommand ----
MoveTabPageCommand::MoveTabPageCommand(QDesignerFormWindowInterface *formWindow) :
    TabWidgetCommand(formWindow),
    m_newIndex(-1),
    m_oldIndex(-1)
{
}

MoveTabPageCommand::~MoveTabPageCommand()
{
}

void MoveTabPageCommand::init(QTabWidget *tabWidget, QWidget *page,
                      const QIcon &icon, const QString &label,
                      int index, int newIndex)
{
    TabWidgetCommand::init(tabWidget);
    setText(QApplication::translate("Command", "Move Page"));

    m_page = page;
    m_newIndex = newIndex;
    m_oldIndex = index;
    m_label = label;
    m_icon = icon;
}

void MoveTabPageCommand::redo()
{
    m_tabWidget->removeTab(m_oldIndex);
    m_tabWidget->insertTab(m_newIndex, m_page, m_icon, m_label);
    m_tabWidget->setCurrentIndex(m_newIndex);
}

void MoveTabPageCommand::undo()
{
    m_tabWidget->removeTab(m_newIndex);
    m_tabWidget->insertTab(m_oldIndex, m_page, m_icon, m_label);
    m_tabWidget->setCurrentIndex(m_oldIndex);
}

// ---- StackedWidgetCommand ----
StackedWidgetCommand::StackedWidgetCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_index(-1)
{
}

StackedWidgetCommand::~StackedWidgetCommand()
{
}

void StackedWidgetCommand::init(QStackedWidget *stackedWidget)
{
    m_stackedWidget = stackedWidget;
    m_index = m_stackedWidget->currentIndex();
    m_widget = m_stackedWidget->widget(m_index);
}

void StackedWidgetCommand::removePage()
{
    m_stackedWidget->removeWidget(m_stackedWidget->widget(m_index));

    m_widget->hide();
    m_widget->setParent(formWindow());

    formWindow()->clearSelection();
    formWindow()->selectWidget(m_stackedWidget, true);
}

void StackedWidgetCommand::addPage()
{
    m_stackedWidget->insertWidget(m_index, m_widget);

    m_widget->show();
    m_stackedWidget->setCurrentIndex(m_index);

    formWindow()->clearSelection();
    formWindow()->selectWidget(m_stackedWidget, true);
}

// ---- MoveStackedWidgetCommand ----
MoveStackedWidgetCommand::MoveStackedWidgetCommand(QDesignerFormWindowInterface *formWindow) :
    StackedWidgetCommand(formWindow),
    m_newIndex(-1),
    m_oldIndex(-1)
{
}

MoveStackedWidgetCommand::~MoveStackedWidgetCommand()
{
}

void MoveStackedWidgetCommand::init(QStackedWidget *stackedWidget, QWidget *page, int newIndex)
{
    StackedWidgetCommand::init(stackedWidget);
    setText(QApplication::translate("Command", "Move Page"));

    m_widget = page;
    m_newIndex = newIndex;
    m_oldIndex = m_stackedWidget->indexOf(m_widget);
}

void MoveStackedWidgetCommand::redo()
{
    m_stackedWidget->removeWidget(m_widget);
    m_stackedWidget->insertWidget(m_newIndex, m_widget);
}

void MoveStackedWidgetCommand::undo()
{
    m_stackedWidget->removeWidget(m_widget);
    m_stackedWidget->insertWidget(m_oldIndex, m_widget);
}

// ---- DeleteStackedWidgetPageCommand ----
DeleteStackedWidgetPageCommand::DeleteStackedWidgetPageCommand(QDesignerFormWindowInterface *formWindow)
    : StackedWidgetCommand(formWindow)
{
}

DeleteStackedWidgetPageCommand::~DeleteStackedWidgetPageCommand()
{
}

void DeleteStackedWidgetPageCommand::init(QStackedWidget *stackedWidget)
{
    StackedWidgetCommand::init(stackedWidget);
    setText(QApplication::translate("Command", "Delete Page"));
}

void DeleteStackedWidgetPageCommand::redo()
{
    removePage();
    cheapUpdate();
}

void DeleteStackedWidgetPageCommand::undo()
{
    addPage();
    cheapUpdate();
}

// ---- AddStackedWidgetPageCommand ----
AddStackedWidgetPageCommand::AddStackedWidgetPageCommand(QDesignerFormWindowInterface *formWindow)
    : StackedWidgetCommand(formWindow)
{
}

AddStackedWidgetPageCommand::~AddStackedWidgetPageCommand()
{
}

void AddStackedWidgetPageCommand::init(QStackedWidget *stackedWidget)
{
    init(stackedWidget, InsertBefore);
}

void AddStackedWidgetPageCommand::init(QStackedWidget *stackedWidget, InsertionMode mode)
{
    m_stackedWidget = stackedWidget;

    m_index = m_stackedWidget->currentIndex();
    if (mode == InsertAfter)
        m_index++;
    m_widget = new QDesignerWidget(formWindow(), m_stackedWidget);
    m_widget->setObjectName(QLatin1String("page"));
    formWindow()->ensureUniqueObjectName(m_widget);

    setText(QApplication::translate("Command", "Insert Page"));

    QDesignerFormEditorInterface *core = formWindow()->core();
    core->metaDataBase()->add(m_widget);
}

void AddStackedWidgetPageCommand::redo()
{
    addPage();
    cheapUpdate();
}

void AddStackedWidgetPageCommand::undo()
{
    removePage();
    cheapUpdate();
}

// ---- TabOrderCommand ----
TabOrderCommand::TabOrderCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Change Tab order"), formWindow),
      m_widgetItem(0)
{
}

void TabOrderCommand::init(const QList<QWidget*> &newTabOrder)
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    Q_ASSERT(core);

    m_widgetItem = core->metaDataBase()->item(formWindow());
    Q_ASSERT(m_widgetItem);
    m_oldTabOrder = m_widgetItem->tabOrder();
    m_newTabOrder = newTabOrder;
}

void TabOrderCommand::redo()
{
    m_widgetItem->setTabOrder(m_newTabOrder);
}

void TabOrderCommand::undo()
{
    m_widgetItem->setTabOrder(m_oldTabOrder);
}

// ---- CreateMenuBarCommand ----
CreateMenuBarCommand::CreateMenuBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Create Menu Bar"), formWindow)
{
}

void CreateMenuBarCommand::init(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    QDesignerFormEditorInterface *core = formWindow()->core();
    m_menuBar = qobject_cast<QMenuBar*>(core->widgetFactory()->createWidget(QLatin1String("QMenuBar"), m_mainWindow));
    core->widgetFactory()->initialize(m_menuBar);
}

void CreateMenuBarCommand::redo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c;
    c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    c->addWidget(m_menuBar);

    m_menuBar->setObjectName(QLatin1String("menuBar"));
    formWindow()->ensureUniqueObjectName(m_menuBar);
    core->metaDataBase()->add(m_menuBar);
    formWindow()->emitSelectionChanged();
    m_menuBar->setFocus();
}

void CreateMenuBarCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c;
    c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    for (int i = 0; i < c->count(); ++i) {
        if (c->widget(i) == m_menuBar) {
            c->remove(i);
            break;
        }
    }

    core->metaDataBase()->remove(m_menuBar);
    formWindow()->emitSelectionChanged();
}

// ---- DeleteMenuBarCommand ----
DeleteMenuBarCommand::DeleteMenuBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Delete Menu Bar"), formWindow)
{
}

void DeleteMenuBarCommand::init(QMenuBar *menuBar)
{
    m_menuBar = menuBar;
    m_mainWindow = qobject_cast<QMainWindow*>(menuBar->parentWidget());
}

void DeleteMenuBarCommand::redo()
{
    if (m_mainWindow) {
        QDesignerContainerExtension *c;
        c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);
        Q_ASSERT(c != 0);
        for (int i=0; i<c->count(); ++i) {
            if (c->widget(i) == m_menuBar) {
                c->remove(i);
                break;
            }
        }
    }

    core()->metaDataBase()->remove(m_menuBar);
    m_menuBar->hide();
    m_menuBar->setParent(formWindow());
    formWindow()->emitSelectionChanged();
}

void DeleteMenuBarCommand::undo()
{
    if (m_mainWindow) {
        m_menuBar->setParent(m_mainWindow);
        QDesignerContainerExtension *c;
        c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);

        c->addWidget(m_menuBar);

        core()->metaDataBase()->add(m_menuBar);
        m_menuBar->show();
        formWindow()->emitSelectionChanged();
    }
}

// ---- CreateStatusBarCommand ----
CreateStatusBarCommand::CreateStatusBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Create Status Bar"), formWindow)
{
}

void CreateStatusBarCommand::init(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    QDesignerFormEditorInterface *core = formWindow()->core();
    m_statusBar = qobject_cast<QStatusBar*>(core->widgetFactory()->createWidget(QLatin1String("QStatusBar"), m_mainWindow));
    core->widgetFactory()->initialize(m_statusBar);
}

void CreateStatusBarCommand::redo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c;
    c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    c->addWidget(m_statusBar);

    m_statusBar->setObjectName(QLatin1String("statusBar"));
    formWindow()->ensureUniqueObjectName(m_statusBar);
    core->metaDataBase()->add(m_statusBar);
    formWindow()->emitSelectionChanged();
}

void CreateStatusBarCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    for (int i = 0; i < c->count(); ++i) {
        if (c->widget(i) == m_statusBar) {
            c->remove(i);
            break;
        }
    }

    core->metaDataBase()->remove(m_statusBar);
    formWindow()->emitSelectionChanged();
}

// ---- DeleteStatusBarCommand ----
DeleteStatusBarCommand::DeleteStatusBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Delete Status Bar"), formWindow)
{
}

void DeleteStatusBarCommand::init(QStatusBar *statusBar)
{
    m_statusBar = statusBar;
    m_mainWindow = qobject_cast<QMainWindow*>(statusBar->parentWidget());
}

void DeleteStatusBarCommand::redo()
{
    if (m_mainWindow) {
        QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);
        Q_ASSERT(c != 0);
        for (int i=0; i<c->count(); ++i) {
            if (c->widget(i) == m_statusBar) {
                c->remove(i);
                break;
            }
        }
    }

    core()->metaDataBase()->remove(m_statusBar);
    m_statusBar->hide();
    m_statusBar->setParent(formWindow());
    formWindow()->emitSelectionChanged();
}

void DeleteStatusBarCommand::undo()
{
    if (m_mainWindow) {
        m_statusBar->setParent(m_mainWindow);
        QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);

        c->addWidget(m_statusBar);

        core()->metaDataBase()->add(m_statusBar);
        m_statusBar->show();
        formWindow()->emitSelectionChanged();
    }
}

// ---- AddToolBarCommand ----
AddToolBarCommand::AddToolBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Add Tool Bar"), formWindow)
{
}

void AddToolBarCommand::init(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    QDesignerWidgetFactoryInterface * wf =  formWindow()->core()->widgetFactory();
    // Pass on 0 parent first to avoid reparenting flicker.
    m_toolBar = qobject_cast<QToolBar*>(wf->createWidget(QLatin1String("QToolBar"), 0));
    wf->initialize(m_toolBar);
    m_toolBar->hide();
}

void AddToolBarCommand::redo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    core->metaDataBase()->add(m_toolBar);

    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    c->addWidget(m_toolBar);

    m_toolBar->setObjectName(QLatin1String("toolBar"));
    formWindow()->ensureUniqueObjectName(m_toolBar);
    setPropertySheetWindowTitle(core, m_toolBar, m_toolBar->objectName());
    formWindow()->emitSelectionChanged();
}

void AddToolBarCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    core->metaDataBase()->remove(m_toolBar);
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    for (int i = 0; i < c->count(); ++i) {
        if (c->widget(i) == m_toolBar) {
            c->remove(i);
            break;
        }
    }
    formWindow()->emitSelectionChanged();
}

// ---- DockWidgetCommand:: ----
DockWidgetCommand::DockWidgetCommand(const QString &description, QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(description, formWindow)
{
}

DockWidgetCommand::~DockWidgetCommand()
{
}

void DockWidgetCommand::init(QDockWidget *dockWidget)
{
    m_dockWidget = dockWidget;
}

// ---- AddDockWidgetCommand ----
AddDockWidgetCommand::AddDockWidgetCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Add Dock Window"), formWindow)
{
}

void AddDockWidgetCommand::init(QMainWindow *mainWindow, QDockWidget *dockWidget)
{
    m_mainWindow = mainWindow;
    m_dockWidget = dockWidget;
}

void AddDockWidgetCommand::init(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    QDesignerFormEditorInterface *core = formWindow()->core();
    m_dockWidget = qobject_cast<QDockWidget*>(core->widgetFactory()->createWidget(QLatin1String("QDockWidget"), m_mainWindow));
}

void AddDockWidgetCommand::redo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    c->addWidget(m_dockWidget);

    m_dockWidget->setObjectName(QLatin1String("dockWidget"));
    formWindow()->ensureUniqueObjectName(m_dockWidget);
    formWindow()->manageWidget(m_dockWidget);
    formWindow()->emitSelectionChanged();
}

void AddDockWidgetCommand::undo()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    for (int i = 0; i < c->count(); ++i) {
        if (c->widget(i) == m_dockWidget) {
            c->remove(i);
            break;
        }
    }

    formWindow()->unmanageWidget(m_dockWidget);
    formWindow()->emitSelectionChanged();
}

// ---- AdjustWidgetSizeCommand ----
AdjustWidgetSizeCommand::AdjustWidgetSizeCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QString(), formWindow)
{
}

void AdjustWidgetSizeCommand::init(QWidget *widget)
{
    m_widget = widget;
    setText(QApplication::translate("Command", "Adjust Size of '%1'").arg(widget->objectName()));
}

QWidget *AdjustWidgetSizeCommand::widgetForAdjust() const
{
    QDesignerFormWindowInterface *fw = formWindow();
    // Return the outer, embedding widget if it is the main container
    if (Utils::isCentralWidget(fw, m_widget))
        return fw->core()->integration()->containerWindow(m_widget);
    return m_widget;
}

void AdjustWidgetSizeCommand::redo()
{
    QWidget *aw = widgetForAdjust();
    m_geometry = aw->geometry();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    aw->adjustSize();
    const bool isMainContainer = aw != m_widget;
    if (!isMainContainer) {
        /* When doing adjustsize on a selected non-laid out child that has been enlarged
         * and pushed partially over the top/left edge[s], it is possible that it "disappears"
         * when shrinking. In that case, move it back so that it remains visible. */
        if (aw->parentWidget()->layout() == 0) {
            const QRect contentsRect = aw->parentWidget()->contentsRect();
            const QRect newGeometry = aw->geometry();
            QPoint newPos = m_geometry.topLeft();
            if (newGeometry.bottom() <= contentsRect.y())
                newPos.setY(contentsRect.y());
            if (newGeometry.right() <= contentsRect.x())
                newPos.setX(contentsRect.x());
            if (newPos != m_geometry.topLeft())
                aw->move(newPos);
        }
    }
    updatePropertyEditor();
}

void AdjustWidgetSizeCommand::undo()
{
    QWidget *aw = widgetForAdjust();
    aw->resize(m_geometry.size());
    if (m_geometry.topLeft() != aw->geometry().topLeft())
        aw->move(m_geometry.topLeft());
    updatePropertyEditor();
}

void AdjustWidgetSizeCommand::updatePropertyEditor() const
{
    if (QDesignerPropertyEditorInterface *propertyEditor = formWindow()->core()->propertyEditor()) {
        if (propertyEditor->object() == m_widget)
            propertyEditor->setPropertyValue(QLatin1String("geometry"), m_widget->geometry(), true);
    }
}
// ------------  ChangeFormLayoutItemRoleCommand

ChangeFormLayoutItemRoleCommand::ChangeFormLayoutItemRoleCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Change Form Layout Item Geometry"), formWindow),
    m_operation(SpanningToLabel)
{
}

void ChangeFormLayoutItemRoleCommand::init(QWidget *widget, Operation op)
{
    m_widget = widget;
    m_operation = op;
}

void ChangeFormLayoutItemRoleCommand::redo()
{
    doOperation(m_operation);
}

void ChangeFormLayoutItemRoleCommand::undo()
{
    doOperation(reverseOperation(m_operation));
}

ChangeFormLayoutItemRoleCommand::Operation ChangeFormLayoutItemRoleCommand::reverseOperation(Operation op)
{
    switch (op) {
    case SpanningToLabel:
        return LabelToSpanning;
    case SpanningToField:
        return FieldToSpanning;
    case LabelToSpanning:
        return SpanningToLabel;
    case FieldToSpanning:
        return SpanningToField;
    }
    return SpanningToField;
}

void ChangeFormLayoutItemRoleCommand::doOperation(Operation op)
{
    QFormLayout *fl = ChangeFormLayoutItemRoleCommand::managedFormLayoutOf(formWindow()->core(), m_widget);
    const int index = fl->indexOf(m_widget);
    Q_ASSERT(index != -1);
     int row;
    QFormLayout::ItemRole role;
    fl->getItemPosition (index, &row, &role);
    Q_ASSERT(index != -1);
    QLayoutItem *item = fl->takeAt(index);
    const QRect area = QRect(0, row, 2, 1);
    switch (op) {
    case SpanningToLabel:
        fl->setItem(row, QFormLayout::LabelRole, item);
        QLayoutSupport::createEmptyCells(fl);
        break;
    case SpanningToField:
        fl->setItem(row, QFormLayout::FieldRole, item);
        QLayoutSupport::createEmptyCells(fl);
        break;
    case LabelToSpanning:
    case FieldToSpanning:
        QLayoutSupport::removeEmptyCells(fl, area);
        fl->setItem(row, QFormLayout::SpanningRole, item);
        break;
    }
}

unsigned ChangeFormLayoutItemRoleCommand::possibleOperations(QDesignerFormEditorInterface *core, QWidget *w)
{
    QFormLayout *fl = managedFormLayoutOf(core, w);
    if (!fl)
        return 0;
    const int index = fl->indexOf(w);
    if (index == -1)
        return 0;
    int row, col, colspan;
    getFormLayoutItemPosition(fl, index, &row, &col, 0, &colspan);
    // Spanning item?
    if (colspan > 1)
        return SpanningToLabel|SpanningToField;
    // Is the neighbouring column free, that is, can the current item be expanded?
    const QFormLayout::ItemRole neighbouringRole = col == 0 ? QFormLayout::FieldRole : QFormLayout::LabelRole;
    const bool empty = LayoutInfo::isEmptyItem(fl->itemAt(row, neighbouringRole));
    if (empty)
        return col == 0 ? LabelToSpanning : FieldToSpanning;
    return 0;
}

QFormLayout *ChangeFormLayoutItemRoleCommand::managedFormLayoutOf(QDesignerFormEditorInterface *core, QWidget *w)
{
    if (QLayout *layout = LayoutInfo::managedLayout(core, w->parentWidget()))
        if (QFormLayout *fl = qobject_cast<QFormLayout *>(layout))
            return fl;
    return 0;
}

// ---- ChangeLayoutItemGeometry ----
ChangeLayoutItemGeometry::ChangeLayoutItemGeometry(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Change Layout Item Geometry"), formWindow)
{
}

void ChangeLayoutItemGeometry::init(QWidget *widget, int row, int column, int rowspan, int colspan)
{
    m_widget = widget;
    Q_ASSERT(m_widget->parentWidget() != 0);

    QLayout *layout = LayoutInfo::managedLayout(formWindow()->core(), m_widget->parentWidget());
    Q_ASSERT(layout != 0);

    QGridLayout *grid = qobject_cast<QGridLayout*>(layout);
    Q_ASSERT(grid != 0);

    const int itemIndex = grid->indexOf(m_widget);
    Q_ASSERT(itemIndex != -1);

    int current_row, current_column, current_rowspan, current_colspan;
    grid->getItemPosition(itemIndex, &current_row, &current_column, &current_rowspan, &current_colspan);

    m_oldInfo.setRect(current_column, current_row, current_colspan, current_rowspan);
    m_newInfo.setRect(column, row, colspan, rowspan);
}

void ChangeLayoutItemGeometry::changeItemPosition(const QRect &g)
{
    QLayout *layout = LayoutInfo::managedLayout(formWindow()->core(), m_widget->parentWidget());
    Q_ASSERT(layout != 0);

    QGridLayout *grid = qobject_cast<QGridLayout*>(layout);
    Q_ASSERT(grid != 0);

    const int itemIndex = grid->indexOf(m_widget);
    Q_ASSERT(itemIndex != -1);

    QLayoutItem *item = grid->takeAt(itemIndex);
    delete item;

    if (!QLayoutSupport::removeEmptyCells(grid, g))
        qWarning() << "ChangeLayoutItemGeometry::changeItemPosition: Nonempty cell at " <<  g << '.';

    grid->addWidget(m_widget, g.top(), g.left(), g.height(), g.width());

    grid->invalidate();
    grid->activate();

    QLayoutSupport::createEmptyCells(grid);

    formWindow()->clearSelection(false);
    formWindow()->selectWidget(m_widget, true);
}

void ChangeLayoutItemGeometry::redo()
{
    changeItemPosition(m_newInfo);
}

void ChangeLayoutItemGeometry::undo()
{
    changeItemPosition(m_oldInfo);
}

// ---- ContainerWidgetCommand ----
ContainerWidgetCommand::ContainerWidgetCommand(QDesignerFormWindowInterface *formWindow)  :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_index(-1)
{
}

ContainerWidgetCommand::~ContainerWidgetCommand()
{
}

QDesignerContainerExtension *ContainerWidgetCommand::containerExtension() const
{
    QExtensionManager *mgr = core()->extensionManager();
    return qt_extension<QDesignerContainerExtension*>(mgr, m_containerWidget);
}

void ContainerWidgetCommand::init(QWidget *containerWidget)
{
    m_containerWidget = containerWidget;

    if (QDesignerContainerExtension *c = containerExtension()) {
        m_index = c->currentIndex();
        m_widget = c->widget(m_index);
    }
}

void ContainerWidgetCommand::removePage()
{
    if (QDesignerContainerExtension *c = containerExtension()) {
        if (const int count = c->count()) {
            // Undo add after last?
            const int deleteIndex = m_index >= 0 ? m_index : count - 1;
            c->remove(deleteIndex);
            m_widget->hide();
            m_widget->setParent(formWindow());
        }
    }
}

void ContainerWidgetCommand::addPage()
{
    if (QDesignerContainerExtension *c = containerExtension()) {
        int newCurrentIndex;
        if (m_index >= 0) {
            c->insertWidget(m_index, m_widget);
            newCurrentIndex = m_index;
        } else {
            c->addWidget(m_widget);
            newCurrentIndex = c->count() -1 ;
        }
        m_widget->show();
        c->setCurrentIndex(newCurrentIndex);
    }
}

// ---- DeleteContainerWidgetPageCommand ----
DeleteContainerWidgetPageCommand::DeleteContainerWidgetPageCommand(QDesignerFormWindowInterface *formWindow)
    : ContainerWidgetCommand(formWindow)
{
}

DeleteContainerWidgetPageCommand::~DeleteContainerWidgetPageCommand()
{
}

void DeleteContainerWidgetPageCommand::init(QWidget *containerWidget, ContainerType ct)
{
    ContainerWidgetCommand::init(containerWidget);
    switch (ct) {
    case WizardContainer:
    case PageContainer:
        setText(QApplication::translate("Command", "Delete Page"));
        break;
    case MdiContainer:
        setText(QApplication::translate("Command", "Delete Subwindow"));
        break;
    }
}

void DeleteContainerWidgetPageCommand::redo()
{
    removePage();
    cheapUpdate();
}

void DeleteContainerWidgetPageCommand::undo()
{
    addPage();
    cheapUpdate();
}

// ---- AddContainerWidgetPageCommand ----
AddContainerWidgetPageCommand::AddContainerWidgetPageCommand(QDesignerFormWindowInterface *formWindow)
    : ContainerWidgetCommand(formWindow)
{
}

AddContainerWidgetPageCommand::~AddContainerWidgetPageCommand()
{
}

void AddContainerWidgetPageCommand::init(QWidget *containerWidget, ContainerType ct, InsertionMode mode)
{
    m_containerWidget = containerWidget;

    if (QDesignerContainerExtension *c = containerExtension()) {
        m_index = c->currentIndex();
        if (m_index >= 0 && mode == InsertAfter)
            m_index++;
        m_widget = 0;
        const QDesignerFormEditorInterface *core = formWindow()->core();
        switch (ct) {
        case PageContainer:
            setText(QApplication::translate("Command", "Insert Page"));
            m_widget = new QDesignerWidget(formWindow(), m_containerWidget);
            m_widget->setObjectName(QApplication::translate("Command", "page"));
            break;
        case MdiContainer:
            setText(QApplication::translate("Command", "Insert Subwindow"));
            m_widget = new QDesignerWidget(formWindow(), m_containerWidget);
            m_widget->setObjectName(QApplication::translate("Command", "subwindow"));
            setPropertySheetWindowTitle(core, m_widget, QApplication::translate("Command", "Subwindow"));
            break;
        case WizardContainer: // Apply style, don't manage
            m_widget = core->widgetFactory()->createWidget(QLatin1String("QWizardPage"), 0);
            break;
        }
        formWindow()->ensureUniqueObjectName(m_widget);
        core->metaDataBase()->add(m_widget);
    }
}

void AddContainerWidgetPageCommand::redo()
{
    addPage();
    cheapUpdate();
}

void AddContainerWidgetPageCommand::undo()
{
    removePage();
    cheapUpdate();
}

ChangeCurrentPageCommand::ChangeCurrentPageCommand(QDesignerFormWindowInterface *formWindow)
    :
    QDesignerFormWindowCommand(QString(), formWindow), m_oldIndex(0), m_newIndex(0)
{
}

ChangeCurrentPageCommand::~ChangeCurrentPageCommand()
{
}

QDesignerContainerExtension *ChangeCurrentPageCommand::containerExtension() const
{
    QExtensionManager *mgr = core()->extensionManager();
    return qt_extension<QDesignerContainerExtension*>(mgr, m_containerWidget);
}

void ChangeCurrentPageCommand::init(QWidget *containerWidget, int newIndex)
{
    m_containerWidget = containerWidget;

    if (QDesignerContainerExtension *c = containerExtension()) {
        m_newIndex = newIndex;
        m_oldIndex = c->currentIndex();
        m_widget = c->widget(m_oldIndex);
    }
}

void ChangeCurrentPageCommand::redo()
{
    containerExtension()->setCurrentIndex(m_newIndex);
}

void ChangeCurrentPageCommand::undo()
{
    containerExtension()->setCurrentIndex(m_oldIndex);
}

static int itemRoles[] = {
    Qt::DecorationPropertyRole,
    Qt::DisplayPropertyRole,
    Qt::ToolTipPropertyRole,
    Qt::StatusTipPropertyRole,
    Qt::WhatsThisPropertyRole,
    Qt::FontRole,
    Qt::TextAlignmentRole,
    Qt::BackgroundRole,
    Qt::ForegroundRole,
    Qt::CheckStateRole,
    -1
};

template<class T>
static void copyRoleFromItem(ItemData *id, int role, const T *item)
{
    QVariant v = item->data(role);
    if (v.isValid())
        id->m_properties.insert(role, v);
}

template<class T>
static void copyRolesFromItem(ItemData *id, const T *item, bool editor)
{
    static const int defaultFlags = T().flags();

    for (int i = 0; itemRoles[i] != -1; i++)
        copyRoleFromItem<T>(id, itemRoles[i], item);

    if (editor)
        copyRoleFromItem<T>(id, ItemFlagsShadowRole, item);
    else if (item->flags() != defaultFlags)
        id->m_properties.insert(ItemFlagsShadowRole, qVariantFromValue((int)item->flags()));
}

template<class T>
static void copyRolesToItem(const ItemData *id, T *item, DesignerIconCache *iconCache, bool editor)
{
    QHash<int, QVariant>::const_iterator it = id->m_properties.constBegin(),
            end = id->m_properties.constEnd();
    for (; it != end; ++it)
        if (it.value().isValid()) {
            if (!editor && it.key() == ItemFlagsShadowRole) {
                item->setFlags((Qt::ItemFlags)it.value().toInt());
            } else {
                item->setData(it.key(), it.value());
                switch (it.key()) {
                case Qt::DecorationPropertyRole:
                    if (iconCache)
                        item->setIcon(iconCache->icon(qVariantValue<PropertySheetIconValue>(it.value())));
                    break;
                case Qt::DisplayPropertyRole:
                    item->setText(qVariantValue<PropertySheetStringValue>(it.value()).value());
                    break;
                case Qt::ToolTipPropertyRole:
                    item->setToolTip(qVariantValue<PropertySheetStringValue>(it.value()).value());
                    break;
                case Qt::StatusTipPropertyRole:
                    item->setStatusTip(qVariantValue<PropertySheetStringValue>(it.value()).value());
                    break;
                case Qt::WhatsThisPropertyRole:
                    item->setWhatsThis(qVariantValue<PropertySheetStringValue>(it.value()).value());
                    break;
                }
            }
        }

    if (editor)
        item->setFlags(item->flags() | Qt::ItemIsEditable);
}

ItemData::ItemData(const QListWidgetItem *item, bool editor)
{
    copyRolesFromItem<QListWidgetItem>(this, item, editor);
}

QListWidgetItem *ItemData::createListItem(DesignerIconCache *iconCache, bool editor) const
{
    QListWidgetItem *item = new QListWidgetItem();
    copyRolesToItem(this, item, iconCache, editor);
    return item;
}

ItemData::ItemData(const QTableWidgetItem *item, bool editor)
{
    copyRolesFromItem(this, item, editor);
}

QTableWidgetItem *ItemData::createTableItem(DesignerIconCache *iconCache, bool editor) const
{
    QTableWidgetItem *item = new QTableWidgetItem;
    copyRolesToItem(this, item, iconCache, editor);
    return item;
}

static void copyRoleFromItem(ItemData *id, int role, const QTreeWidgetItem *item, int column)
{
    QVariant v = item->data(column, role);
    if (v.isValid())
        id->m_properties.insert(role, v);
}

ItemData::ItemData(const QTreeWidgetItem *item, int column)
{
    copyRoleFromItem(this, Qt::EditRole, item, column);
    PropertySheetStringValue str(item->text(column));
    m_properties.insert(Qt::DisplayPropertyRole, qVariantFromValue(str));

    for (int i = 0; itemRoles[i] != -1; i++)
        copyRoleFromItem(this, itemRoles[i], item, column);
}

void ItemData::fillTreeItemColumn(QTreeWidgetItem *item, int column, DesignerIconCache *iconCache) const
{
    QHash<int, QVariant>::const_iterator it = m_properties.constBegin(), end = m_properties.constEnd();
    for (; it != end; ++it)
        if (it.value().isValid()) {
            item->setData(column, it.key(), it.value());
            switch (it.key()) {
            case Qt::DecorationPropertyRole:
                if (iconCache)
                    item->setIcon(column, iconCache->icon(qVariantValue<PropertySheetIconValue>(it.value())));
                break;
            case Qt::DisplayPropertyRole:
                item->setText(column, qVariantValue<PropertySheetStringValue>(it.value()).value());
                break;
            case Qt::ToolTipPropertyRole:
                item->setToolTip(column, qVariantValue<PropertySheetStringValue>(it.value()).value());
                break;
            case Qt::StatusTipPropertyRole:
                item->setStatusTip(column, qVariantValue<PropertySheetStringValue>(it.value()).value());
                break;
            case Qt::WhatsThisPropertyRole:
                item->setWhatsThis(column, qVariantValue<PropertySheetStringValue>(it.value()).value());
                break;
            }
        }
}

ListContents::ListContents(const QTreeWidgetItem *item)
{
    for (int i = 0; i < item->columnCount(); i++)
        m_items.append(ItemData(item, i));
}

QTreeWidgetItem *ListContents::createTreeItem(DesignerIconCache *iconCache) const
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    int i = 0;
    foreach (const ItemData &id, m_items)
        id.fillTreeItemColumn(item, i++, iconCache);
    return item;
}

void ListContents::createFromListWidget(const QListWidget *listWidget, bool editor)
{
    m_items.clear();

    for (int i = 0; i < listWidget->count(); i++)
        m_items.append(ItemData(listWidget->item(i), editor));
}

void ListContents::applyToListWidget(QListWidget *listWidget, DesignerIconCache *iconCache, bool editor) const
{
    listWidget->clear();

    int i = 0;
    foreach (const ItemData &entry, m_items) {
        if (!entry.isValid())
            new QListWidgetItem(TableWidgetContents::defaultHeaderText(i), listWidget);
        else
            listWidget->addItem(entry.createListItem(iconCache, editor));
        i++;
    }
}

void ListContents::createFromComboBox(const QComboBox *comboBox)
{
    m_items.clear();

    const int count = comboBox->count();
    for (int i = 0; i < count; i++) {
        // We might encounter items added in a custom combo
        // constructor. Ignore those.
        const QVariant textValue = comboBox->itemData(i, Qt::DisplayPropertyRole);
        if (!textValue.isNull()) {
            ItemData entry;
            entry.m_properties.insert(Qt::DisplayPropertyRole, textValue);
            const QVariant iconValue =  comboBox->itemData(i, Qt::DecorationPropertyRole);
            if (!iconValue.isNull())
                entry.m_properties.insert(Qt::DecorationPropertyRole, iconValue);
            m_items.append(entry);
        }
    }
}

void ListContents::applyToComboBox(QComboBox *comboBox, DesignerIconCache *iconCache) const
{
    comboBox->clear();

    foreach (const ItemData &hash, m_items) {
        QIcon icon;
        if (iconCache)
            icon = iconCache->icon(qVariantValue<PropertySheetIconValue>(
                    hash.m_properties[Qt::DecorationPropertyRole]));
        QVariant var = hash.m_properties[Qt::DisplayPropertyRole];
        PropertySheetStringValue str = qVariantValue<PropertySheetStringValue>(var);
        comboBox->addItem(icon, str.value());
        comboBox->setItemData(comboBox->count() - 1,
                              var,
                              Qt::DisplayPropertyRole);
        comboBox->setItemData(comboBox->count() - 1,
                              hash.m_properties[Qt::DecorationPropertyRole],
                              Qt::DecorationPropertyRole);
    }
}

// --------- TableWidgetContents

TableWidgetContents::TableWidgetContents() :
    m_columnCount(0),
    m_rowCount(0)
{
}

void TableWidgetContents::clear()
{
    m_horizontalHeader.m_items.clear();
    m_verticalHeader.m_items.clear();
    m_items.clear();
    m_columnCount = 0;
    m_rowCount = 0;
}

QString TableWidgetContents::defaultHeaderText(int i)
{
    return QString::number(i + 1);
}

bool TableWidgetContents::nonEmpty(const QTableWidgetItem *item, int headerColumn)
{
    static int defaultFlags = QTableWidgetItem().flags();

    if (item->flags() != defaultFlags)
        return true;

    QString text = qVariantValue<PropertySheetStringValue>(item->data(Qt::DisplayPropertyRole)).value();
    if (!text.isEmpty()) {
        if (headerColumn < 0 || text != defaultHeaderText(headerColumn))
            return true;
    } else {
        // FIXME: This doesn't seem to make sense
        return true;
    }

    for (int i = 0; itemRoles[i] != -1; i++)
        if (itemRoles[i] != Qt::DisplayPropertyRole && item->data(itemRoles[i]).isValid())
            return true;

    return false;
}

void TableWidgetContents::insertHeaderItem(const QTableWidgetItem *item, int i, ListContents *header, bool editor)
{
    if (nonEmpty(item, i))
        header->m_items.append(ItemData(item, editor));
    else
        header->m_items.append(ItemData());
}

void TableWidgetContents::fromTableWidget(const QTableWidget *tableWidget, bool editor)
{
    clear();
    m_columnCount = tableWidget->columnCount();
    m_rowCount = tableWidget->rowCount();
    // horiz header: Legacy behaviour: auto-generate number for empty items
    for (int col = 0; col <  m_columnCount; col++)
        if (const QTableWidgetItem *item = tableWidget->horizontalHeaderItem(col))
            insertHeaderItem(item, col, &m_horizontalHeader, editor);
    // vertical header: Legacy behaviour: auto-generate number for empty items
    for (int row = 0; row < m_rowCount; row++)
        if (const QTableWidgetItem *item = tableWidget->verticalHeaderItem(row))
            insertHeaderItem(item, row, &m_verticalHeader, editor);
    // cell data
    for (int col = 0; col < m_columnCount; col++)
        for (int row = 0; row < m_rowCount; row++)
            if (const QTableWidgetItem *item = tableWidget->item(row, col))
                if (nonEmpty(item, -1))
                    m_items.insert(CellRowColumnAddress(row, col), ItemData(item, editor));
}

void TableWidgetContents::applyToTableWidget(QTableWidget *tableWidget, DesignerIconCache *iconCache, bool editor) const
{
    tableWidget->clear();

    tableWidget->setColumnCount(m_columnCount);
    tableWidget->setRowCount(m_rowCount);

    // horiz header
    int col = 0;
    foreach (const ItemData &id, m_horizontalHeader.m_items) {
        if (id.isValid())
            tableWidget->setHorizontalHeaderItem(col, id.createTableItem(iconCache, editor));
        col++;
    }
    // vertical header
    int row = 0;
    foreach (const ItemData &id, m_verticalHeader.m_items) {
        if (id.isValid())
            tableWidget->setVerticalHeaderItem(row, id.createTableItem(iconCache, editor));
        row++;
    }
    // items
    const TableItemMap::const_iterator icend = m_items.constEnd();
    for (TableItemMap::const_iterator it = m_items.constBegin(); it !=  icend; ++ it)
        tableWidget->setItem(it.key().first, it.key().second, it.value().createTableItem(iconCache, editor));
}

bool TableWidgetContents::operator==(const TableWidgetContents &rhs) const
{
    if (m_columnCount != rhs.m_columnCount || m_rowCount !=  rhs.m_rowCount)
        return false;

    return m_horizontalHeader.m_items == rhs.m_horizontalHeader.m_items &&
           m_verticalHeader.m_items == rhs.m_verticalHeader.m_items &&
           m_items == rhs.m_items;
}

// ---- ChangeTableContentsCommand ----
ChangeTableContentsCommand::ChangeTableContentsCommand(QDesignerFormWindowInterface *formWindow)  :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Change Table Contents"),
    formWindow), m_iconCache(0)
{
    FormWindowBase *fwb = qobject_cast<FormWindowBase *>(formWindow);
    if (fwb)
        m_iconCache = fwb->iconCache();
}

void ChangeTableContentsCommand::init(QTableWidget *tableWidget,
        const TableWidgetContents &oldCont, const TableWidgetContents &newCont)
{
    m_tableWidget = tableWidget;
    m_oldContents = oldCont;
    m_newContents = newCont;
}

void ChangeTableContentsCommand::redo()
{
    m_newContents.applyToTableWidget(m_tableWidget, m_iconCache, false);
    QMetaObject::invokeMethod(m_tableWidget, "updateGeometries");
}

void ChangeTableContentsCommand::undo()
{
    m_oldContents.applyToTableWidget(m_tableWidget, m_iconCache, false);
    QMetaObject::invokeMethod(m_tableWidget, "updateGeometries");
}

// --------- TreeWidgetContents
TreeWidgetContents::ItemContents::ItemContents(const QTreeWidgetItem *item, bool editor) :
    ListContents(item)
{
    static const int defaultFlags = QTreeWidgetItem().flags();

    if (editor) {
        QVariant v = item->data(0, ItemFlagsShadowRole);
        m_itemFlags = v.isValid() ? v.toInt() : -1;
    } else  {
        m_itemFlags = (item->flags() != defaultFlags) ? (int)item->flags() : -1;
    }

    for (int i = 0; i < item->childCount(); i++)
        m_children.append(ItemContents(item->child(i), editor));
}

QTreeWidgetItem *TreeWidgetContents::ItemContents::createTreeItem(DesignerIconCache *iconCache, bool editor) const
{
    QTreeWidgetItem *item = ListContents::createTreeItem(iconCache);

    if (editor)
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (m_itemFlags != -1) {
        if (editor)
            item->setData(0, ItemFlagsShadowRole, qVariantFromValue(m_itemFlags));
        else
            item->setFlags((Qt::ItemFlags)m_itemFlags);
    }

    foreach (const ItemContents &ic, m_children)
        item->addChild(ic.createTreeItem(iconCache, editor));

    return item;
}

bool TreeWidgetContents::ItemContents::operator==(const TreeWidgetContents::ItemContents &rhs) const
{
    return
        m_itemFlags == rhs.m_itemFlags &&
        m_items == rhs.m_items &&
        m_children == rhs.m_children;
}

void TreeWidgetContents::clear()
{
    m_headerItem.m_items.clear();
    m_rootItems.clear();
}

void TreeWidgetContents::fromTreeWidget(const QTreeWidget *treeWidget, bool editor)
{
    clear();
    m_headerItem = ListContents(treeWidget->headerItem());
    for (int col = 0; col < treeWidget->topLevelItemCount(); col++)
        m_rootItems.append(ItemContents(treeWidget->topLevelItem(col), editor));
}

void TreeWidgetContents::applyToTreeWidget(QTreeWidget *treeWidget, DesignerIconCache *iconCache, bool editor) const
{
    treeWidget->clear();

    treeWidget->setColumnCount(m_headerItem.m_items.count());
    treeWidget->setHeaderItem(m_headerItem.createTreeItem(iconCache));
    foreach (const ItemContents &ic, m_rootItems)
        treeWidget->addTopLevelItem(ic.createTreeItem(iconCache, editor));
    treeWidget->expandAll();
}

bool TreeWidgetContents::operator==(const TreeWidgetContents &rhs) const
{
    return
        m_headerItem == rhs.m_headerItem &&
        m_rootItems == rhs.m_rootItems;
}

// ---- ChangeTreeContentsCommand ----
ChangeTreeContentsCommand::ChangeTreeContentsCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Change Tree Contents"), formWindow),
        m_iconCache(0)
{
    FormWindowBase *fwb = qobject_cast<FormWindowBase *>(formWindow);
    if (fwb)
        m_iconCache = fwb->iconCache();
}

void ChangeTreeContentsCommand::init(QTreeWidget *treeWidget,
        const TreeWidgetContents &oldState, const TreeWidgetContents &newState)
{
    m_treeWidget = treeWidget;
    m_oldState = oldState;
    m_newState = newState;
}

void ChangeTreeContentsCommand::redo()
{
    m_newState.applyToTreeWidget(m_treeWidget, m_iconCache, false);
}

void ChangeTreeContentsCommand::undo()
{
    m_oldState.applyToTreeWidget(m_treeWidget, m_iconCache, false);
}

// ---- ChangeListContentsCommand ----
ChangeListContentsCommand::ChangeListContentsCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QString(), formWindow), m_iconCache(0)
{
    FormWindowBase *fwb = qobject_cast<FormWindowBase *>(formWindow);
    if (fwb)
        m_iconCache = fwb->iconCache();
}

void ChangeListContentsCommand::init(QListWidget *listWidget,
        const ListContents &oldItems, const ListContents &items)
{
    m_listWidget = listWidget;
    m_comboBox = 0;

    m_newItemsState = items;
    m_oldItemsState = oldItems;
}

void ChangeListContentsCommand::init(QComboBox *comboBox,
        const ListContents &oldItems, const ListContents &items)
{
    m_listWidget = 0;
    m_comboBox = comboBox;

    m_newItemsState = items;
    m_oldItemsState = oldItems;
}

void ChangeListContentsCommand::redo()
{
    if (m_listWidget)
        m_newItemsState.applyToListWidget(m_listWidget, m_iconCache, false);
    else if (m_comboBox)
        m_newItemsState.applyToComboBox(m_comboBox, m_iconCache);
}

void ChangeListContentsCommand::undo()
{
    if (m_listWidget)
        m_oldItemsState.applyToListWidget(m_listWidget, m_iconCache, false);
    else if (m_comboBox)
        m_oldItemsState.applyToComboBox(m_comboBox, m_iconCache);
}

// ---- AddActionCommand ----

AddActionCommand::AddActionCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Add action"), formWindow)
{
    m_action = 0;
}

void AddActionCommand::init(QAction *action)
{
    Q_ASSERT(m_action == 0);
    m_action = action;
}

void AddActionCommand::redo()
{
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->manageAction(m_action);
}

void AddActionCommand::undo()
{
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->unmanageAction(m_action);
}

// ---- RemoveActionCommand ----

RemoveActionCommand::RemoveActionCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Remove action"), formWindow),
    m_action(0)
{
}

static RemoveActionCommand::ActionData findActionIn(QAction *action)
{
    RemoveActionCommand::ActionData result;
    // We only want menus and toolbars, no toolbuttons.
    foreach (QWidget *widget, action->associatedWidgets())
        if (qobject_cast<const QMenu *>(widget) || qobject_cast<const QToolBar *>(widget)) {
            const QList<QAction*> actionList = widget->actions();
            const int size = actionList.size();
            for (int i = 0; i < size; ++i) {
                if (actionList.at(i) == action) {
                    QAction *before = 0;
                    if (i + 1 < size)
                        before = actionList.at(i + 1);
                    result.append(RemoveActionCommand::ActionDataItem(before, widget));
                    break;
                }
            }
        }
    return result;
}

void RemoveActionCommand::init(QAction *action)
{
    Q_ASSERT(m_action == 0);
    m_action = action;

    m_actionData = findActionIn(action);
}

void RemoveActionCommand::redo()
{
    QDesignerFormWindowInterface *fw = formWindow();
    foreach (const ActionDataItem &item, m_actionData) {
        item.widget->removeAction(m_action);
    }
    // Notify components (for example, signal slot editor)
    if (qdesigner_internal::FormWindowBase *fwb = qobject_cast<qdesigner_internal::FormWindowBase *>(fw))
        fwb->emitObjectRemoved(m_action);

    core()->actionEditor()->setFormWindow(fw);
    core()->actionEditor()->unmanageAction(m_action);
    if (!m_actionData.empty())
        core()->objectInspector()->setFormWindow(fw);
}

void RemoveActionCommand::undo()
{
    core()->actionEditor()->setFormWindow(formWindow());
    core()->actionEditor()->manageAction(m_action);
    foreach (const ActionDataItem &item, m_actionData) {
        item.widget->insertAction(item.before, m_action);
    }
    if (!m_actionData.empty())
        core()->objectInspector()->setFormWindow(formWindow());
}

// ---- ActionInsertionCommand ----

ActionInsertionCommand::ActionInsertionCommand(const QString &text, QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(text, formWindow),
    m_parentWidget(0),
    m_action(0),
    m_beforeAction(0),
    m_update(false)
{
}

void ActionInsertionCommand::init(QWidget *parentWidget, QAction *action, QAction *beforeAction, bool update)
{
    Q_ASSERT(m_parentWidget == 0);
    Q_ASSERT(m_action == 0);

    m_parentWidget = parentWidget;
    m_action = action;
    m_beforeAction = beforeAction;
    m_update = update;
}

void ActionInsertionCommand::insertAction()
{
    Q_ASSERT(m_action != 0);
    Q_ASSERT(m_parentWidget != 0);

    if (m_beforeAction)
        m_parentWidget->insertAction(m_beforeAction, m_action);
    else
        m_parentWidget->addAction(m_action);

    if (m_update) {
        cheapUpdate();
        if (QMenu *menu = m_action->menu())
            selectUnmanagedObject(menu);
        else
            selectUnmanagedObject(m_action);
        PropertyHelper::triggerActionChanged(m_action); // Update Used column in action editor.
    }
}
void ActionInsertionCommand::removeAction()
{
    Q_ASSERT(m_action != 0);
    Q_ASSERT(m_parentWidget != 0);

    if (QDesignerMenu *menu = qobject_cast<QDesignerMenu*>(m_parentWidget))
        menu->hideSubMenu();

    m_parentWidget->removeAction(m_action);

    if (m_update) {
        cheapUpdate();
        selectUnmanagedObject(m_parentWidget);
        PropertyHelper::triggerActionChanged(m_action); // Update Used column in action editor.
    }
}

InsertActionIntoCommand::InsertActionIntoCommand(QDesignerFormWindowInterface *formWindow) :
    ActionInsertionCommand(QApplication::translate("Command", "Add action"), formWindow)
{
}
// ---- RemoveActionFromCommand ----

RemoveActionFromCommand::RemoveActionFromCommand(QDesignerFormWindowInterface *formWindow) :
    ActionInsertionCommand(QApplication::translate("Command", "Remove action"), formWindow)
{
}

// ---- AddMenuActionCommand ----

MenuActionCommand::MenuActionCommand(const QString &text, QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(text, formWindow),
    m_action(0),
    m_actionBefore(0),
    m_menuParent(0),
    m_associatedWidget(0),
    m_objectToSelect(0)
{
}

void MenuActionCommand::init(QAction *action, QAction *actionBefore,
                             QWidget *associatedWidget, QWidget *objectToSelect)
{
    QMenu *menu = action->menu();
    Q_ASSERT(menu);
    m_menuParent = menu->parentWidget();
    m_action = action;
    m_actionBefore = actionBefore;
    m_associatedWidget = associatedWidget;
    m_objectToSelect = objectToSelect;
}

void MenuActionCommand::insertMenu()
{
    core()->metaDataBase()->add(m_action);
    QMenu *menu = m_action->menu();
    if (m_menuParent && menu->parentWidget() != m_menuParent)
        menu->setParent(m_menuParent);
    core()->metaDataBase()->add(menu);
    m_associatedWidget->insertAction(m_actionBefore, m_action);
    cheapUpdate();
    selectUnmanagedObject(menu);
}

void MenuActionCommand::removeMenu()
{
    m_action->menu()->setParent(0);
    QMenu *menu = m_action->menu();
    core()->metaDataBase()->remove(menu);
    menu->setParent(0);
    core()->metaDataBase()->remove(m_action);
    m_associatedWidget->removeAction(m_action);
    cheapUpdate();
    selectUnmanagedObject(m_objectToSelect);
}

AddMenuActionCommand::AddMenuActionCommand(QDesignerFormWindowInterface *formWindow)  :
    MenuActionCommand(QApplication::translate("Command", "Add menu"), formWindow)
{
}

// ---- RemoveMenuActionCommand ----
RemoveMenuActionCommand::RemoveMenuActionCommand(QDesignerFormWindowInterface *formWindow) :
    MenuActionCommand(QApplication::translate("Command", "Remove menu"), formWindow)
{
}

// ---- CreateSubmenuCommand ----
CreateSubmenuCommand::CreateSubmenuCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QApplication::translate("Command", "Create submenu"), formWindow),
    m_action(0),
    m_menu(0),
    m_objectToSelect(0)
{
}

void CreateSubmenuCommand::init(QDesignerMenu *menu, QAction *action, QObject *objectToSelect)
{
    m_menu = menu;
    m_action = action;
    m_objectToSelect = objectToSelect;
}

void CreateSubmenuCommand::redo()
{
    m_menu->createRealMenuAction(m_action);
    cheapUpdate();
    if (m_objectToSelect)
        selectUnmanagedObject(m_objectToSelect);
}

void CreateSubmenuCommand::undo()
{
    m_menu->removeRealMenu(m_action);
    cheapUpdate();
    selectUnmanagedObject(m_menu);
}

// ---- DeleteToolBarCommand ----
DeleteToolBarCommand::DeleteToolBarCommand(QDesignerFormWindowInterface *formWindow)
    : QDesignerFormWindowCommand(QApplication::translate("Command", "Delete Tool Bar"), formWindow)
{
}

void DeleteToolBarCommand::init(QToolBar *toolBar)
{
    m_toolBar = toolBar;
    m_mainWindow = qobject_cast<QMainWindow*>(toolBar->parentWidget());
}

void DeleteToolBarCommand::redo()
{
    if (m_mainWindow) {
        QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);
        Q_ASSERT(c != 0);
        for (int i=0; i<c->count(); ++i) {
            if (c->widget(i) == m_toolBar) {
                c->remove(i);
                break;
            }
        }
    }

    core()->metaDataBase()->remove(m_toolBar);
    m_toolBar->hide();
    m_toolBar->setParent(formWindow());
    formWindow()->emitSelectionChanged();
}

void DeleteToolBarCommand::undo()
{
    if (m_mainWindow) {
        m_toolBar->setParent(m_mainWindow);
        QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);

        c->addWidget(m_toolBar);

        core()->metaDataBase()->add(m_toolBar);
        m_toolBar->show();
        formWindow()->emitSelectionChanged();
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
