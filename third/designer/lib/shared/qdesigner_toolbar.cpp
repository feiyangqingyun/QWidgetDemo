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

#include "qdesigner_toolbar_p.h"
#include "qdesigner_command_p.h"
#include "actionrepository_p.h"
#include "actionprovider_p.h"
#include "qdesigner_utils_p.h"
#include "qdesigner_objectinspector_p.h"
#include "promotiontaskmenu_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <actionprovider_p.h>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerWidgetFactoryInterface>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtGui/qevent.h>
#include <QtGui/QApplication>
#include <QtCore/QDebug>

Q_DECLARE_METATYPE(QAction*)

QT_BEGIN_NAMESPACE

typedef QList<QAction*> ActionList;

namespace qdesigner_internal {
// ------------------- ToolBarEventFilter
void ToolBarEventFilter::install(QToolBar *tb)
{
    ToolBarEventFilter *tf = new ToolBarEventFilter(tb);
    tb->installEventFilter(tf);
    tb->setAcceptDrops(true); // ### fake
}

ToolBarEventFilter::ToolBarEventFilter(QToolBar *tb) :
    QObject(tb),
    m_toolBar(tb),
    m_promotionTaskMenu(0)
{
}

ToolBarEventFilter *ToolBarEventFilter::eventFilterOf(const QToolBar *tb)
{
    // Look for 1st order children only..otherwise, we might get filters of nested widgets
    const QObjectList children = tb->children();
    const QObjectList::const_iterator cend = children.constEnd();
    for (QObjectList::const_iterator it = children.constBegin(); it != cend; ++it) {
        QObject *o = *it;
        if (!o->isWidgetType())
            if (ToolBarEventFilter *ef = qobject_cast<ToolBarEventFilter *>(o))
                return ef;
    }
    return 0;
}

bool ToolBarEventFilter::eventFilter (QObject *watched, QEvent *event)
{
    if (watched != m_toolBar)
        return QObject::eventFilter (watched, event);

    switch (event->type()) {
    case QEvent::ChildAdded: {
        // Children should not interact with the mouse
        const QChildEvent *ce = static_cast<const QChildEvent *>(event);
        if (QWidget *w = qobject_cast<QWidget *>(ce->child())) {
            w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            w->setFocusPolicy(Qt::NoFocus);
        }
    }
        break;
    case QEvent::ContextMenu:
        return handleContextMenuEvent(static_cast<QContextMenuEvent*>(event));
    case QEvent::DragEnter:
    case QEvent::DragMove:
        return handleDragEnterMoveEvent(static_cast<QDragMoveEvent *>(event));
    case QEvent::DragLeave:
        return handleDragLeaveEvent(static_cast<QDragLeaveEvent *>(event));
    case QEvent::Drop:
        return handleDropEvent(static_cast<QDropEvent *>(event));
    case QEvent::MouseButtonPress:
        return handleMousePressEvent(static_cast<QMouseEvent*>(event));
    case QEvent::MouseButtonRelease:
        return handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
    case QEvent::MouseMove:
        return handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
    default:
        break;
    }
    return QObject::eventFilter (watched, event);
}

ActionList ToolBarEventFilter::contextMenuActions(const QPoint &globalPos)
{
    ActionList rc;
    const int index = actionIndexAt(m_toolBar, m_toolBar->mapFromGlobal(globalPos), m_toolBar->orientation());
    const ActionList actions = m_toolBar->actions();
    QAction *action = index != -1 ?actions.at(index) : 0;
    QVariant itemData;

    // Insert before
    if (action && index != 0 && !action->isSeparator()) {
        QAction *newSeperatorAct = new QAction(tr("Insert Separator before '%1'").arg(action->objectName()), 0);
        qVariantSetValue(itemData, action);
        newSeperatorAct->setData(itemData);
        connect(newSeperatorAct, SIGNAL(triggered()), this, SLOT(slotInsertSeparator()));
        rc.push_back(newSeperatorAct);
    }

    // Append separator
    if (actions.empty() || !actions.back()->isSeparator()) {
        QAction *newSeperatorAct = new QAction(tr("Append Separator"), 0);
        qVariantSetValue(itemData, static_cast<QAction*>(0));
        newSeperatorAct->setData(itemData);
        connect(newSeperatorAct, SIGNAL(triggered()), this, SLOT(slotInsertSeparator()));
        rc.push_back(newSeperatorAct);
    }
    // Promotion
    if (!m_promotionTaskMenu)
        m_promotionTaskMenu = new PromotionTaskMenu(m_toolBar, PromotionTaskMenu::ModeSingleWidget, this);
    m_promotionTaskMenu->addActions(formWindow(), PromotionTaskMenu::LeadingSeparator|PromotionTaskMenu::TrailingSeparator, rc);
    // Remove
    if (action) {
        QAction *a = new QAction(tr("Remove action '%1'").arg(action->objectName()), 0);
        qVariantSetValue(itemData, action);
        a->setData(itemData);
        connect(a, SIGNAL(triggered()), this, SLOT(slotRemoveSelectedAction()));
        rc.push_back(a);
    }

    QAction *remove_toolbar = new QAction(tr("Remove Toolbar '%1'").arg(m_toolBar->objectName()), 0);
    connect(remove_toolbar, SIGNAL(triggered()), this, SLOT(slotRemoveToolBar()));
    rc.push_back(remove_toolbar);
    return rc;
}

bool ToolBarEventFilter::handleContextMenuEvent(QContextMenuEvent * event )
{
    event->accept();

    const QPoint globalPos = event->globalPos();
    const ActionList al = contextMenuActions(event->globalPos());

    QMenu menu(0);
    const ActionList::const_iterator acend = al.constEnd();
    for (ActionList::const_iterator it = al.constBegin(); it != acend; ++it)
        menu.addAction(*it);
    menu.exec(globalPos);
    return true;
}

void ToolBarEventFilter::slotRemoveSelectedAction()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QAction *a = qvariant_cast<QAction*>(action->data());
    Q_ASSERT(a != 0);

    QDesignerFormWindowInterface *fw = formWindow();
    Q_ASSERT(fw);

    const ActionList actions = m_toolBar->actions();
    const int pos = actions.indexOf(a);
    QAction *action_before = 0;
    if (pos != -1 && actions.count() > pos + 1)
        action_before = actions.at(pos + 1);

    RemoveActionFromCommand *cmd = new RemoveActionFromCommand(fw);
    cmd->init(m_toolBar, a, action_before);
    fw->commandHistory()->push(cmd);
}

void ToolBarEventFilter::slotRemoveToolBar()
{
    QDesignerFormWindowInterface *fw = formWindow();
    Q_ASSERT(fw);
    DeleteToolBarCommand *cmd = new DeleteToolBarCommand(fw);
    cmd->init(m_toolBar);
    fw->commandHistory()->push(cmd);
}

void ToolBarEventFilter::slotInsertSeparator()
{
    QDesignerFormWindowInterface *fw = formWindow();
    QAction *theSender = qobject_cast<QAction*>(sender());
    QAction *previous = qvariant_cast<QAction *>(theSender->data());
    fw->beginCommand(tr("Insert Separator"));
    QAction *action = createAction(fw, QLatin1String("separator"), true);
    InsertActionIntoCommand *cmd = new InsertActionIntoCommand(fw);
    cmd->init(m_toolBar, action, previous);
    fw->commandHistory()->push(cmd);
    fw->endCommand();
}

QDesignerFormWindowInterface *ToolBarEventFilter::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(m_toolBar);
}

QAction *ToolBarEventFilter::createAction(QDesignerFormWindowInterface *fw, const QString &objectName, bool separator)
{
    QAction *action = new QAction(fw);
    fw->core()->widgetFactory()->initialize(action);
    if (separator)
        action->setSeparator(true);

    action->setObjectName(objectName);
    fw->ensureUniqueObjectName(action);

    qdesigner_internal::AddActionCommand *cmd = new  qdesigner_internal::AddActionCommand(fw);
    cmd->init(action);
    fw->commandHistory()->push(cmd);

    return action;
}

void ToolBarEventFilter::adjustDragIndicator(const QPoint &pos)
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QDesignerFormEditorInterface *core = fw->core();
        if (QDesignerActionProviderExtension *a = qt_extension<QDesignerActionProviderExtension*>(core->extensionManager(), m_toolBar))
            a->adjustIndicator(pos);
    }
}

void ToolBarEventFilter::hideDragIndicator()
{
    adjustDragIndicator(QPoint(-1, -1));
}

bool ToolBarEventFilter::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || withinHandleArea(m_toolBar, event->pos()))
        return false;

    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QDesignerFormEditorInterface *core = fw->core();
        // Keep selection in sync
        fw->clearSelection(false);
        if (QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core->objectInspector())) {
            oi->clearSelection();
            oi->selectObject(m_toolBar);
        }
        core->propertyEditor()->setObject(m_toolBar);
    }
    m_startPosition = m_toolBar->mapFromGlobal(event->globalPos());
    event->accept();
    return true;
}

bool ToolBarEventFilter::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || m_startPosition.isNull() || withinHandleArea(m_toolBar, event->pos()))
        return false;

    // Accept the event, otherwise, form window selection will trigger
    m_startPosition = QPoint();
    event->accept();
    return true;
}

bool ToolBarEventFilter::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_startPosition.isNull() || withinHandleArea(m_toolBar, event->pos()))
        return false;

    const QPoint pos = m_toolBar->mapFromGlobal(event->globalPos());
    if ((pos - m_startPosition).manhattanLength() > qApp->startDragDistance()) {
        startDrag(m_startPosition, event->modifiers());
        m_startPosition = QPoint();
        event->accept();
        return true;
    }
    return false;
}

bool ToolBarEventFilter::handleDragEnterMoveEvent(QDragMoveEvent *event)
{
    const ActionRepositoryMimeData *d = qobject_cast<const ActionRepositoryMimeData*>(event->mimeData());
    if (!d)
        return false;

    if (d->actionList().isEmpty()) {
        event->ignore();
        hideDragIndicator();
        return true;
    }

    QAction *action = d->actionList().first();
    if (!action || action->menu() || m_toolBar->actions().contains(action) || !Utils::isObjectAncestorOf(formWindow()->mainContainer(), action)) {
        event->ignore();
        hideDragIndicator();
        return true;
    }

    d->accept(event);
    adjustDragIndicator(event->pos());
    return true;
}

bool ToolBarEventFilter::handleDragLeaveEvent(QDragLeaveEvent *)
{
    hideDragIndicator();
    return false;
}

bool ToolBarEventFilter::handleDropEvent(QDropEvent *event)
{
    const ActionRepositoryMimeData *d = qobject_cast<const ActionRepositoryMimeData*>(event->mimeData());
    if (!d)
        return false;

    if (d->actionList().isEmpty()) {
        event->ignore();
        hideDragIndicator();
        return true;
    }

    QAction *action = d->actionList().first();

    const ActionList actions = m_toolBar->actions();
    if (!action || actions.contains(action)) {
        event->ignore();
        hideDragIndicator();
        return true;
    }

    // Try to find action to 'insert before'. Click on action or in free area, else ignore.
    QAction *beforeAction = 0;
    const QPoint pos = event->pos();
    const int index = actionIndexAt(m_toolBar, pos, m_toolBar->orientation());
    if (index != -1) {
        beforeAction = actions.at(index);
    } else {
        if (!freeArea(m_toolBar).contains(pos)) {
            event->ignore();
            hideDragIndicator();
            return true;
        }
    }

    event->acceptProposedAction();
    QDesignerFormWindowInterface *fw = formWindow();
    InsertActionIntoCommand *cmd = new InsertActionIntoCommand(fw);
    cmd->init(m_toolBar, action, beforeAction);
    fw->commandHistory()->push(cmd);
    hideDragIndicator();
    return true;
}

void ToolBarEventFilter::startDrag(const QPoint &pos, Qt::KeyboardModifiers modifiers)
{
    const int index = actionIndexAt(m_toolBar, pos, m_toolBar->orientation());
    if (index == - 1)
        return;

    const ActionList actions = m_toolBar->actions();
    QAction *action = actions.at(index);
    QDesignerFormWindowInterface *fw = formWindow();

    const Qt::DropAction dropAction = (modifiers & Qt::ControlModifier) ? Qt::CopyAction : Qt::MoveAction;
    if (dropAction == Qt::MoveAction) {
        RemoveActionFromCommand *cmd = new RemoveActionFromCommand(fw);
        const int nextIndex = index + 1;
        QAction *nextAction = nextIndex < actions.size() ? actions.at(nextIndex) : 0;
        cmd->init(m_toolBar, action, nextAction);
        fw->commandHistory()->push(cmd);
    }

    QDrag *drag = new QDrag(m_toolBar);
    drag->setPixmap(ActionRepositoryMimeData::actionDragPixmap( action));
    drag->setMimeData(new ActionRepositoryMimeData(action, dropAction));

    if (drag->start(dropAction) == Qt::IgnoreAction) {
        hideDragIndicator();
        if (dropAction == Qt::MoveAction) {
            const ActionList currentActions = m_toolBar->actions();
            QAction *previous = 0;
            if (index >= 0 && index < currentActions.size())
                previous = currentActions.at(index);
            InsertActionIntoCommand *cmd = new InsertActionIntoCommand(fw);
            cmd->init(m_toolBar, action, previous);
            fw->commandHistory()->push(cmd);
        }
    }
}

QAction *ToolBarEventFilter::actionAt(const QToolBar *tb, const QPoint &pos)
{
    const int index = actionIndexAt(tb, pos, tb->orientation());
    if (index == -1)
        return 0;
    return tb->actions().at(index);
}

//that's a trick to get access to the initStyleOption which is a protected member
class FriendlyToolBar : public QToolBar {
public:
    friend class ToolBarEventFilter;
};

QRect ToolBarEventFilter::handleArea(const QToolBar *tb)
{
    QStyleOptionToolBar opt;
    static_cast<const FriendlyToolBar*>(tb)->initStyleOption(&opt);
    return tb->style()->subElementRect(QStyle::SE_ToolBarHandle, &opt, tb);
}

bool ToolBarEventFilter::withinHandleArea(const QToolBar *tb, const QPoint &pos)
{
    return handleArea(tb).contains(pos);
}

// Determine the free area behind the last action.
QRect ToolBarEventFilter::freeArea(const QToolBar *tb)
{
    QRect rc = QRect(QPoint(0, 0), tb->size());
    const ActionList actionList = tb->actions();
    QRect exclusionRectangle = actionList.empty() ? handleArea(tb) : tb->actionGeometry(actionList.back());
    switch (tb->orientation()) {
    case Qt::Horizontal:
        switch (tb->layoutDirection()) {
        case Qt::LayoutDirectionAuto: // Should never happen
        case Qt::LeftToRight:
            rc.setX(exclusionRectangle.right() + 1);
            break;
        case Qt::RightToLeft:
            rc.setRight(exclusionRectangle.x());
            break;
        }
        break;
    case Qt::Vertical:
        rc.setY(exclusionRectangle.bottom() + 1);
        break;
    }
    return rc;
}

}

QT_END_NAMESPACE
