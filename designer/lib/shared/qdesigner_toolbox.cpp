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

#include "qdesigner_toolbox_p.h"
#include "qdesigner_command_p.h"
#include "orderdialog_p.h"
#include "promotiontaskmenu_p.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtCore/QEvent>
#include <QtGui/QAction>
#include <QtGui/QToolBox>
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

QToolBoxHelper::QToolBoxHelper(QToolBox *toolbox) :
    QObject(toolbox),
    m_toolbox(toolbox),
    m_actionDeletePage(new QAction(tr("Delete Page"), this)),
    m_actionInsertPage(new QAction(tr("Before Current Page"), this)),
    m_actionInsertPageAfter(new QAction(tr("After Current Page"), this)),
    m_actionChangePageOrder(new QAction(tr("Change Page Order..."), this)),
    m_pagePromotionTaskMenu(new qdesigner_internal::PromotionTaskMenu(0, qdesigner_internal::PromotionTaskMenu::ModeSingleWidget, this))
{
    connect(m_actionDeletePage, SIGNAL(triggered()), this, SLOT(removeCurrentPage()));
    connect(m_actionInsertPage, SIGNAL(triggered()), this, SLOT(addPage()));
    connect(m_actionInsertPageAfter, SIGNAL(triggered()), this, SLOT(addPageAfter()));
    connect(m_actionChangePageOrder, SIGNAL(triggered()), this, SLOT(changeOrder()));

    m_toolbox->installEventFilter(this);
}

void QToolBoxHelper::install(QToolBox *toolbox)
{
    new QToolBoxHelper(toolbox);
}

bool QToolBoxHelper::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
    case QEvent::ChildPolished:
        // Install on the buttons
        if (watched == m_toolbox) {
            QChildEvent *ce = static_cast<QChildEvent *>(event);
            if (!qstrcmp(ce->child()->metaObject()->className(), "QToolBoxButton"))
                ce->child()->installEventFilter(this);
        }
        break;
    case QEvent::ContextMenu:
        if (watched != m_toolbox) {
            // An action invoked from the passive interactor (ToolBox button) might
            // cause its deletion within its event handler, triggering a warning. Re-post
            // the event to the toolbox.
            QContextMenuEvent *current = static_cast<QContextMenuEvent *>(event);
            QContextMenuEvent *copy = new QContextMenuEvent(current->reason(), current->pos(), current-> globalPos(), current->modifiers());
            QApplication::postEvent(m_toolbox, copy);
            current->accept();
            return true;
        }
        break;
    case QEvent::MouseButtonRelease:
        if (watched != m_toolbox)
            if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
                fw->clearSelection();
                fw->selectWidget(m_toolbox, true);
            }
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

QToolBoxHelper *QToolBoxHelper::helperOf(const QToolBox *toolbox)
{
    // Look for 1st order children only..otherwise, we might get filters of nested widgets
    const QObjectList children = toolbox->children();
    const QObjectList::const_iterator cend = children.constEnd();
    for (QObjectList::const_iterator it = children.constBegin(); it != cend; ++it) {
        QObject *o = *it;
        if (!o->isWidgetType())
            if (QToolBoxHelper *h = qobject_cast<QToolBoxHelper *>(o))
                return h;
    }
    return 0;
}

QMenu *QToolBoxHelper::addToolBoxContextMenuActions(const QToolBox *toolbox, QMenu *popup)
{
    QToolBoxHelper *helper = helperOf(toolbox);
    if (!helper)
        return 0;
    return helper->addContextMenuActions(popup);
}

void QToolBoxHelper::removeCurrentPage()
{
    if (m_toolbox->currentIndex() == -1 || !m_toolbox->widget(m_toolbox->currentIndex()))
        return;

    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::DeleteToolBoxPageCommand *cmd = new qdesigner_internal::DeleteToolBoxPageCommand(fw);
        cmd->init(m_toolbox);
        fw->commandHistory()->push(cmd);
    }
}

void QToolBoxHelper::addPage()
{
    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::AddToolBoxPageCommand *cmd = new qdesigner_internal::AddToolBoxPageCommand(fw);
        cmd->init(m_toolbox, qdesigner_internal::AddToolBoxPageCommand::InsertBefore);
        fw->commandHistory()->push(cmd);
    }
}

void QToolBoxHelper::changeOrder()
{
    QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox);

    if (!fw)
        return;

    const QWidgetList oldPages = qdesigner_internal::OrderDialog::pagesOfContainer(fw->core(), m_toolbox);
    const int pageCount = oldPages.size();
    if (pageCount < 2)
        return;

    qdesigner_internal::OrderDialog dlg(fw);
    dlg.setPageList(oldPages);
    if (dlg.exec() == QDialog::Rejected)
        return;

    const QWidgetList newPages = dlg.pageList();
    if (newPages == oldPages)
        return;

    fw->beginCommand(tr("Change Page Order"));
    for(int i=0; i < pageCount; ++i) {
        if (newPages.at(i) == m_toolbox->widget(i))
            continue;
        qdesigner_internal::MoveToolBoxPageCommand *cmd = new qdesigner_internal::MoveToolBoxPageCommand(fw);
        cmd->init(m_toolbox, newPages.at(i), i);
        fw->commandHistory()->push(cmd);
    }
    fw->endCommand();
}

void QToolBoxHelper::addPageAfter()
{
    if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(m_toolbox)) {
        qdesigner_internal::AddToolBoxPageCommand *cmd = new qdesigner_internal::AddToolBoxPageCommand(fw);
        cmd->init(m_toolbox, qdesigner_internal::AddToolBoxPageCommand::InsertAfter);
        fw->commandHistory()->push(cmd);
    }
}

QPalette::ColorRole QToolBoxHelper::currentItemBackgroundRole() const
{
    const QWidget *w = m_toolbox->widget(0);
    if (!w)
        return  QPalette::Window;
    return w->backgroundRole();
}

void QToolBoxHelper::setCurrentItemBackgroundRole(QPalette::ColorRole role)
{
    const int count = m_toolbox->count();
    for (int i = 0; i < count; ++i) {
        QWidget *w = m_toolbox->widget(i);
        w->setBackgroundRole(role);
        w->update();
    }
}

QMenu *QToolBoxHelper::addContextMenuActions(QMenu *popup) const
{
    QMenu *pageMenu = 0;
    const int count = m_toolbox->count();
    m_actionDeletePage->setEnabled(count > 1);
    if (count) {
        const QString pageSubMenuLabel = tr("Page %1 of %2").arg(m_toolbox->currentIndex() + 1).arg(count);
        pageMenu = popup->addMenu(pageSubMenuLabel);

        pageMenu->addAction(m_actionDeletePage);
        // Set up promotion menu for current widget.
        if (QWidget *page =  m_toolbox->currentWidget ()) {
            m_pagePromotionTaskMenu->setWidget(page);
            m_pagePromotionTaskMenu->addActions(QDesignerFormWindowInterface::findFormWindow(m_toolbox),
                                                qdesigner_internal::PromotionTaskMenu::SuppressGlobalEdit,
                                                pageMenu);
        }
    }
    QMenu *insertPageMenu = popup->addMenu(tr("Insert Page"));
    insertPageMenu->addAction(m_actionInsertPageAfter);
    insertPageMenu->addAction(m_actionInsertPage);
    if (count > 1) {
        popup->addAction(m_actionChangePageOrder);
    }
    popup->addSeparator();
    return pageMenu;
}

// -------- QToolBoxWidgetPropertySheet

static const char *currentItemTextKey = "currentItemText";
static const char *currentItemNameKey = "currentItemName";
static const char *currentItemIconKey = "currentItemIcon";
static const char *currentItemToolTipKey = "currentItemToolTip";
static const char *tabSpacingKey = "tabSpacing";

enum { tabSpacingDefault = -1 };

QToolBoxWidgetPropertySheet::QToolBoxWidgetPropertySheet(QToolBox *object, QObject *parent) :
    QDesignerPropertySheet(object, parent),
    m_toolBox(object)
{
    createFakeProperty(QLatin1String(currentItemTextKey), qVariantFromValue(qdesigner_internal::PropertySheetStringValue()));
    createFakeProperty(QLatin1String(currentItemNameKey), QString());
    createFakeProperty(QLatin1String(currentItemIconKey), qVariantFromValue(qdesigner_internal::PropertySheetIconValue()));
    if (formWindowBase())
        formWindowBase()->addReloadableProperty(this, indexOf(QLatin1String(currentItemIconKey)));
    createFakeProperty(QLatin1String(currentItemToolTipKey), qVariantFromValue(qdesigner_internal::PropertySheetStringValue()));
    createFakeProperty(QLatin1String(tabSpacingKey), QVariant(tabSpacingDefault));
}

QToolBoxWidgetPropertySheet::ToolBoxProperty QToolBoxWidgetPropertySheet::toolBoxPropertyFromName(const QString &name)
{
    typedef QHash<QString, ToolBoxProperty> ToolBoxPropertyHash;
    static ToolBoxPropertyHash toolBoxPropertyHash;
    if (toolBoxPropertyHash.empty()) {
        toolBoxPropertyHash.insert(QLatin1String(currentItemTextKey),    PropertyCurrentItemText);
        toolBoxPropertyHash.insert(QLatin1String(currentItemNameKey),    PropertyCurrentItemName);
        toolBoxPropertyHash.insert(QLatin1String(currentItemIconKey),    PropertyCurrentItemIcon);
        toolBoxPropertyHash.insert(QLatin1String(currentItemToolTipKey), PropertyCurrentItemToolTip);
        toolBoxPropertyHash.insert(QLatin1String(tabSpacingKey),         PropertyTabSpacing);
    }
    return toolBoxPropertyHash.value(name, PropertyToolBoxNone);
}

void QToolBoxWidgetPropertySheet::setProperty(int index, const QVariant &value)
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        m_toolBox->layout()->setSpacing(value.toInt());
        return;
    case PropertyToolBoxNone:
        QDesignerPropertySheet::setProperty(index, value);
        return;
    default:
        break;
    }
    // index-dependent
    const int currentIndex = m_toolBox->currentIndex();
    QWidget *currentWidget = m_toolBox->currentWidget();
    if (!currentWidget)
        return;

    switch (toolBoxProperty) {
    case PropertyCurrentItemText:
        m_toolBox->setItemText(currentIndex, qvariant_cast<QString>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].text = qVariantValue<qdesigner_internal::PropertySheetStringValue>(value);
        break;
    case PropertyCurrentItemName:
        currentWidget->setObjectName(value.toString());
        break;
    case PropertyCurrentItemIcon:
        m_toolBox->setItemIcon(currentIndex, qvariant_cast<QIcon>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].icon = qVariantValue<qdesigner_internal::PropertySheetIconValue>(value);
        break;
    case PropertyCurrentItemToolTip:
        m_toolBox->setItemToolTip(currentIndex, qvariant_cast<QString>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].tooltip = qVariantValue<qdesigner_internal::PropertySheetStringValue>(value);
        break;
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
}

bool QToolBoxWidgetPropertySheet::isEnabled(int index) const
{
    switch (toolBoxPropertyFromName(propertyName(index))) {
    case PropertyToolBoxNone:  // independent of index
    case PropertyTabSpacing:
        return QDesignerPropertySheet::isEnabled(index);
    default:
        break;
    }
    return m_toolBox->currentIndex() != -1;
}

QVariant QToolBoxWidgetPropertySheet::property(int index) const
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        return m_toolBox->layout()->spacing();
    case PropertyToolBoxNone:
        return QDesignerPropertySheet::property(index);
    default:
        break;
    }
    // index-dependent
    QWidget *currentWidget = m_toolBox->currentWidget();
    if (!currentWidget) {
        if (toolBoxProperty == PropertyCurrentItemIcon)
            return  qVariantFromValue(qdesigner_internal::PropertySheetIconValue());
        if (toolBoxProperty == PropertyCurrentItemText)
            return  qVariantFromValue(qdesigner_internal::PropertySheetStringValue());
        if (toolBoxProperty == PropertyCurrentItemToolTip)
            return  qVariantFromValue(qdesigner_internal::PropertySheetStringValue());
        return QVariant(QString());
    }

    // index-dependent
    switch (toolBoxProperty) {
    case PropertyCurrentItemText:
        return qVariantFromValue(m_pageToData.value(currentWidget).text);
    case PropertyCurrentItemName:
        return currentWidget->objectName();
    case PropertyCurrentItemIcon:
        return qVariantFromValue(m_pageToData.value(currentWidget).icon);
    case PropertyCurrentItemToolTip:
        return qVariantFromValue(m_pageToData.value(currentWidget).tooltip);
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
    return QVariant();
}

bool QToolBoxWidgetPropertySheet::reset(int index)
{
    const ToolBoxProperty toolBoxProperty = toolBoxPropertyFromName(propertyName(index));
    // independent of index
    switch (toolBoxProperty) {
    case PropertyTabSpacing:
        setProperty(index, QVariant(tabSpacingDefault));
        return true;
    case PropertyToolBoxNone:
        return QDesignerPropertySheet::reset(index);
    default:
        break;
    }
    // index-dependent
    QWidget *currentWidget = m_toolBox->currentWidget();
    if (!currentWidget)
        return false;

    // index-dependent
    switch (toolBoxProperty) {
    case PropertyCurrentItemName:
        setProperty(index, QString());
        break;
    case PropertyCurrentItemToolTip:
        m_pageToData[currentWidget].tooltip = qdesigner_internal::PropertySheetStringValue();
        setProperty(index, QString());
        break;
    case PropertyCurrentItemText:
        m_pageToData[currentWidget].text = qdesigner_internal::PropertySheetStringValue();
        setProperty(index, QString());
        break;
    case PropertyCurrentItemIcon:
        m_pageToData[currentWidget].icon = qdesigner_internal::PropertySheetIconValue();
        setProperty(index, QIcon());
        break;
    case PropertyTabSpacing:
    case PropertyToolBoxNone:
        break;
    }
    return true;
}

bool QToolBoxWidgetPropertySheet::checkProperty(const QString &propertyName)
{
    switch (toolBoxPropertyFromName(propertyName)) {
    case PropertyCurrentItemText:
    case PropertyCurrentItemName:
    case PropertyCurrentItemToolTip:
    case PropertyCurrentItemIcon:
        return false;
    default:
        break;
    }
    return true;
}

QT_END_NAMESPACE
