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

#include "qdesigner_menubar_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "actionrepository_p.h"
#include "actionprovider_p.h"
#include "actioneditor_p.h"
#include "qdesigner_utils_p.h"
#include "promotiontaskmenu_p.h"
#include "qdesigner_objectinspector_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QExtensionManager>

#include <QtCore/QMimeData>

#include <QtCore/qdebug.h>

#include <QtGui/QApplication>
#include <QtGui/QDrag>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/qevent.h>

Q_DECLARE_METATYPE(QAction*)

QT_BEGIN_NAMESPACE

typedef QList<QAction *> ActionList;

using namespace qdesigner_internal;

namespace qdesigner_internal
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////
SpecialMenuAction::SpecialMenuAction(QObject *parent)
    : QAction(parent)
{
}

SpecialMenuAction::~SpecialMenuAction()
{
}


} // namespace qdesigner_internal


/////////////////////////////////////////////////////////////////////////////////////////////////////////
QDesignerMenuBar::QDesignerMenuBar(QWidget *parent)  :
    QMenuBar(parent),
    m_addMenu(new SpecialMenuAction(this)),
    m_currentIndex(0),
    m_interactive(true),
    m_editor(new QLineEdit(this)),
    m_dragging(false),
    m_lastMenuActionIndex( -1),
    m_promotionTaskMenu(new PromotionTaskMenu(this, PromotionTaskMenu::ModeSingleWidget, this))
{
    setContextMenuPolicy(Qt::DefaultContextMenu);

    setAcceptDrops(true); // ### fake
    // Fake property: Keep the menu bar editable in the form even if a native menu bar is used.
    setNativeMenuBar(false);

    m_addMenu->setText(tr("Type Here"));
    addAction(m_addMenu);

    QFont italic;
    italic.setItalic(true);
    m_addMenu->setFont(italic);

    m_editor->setObjectName(QLatin1String("__qt__passive_editor"));
    m_editor->hide();
    m_editor->installEventFilter(this);
    installEventFilter(this);
}

QDesignerMenuBar::~QDesignerMenuBar()
{
}

void QDesignerMenuBar::paintEvent(QPaintEvent *event)
{
    QMenuBar::paintEvent(event);

    QPainter p(this);

    foreach (QAction *a, actions()) {
        if (qobject_cast<SpecialMenuAction*>(a)) {
            const QRect g = actionGeometry(a);
            QLinearGradient lg(g.left(), g.top(), g.left(), g.bottom());
            lg.setColorAt(0.0, Qt::transparent);
            lg.setColorAt(0.7, QColor(0, 0, 0, 32));
            lg.setColorAt(1.0, Qt::transparent);

            p.fillRect(g, lg);
        }
    }

    QAction *action = currentAction();

    if (m_dragging || !action)
        return;

    if (hasFocus()) {
        const QRect g = actionGeometry(action);
        QDesignerMenu::drawSelection(&p, g.adjusted(1, 1, -1, -1));
    } else if (action->menu() && action->menu()->isVisible()) {
        const QRect g = actionGeometry(action);
        p.drawRect(g.adjusted(1, 1, -1, -1));
    }
}

bool QDesignerMenuBar::handleEvent(QWidget *widget, QEvent *event)
{
    if (!formWindow())
        return false;

    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)
        update();

    switch (event->type()) {
        default: break;

        case QEvent::MouseButtonDblClick:
            return handleMouseDoubleClickEvent(widget, static_cast<QMouseEvent*>(event));
        case QEvent::MouseButtonPress:
            return handleMousePressEvent(widget, static_cast<QMouseEvent*>(event));
        case QEvent::MouseButtonRelease:
            return handleMouseReleaseEvent(widget, static_cast<QMouseEvent*>(event));
        case QEvent::MouseMove:
            return handleMouseMoveEvent(widget, static_cast<QMouseEvent*>(event));
        case QEvent::ContextMenu:
            return handleContextMenuEvent(widget, static_cast<QContextMenuEvent*>(event));
        case QEvent::KeyPress:
            return handleKeyPressEvent(widget, static_cast<QKeyEvent*>(event));
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            return widget != m_editor;
    }

    return true;
}

bool QDesignerMenuBar::handleMouseDoubleClickEvent(QWidget *, QMouseEvent *event)
{
    if (!rect().contains(event->pos()))
        return true;

    if ((event->buttons() & Qt::LeftButton) != Qt::LeftButton)
        return true;

    event->accept();

    m_startPosition = QPoint();

    m_currentIndex = actionIndexAt(this, event->pos(), Qt::Horizontal);
    if (m_currentIndex != -1) {
        showLineEdit();
    }

    return true;
}

bool QDesignerMenuBar::handleKeyPressEvent(QWidget *, QKeyEvent *e)
{
    if (m_editor->isHidden()) { // In navigation mode
        switch (e->key()) {

        case Qt::Key_Delete:
            if (m_currentIndex == -1 || m_currentIndex >= realActionCount())
                break;
            hideMenu();
            deleteMenu();
            break;

        case Qt::Key_Left:
            e->accept();
            moveLeft(e->modifiers() & Qt::ControlModifier);
            return true;

        case Qt::Key_Right:
            e->accept();
            moveRight(e->modifiers() & Qt::ControlModifier);
            return true; // no update

        case Qt::Key_Up:
            e->accept();
            moveUp();
            return true;

        case Qt::Key_Down:
            e->accept();
            moveDown();
            return true;

        case Qt::Key_PageUp:
            m_currentIndex = 0;
            break;

        case Qt::Key_PageDown:
            m_currentIndex = actions().count() - 1;
            break;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            e->accept();
            enterEditMode();
            return true; // no update

        case Qt::Key_Alt:
        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Escape:
            e->ignore();
            setFocus(); // FIXME: this is because some other widget get the focus when CTRL is pressed
            return true; // no update

        default:
            if (!e->text().isEmpty() && e->text().at(0).toLatin1() >= 32) {
                showLineEdit();
                QApplication::sendEvent(m_editor, e);
                e->accept();
            } else {
                e->ignore();
            }
            return true;
        }
    } else { // In edit mode
        switch (e->key()) {
        default:
            return false;

        case Qt::Key_Control:
            e->ignore();
            return true;

        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (!m_editor->text().isEmpty()) {
                leaveEditMode(ForceAccept);
                if (m_lastFocusWidget)
                    m_lastFocusWidget->setFocus();

                m_editor->hide();
                showMenu();
                break;
            }
            // fall through

        case Qt::Key_Escape:
            update();
            setFocus();
            break;
        }
    }

    e->accept();
    update();

    return true;
}

void QDesignerMenuBar::startDrag(const QPoint &pos)
{
    const int index = findAction(pos);
    if (m_currentIndex == -1 || index >= realActionCount())
        return;

    QAction *action = safeActionAt(index);

    QDesignerFormWindowInterface *fw = formWindow();
    RemoveActionFromCommand *cmd = new RemoveActionFromCommand(fw);
    cmd->init(this, action, actions().at(index + 1));
    fw->commandHistory()->push(cmd);

    adjustSize();

    hideMenu(index);

    QDrag *drag = new QDrag(this);
    drag->setPixmap(ActionRepositoryMimeData::actionDragPixmap(action));
    drag->setMimeData(new ActionRepositoryMimeData(action, Qt::MoveAction));

    const int old_index = m_currentIndex;
    m_currentIndex = -1;

    if (drag->start(Qt::MoveAction) == Qt::IgnoreAction) {
        InsertActionIntoCommand *cmd = new InsertActionIntoCommand(fw);
        cmd->init(this, action, safeActionAt(index));
        fw->commandHistory()->push(cmd);

        m_currentIndex = old_index;
        adjustSize();
    }
}

bool QDesignerMenuBar::handleMousePressEvent(QWidget *, QMouseEvent *event)
{
    m_startPosition = QPoint();
    event->accept();

    if (event->button() != Qt::LeftButton)
        return true;

    m_startPosition = event->pos();
    const int newIndex = actionIndexAt(this, m_startPosition, Qt::Horizontal);
    const bool changed = newIndex != m_currentIndex;
    m_currentIndex =  newIndex;
    updateCurrentAction(changed);

    return true;
}

bool QDesignerMenuBar::handleMouseReleaseEvent(QWidget *, QMouseEvent *event)
{
    m_startPosition = QPoint();

    if (event->button() != Qt::LeftButton)
        return true;

    event->accept();
    m_currentIndex = actionIndexAt(this, event->pos(), Qt::Horizontal);
    if (!m_editor->isVisible() && m_currentIndex != -1 && m_currentIndex < realActionCount())
        showMenu();

    return true;
}

bool QDesignerMenuBar::handleMouseMoveEvent(QWidget *, QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) != Qt::LeftButton)
        return true;

    if (m_startPosition.isNull())
        return true;

    const QPoint pos = mapFromGlobal(event->globalPos());

    if ((pos - m_startPosition).manhattanLength() < qApp->startDragDistance())
        return true;

    const int index =  actionIndexAt(this, m_startPosition, Qt::Horizontal);
    if (index < actions().count()) {
        hideMenu(index);
        update();
    }

    startDrag(m_startPosition);
    m_startPosition = QPoint();

    return true;
}

ActionList QDesignerMenuBar::contextMenuActions()
{
    ActionList rc;
    if (QAction *action = safeActionAt(m_currentIndex)) {
        if (!qobject_cast<SpecialMenuAction*>(action)) {
            QVariant itemData;
            qVariantSetValue(itemData, action);

            QAction *remove_action = new QAction(tr("Remove Menu '%1'").arg(action->menu()->objectName()), 0);
            remove_action->setData(itemData);
            connect(remove_action, SIGNAL(triggered()), this, SLOT(deleteMenu()));
            rc.push_back(remove_action);
            QAction *sep = new QAction(0);
            sep->setSeparator(true);
            rc.push_back(sep);
        }
    }

    m_promotionTaskMenu->addActions(formWindow(), PromotionTaskMenu::TrailingSeparator, rc);

    QAction *remove_menubar = new QAction(tr("Remove Menu Bar"), 0);
    connect(remove_menubar, SIGNAL(triggered()), this, SLOT(slotRemoveMenuBar()));
    rc.push_back(remove_menubar);
    return rc;
}

bool QDesignerMenuBar::handleContextMenuEvent(QWidget *, QContextMenuEvent *event)
{
    event->accept();

    m_currentIndex = actionIndexAt(this, mapFromGlobal(event->globalPos()), Qt::Horizontal);

    update();

    QMenu menu;
    const ActionList al = contextMenuActions();
    const ActionList::const_iterator acend = al.constEnd();
    for (ActionList::const_iterator it =  al.constBegin(); it != acend; ++it)
        menu.addAction(*it);
    menu.exec(event->globalPos());
    return true;
}

void QDesignerMenuBar::slotRemoveMenuBar()
{
    Q_ASSERT(formWindow() != 0);

    QDesignerFormWindowInterface *fw = formWindow();

    DeleteMenuBarCommand *cmd = new DeleteMenuBarCommand(fw);
    cmd->init(this);
    fw->commandHistory()->push(cmd);
}

void QDesignerMenuBar::focusOutEvent(QFocusEvent *event)
{
    QMenuBar::focusOutEvent(event);
}

void QDesignerMenuBar::enterEditMode()
{
    if (m_currentIndex >= 0 && m_currentIndex <= realActionCount()) {
        showLineEdit();
    }
}

void QDesignerMenuBar::leaveEditMode(LeaveEditMode mode)
{
    m_editor->releaseKeyboard();

    if (mode == Default)
        return;

    if (m_editor->text().isEmpty())
        return;

    QAction *action = 0;

    QDesignerFormWindowInterface *fw = formWindow();
    Q_ASSERT(fw);

    if (m_currentIndex >= 0 && m_currentIndex < realActionCount()) {
        action = safeActionAt(m_currentIndex);
        fw->beginCommand(QApplication::translate("Command", "Change Title"));
    } else {
        fw->beginCommand(QApplication::translate("Command", "Insert Menu"));
        const QString niceObjectName = ActionEditor::actionTextToName(m_editor->text(), QLatin1String("menu"));
        QMenu *menu = qobject_cast<QMenu*>(fw->core()->widgetFactory()->createWidget(QLatin1String("QMenu"), this));
        fw->core()->widgetFactory()->initialize(menu);
        menu->setObjectName(niceObjectName);
        menu->setTitle(tr("Menu"));
        fw->ensureUniqueObjectName(menu);
        action = menu->menuAction();
        AddMenuActionCommand *cmd = new AddMenuActionCommand(fw);
        cmd->init(action, m_addMenu, this, this);
        fw->commandHistory()->push(cmd);
    }

    SetPropertyCommand *cmd = new SetPropertyCommand(fw);
    cmd->init(action, QLatin1String("text"), m_editor->text());
    fw->commandHistory()->push(cmd);
    fw->endCommand();
}

void QDesignerMenuBar::showLineEdit()
{
    QAction *action = 0;

    if (m_currentIndex >= 0 && m_currentIndex < realActionCount())
        action = safeActionAt(m_currentIndex);
    else
        action = m_addMenu;

    if (action->isSeparator())
        return;

    // hideMenu();

    m_lastFocusWidget = qApp->focusWidget();

    // open edit field for item name
    const QString text = action != m_addMenu ? action->text() : QString();

    m_editor->setText(text);
    m_editor->selectAll();
    m_editor->setGeometry(actionGeometry(action));
    m_editor->show();
    qApp->setActiveWindow(m_editor);
    m_editor->setFocus();
    m_editor->grabKeyboard();
}

bool QDesignerMenuBar::eventFilter(QObject *object, QEvent *event)
{
    if (object != this && object != m_editor)
        return false;

    if (!m_editor->isHidden() && object == m_editor && event->type() == QEvent::FocusOut) {
        leaveEditMode(Default);
        m_editor->hide();
        update();
        return true;
    }

    bool dispatch = true;

    switch (event->type()) {
        default: break;

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        case QEvent::ContextMenu:
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            dispatch = (object != m_editor);
            // no break

        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        {
            QWidget *widget = qobject_cast<QWidget*>(object);

            if (dispatch && widget && (widget == this || isAncestorOf(widget)))
                return handleEvent(widget, event);
        } break;

        case QEvent::Shortcut:
            event->accept();
            return true;
    }

    return false;
};

int QDesignerMenuBar::findAction(const QPoint &pos) const
{
    const int index = actionIndexAt(this, pos, Qt::Horizontal);
    if (index == -1)
        return realActionCount();

    return index;
}

void QDesignerMenuBar::adjustIndicator(const QPoint &pos)
{
    const int index = findAction(pos);
    QAction *action = safeActionAt(index);
    Q_ASSERT(action != 0);

    if (pos != QPoint(-1, -1)) {
        QDesignerMenu *m = qobject_cast<QDesignerMenu*>(action->menu());
        if (!m || m->parentMenu()) {
            m_currentIndex = index;
            showMenu(index);
        }
    }

    if (QDesignerActionProviderExtension *a = actionProvider()) {
        a->adjustIndicator(pos);
    }
}

QDesignerMenuBar::ActionDragCheck QDesignerMenuBar::checkAction(QAction *action) const
{
    // action belongs to another form
    if (!action || !Utils::isObjectAncestorOf(formWindow()->mainContainer(), action))
        return NoActionDrag;

    if (!action->menu())
        return ActionDragOnSubMenu; // simple action only on sub menus

    QDesignerMenu *m = qobject_cast<QDesignerMenu*>(action->menu());
    if (m && m->parentMenu())
        return ActionDragOnSubMenu; // it looks like a submenu

    if (actions().contains(action))
        return ActionDragOnSubMenu; // we already have the action in the menubar

    return AcceptActionDrag;
}

void QDesignerMenuBar::dragEnterEvent(QDragEnterEvent *event)
{
    const ActionRepositoryMimeData *d = qobject_cast<const ActionRepositoryMimeData*>(event->mimeData());
    if (!d || d->actionList().empty()) {
        event->ignore();
        return;
    }

    QAction *action = d->actionList().first();
    switch (checkAction(action)) {
    case NoActionDrag:
        event->ignore();
        break;
    case ActionDragOnSubMenu:
        m_dragging = true;
        d->accept(event);
        break;
    case AcceptActionDrag:
        m_dragging = true;
        d->accept(event);
        adjustIndicator(event->pos());
        break;
    }
}

void QDesignerMenuBar::dragMoveEvent(QDragMoveEvent *event)
{
    const ActionRepositoryMimeData *d = qobject_cast<const ActionRepositoryMimeData*>(event->mimeData());
    if (!d || d->actionList().empty()) {
        event->ignore();
        return;
    }
    QAction *action = d->actionList().first();

    switch (checkAction(action)) {
    case NoActionDrag:
        event->ignore();
        break;
    case ActionDragOnSubMenu:
        event->ignore();
        showMenu(findAction(event->pos()));
        break;
    case AcceptActionDrag:
        d->accept(event);
        adjustIndicator(event->pos());
        break;
    }
}

void QDesignerMenuBar::dragLeaveEvent(QDragLeaveEvent *)
{
    m_dragging = false;

    adjustIndicator(QPoint(-1, -1));
}

void QDesignerMenuBar::dropEvent(QDropEvent *event)
{
    m_dragging = false;

    if (const ActionRepositoryMimeData *d = qobject_cast<const ActionRepositoryMimeData*>(event->mimeData())) {

        QAction *action = d->actionList().first();
        if (checkAction(action) == AcceptActionDrag) {
            event->acceptProposedAction();
            int index = findAction(event->pos());
            index = qMin(index, actions().count() - 1);

            QDesignerFormWindowInterface *fw = formWindow();
            InsertActionIntoCommand *cmd = new InsertActionIntoCommand(fw);
            cmd->init(this, action, safeActionAt(index));
            fw->commandHistory()->push(cmd);

            m_currentIndex = index;
            update();
            adjustIndicator(QPoint(-1, -1));
            return;
        }
    }
    event->ignore();
}

void QDesignerMenuBar::actionEvent(QActionEvent *event)
{
    QMenuBar::actionEvent(event);
}

QDesignerFormWindowInterface *QDesignerMenuBar::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(const_cast<QDesignerMenuBar*>(this));
}

QDesignerActionProviderExtension *QDesignerMenuBar::actionProvider()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QDesignerFormEditorInterface *core = fw->core();
        return qt_extension<QDesignerActionProviderExtension*>(core->extensionManager(), this);
    }

    return 0;
}

QAction *QDesignerMenuBar::currentAction() const
{
    if (m_currentIndex < 0 || m_currentIndex >= actions().count())
        return 0;

    return safeActionAt(m_currentIndex);
}

int QDesignerMenuBar::realActionCount() const
{
    return actions().count() - 1; // 1 fake actions
}

bool QDesignerMenuBar::dragging() const
{
    return m_dragging;
}

void QDesignerMenuBar::moveLeft(bool ctrl)
{
    if (layoutDirection() == Qt::LeftToRight) {
        movePrevious(ctrl);
    } else {
        moveNext(ctrl);
    }
}

void QDesignerMenuBar::moveRight(bool ctrl)
{
    if (layoutDirection() == Qt::LeftToRight) {
        moveNext(ctrl);
    } else {
        movePrevious(ctrl);
    }
}

void QDesignerMenuBar::movePrevious(bool ctrl)
{
    const bool swapped = ctrl && swapActions(m_currentIndex, m_currentIndex - 1);
    const int newIndex = qMax(0, m_currentIndex - 1);
    // Always re-select, swapping destroys order
    if (swapped || newIndex != m_currentIndex) {
        m_currentIndex = newIndex;
        updateCurrentAction(true);
    }
}

void QDesignerMenuBar::moveNext(bool ctrl)
{
    const bool swapped = ctrl && swapActions(m_currentIndex + 1, m_currentIndex);
    const int newIndex = qMin(actions().count() - 1, m_currentIndex + 1);
    if (swapped || newIndex != m_currentIndex) {
        m_currentIndex = newIndex;
        updateCurrentAction(!ctrl);
    }
}

void QDesignerMenuBar::moveUp()
{
    update();
}

void QDesignerMenuBar::moveDown()
{
    showMenu();
}

void QDesignerMenuBar::adjustSpecialActions()
{
    removeAction(m_addMenu);
    addAction(m_addMenu);
}

bool QDesignerMenuBar::interactive(bool i)
{
    const bool old = m_interactive;
    m_interactive = i;
    return old;
}

void QDesignerMenuBar::hideMenu(int index)
{
    if (index < 0 && m_currentIndex >= 0)
        index = m_currentIndex;

    if (index < 0 || index >= realActionCount())
        return;

    QAction *action = safeActionAt(index);

    if (action && action->menu()) {
        action->menu()->hide();

        if (QDesignerMenu *menu = qobject_cast<QDesignerMenu*>(action->menu())) {
            menu->closeMenuChain();
        }
    }
}

void QDesignerMenuBar::deleteMenu()
{
    deleteMenuAction(currentAction());
}

void QDesignerMenuBar::deleteMenuAction(QAction *action)
{
    if (action && !qobject_cast<SpecialMenuAction*>(action)) {
        const int pos = actions().indexOf(action);
        QAction *action_before = 0;
        if (pos != -1)
            action_before = safeActionAt(pos + 1);

        QDesignerFormWindowInterface *fw = formWindow();
        RemoveMenuActionCommand *cmd = new RemoveMenuActionCommand(fw);
        cmd->init(action, action_before, this, this);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerMenuBar::showMenu(int index)
{
    if (index < 0 && m_currentIndex >= 0)
        index = m_currentIndex;

    if (index < 0 || index >= realActionCount())
        return;

    m_currentIndex = index;
    QAction *action = currentAction();

    if (action && action->menu()) {
        if (m_lastMenuActionIndex != -1 && m_lastMenuActionIndex != index) {
            hideMenu(m_lastMenuActionIndex);
        }

        m_lastMenuActionIndex = index;
        QMenu *menu = action->menu();
        const QRect g = actionGeometry(action);

        if (!menu->isVisible()) {
            if ((menu->windowFlags() & Qt::Popup) != Qt::Popup)
                menu->setWindowFlags(Qt::Popup);
            menu->adjustSize();
            if (layoutDirection() == Qt::LeftToRight) {
                menu->move(mapToGlobal(g.bottomLeft()));
            } else {
                // The position is not initially correct due to the unknown width,
                // causing it to overlap a bit the first time it is invoked.
                const QSize menuSize = menu->size();
                QPoint point = g.bottomRight() - QPoint(menu->width(), 0);
                menu->move(mapToGlobal(point));
            }
            menu->setFocus(Qt::MouseFocusReason);
            menu->raise();
            menu->show();
        } else {
            menu->raise();
        }
    }
}

QAction *QDesignerMenuBar::safeActionAt(int index) const
{
    if (index < 0 || index >= actions().count())
        return 0;

    return actions().at(index);
}

bool QDesignerMenuBar::swapActions(int a, int b)
{
    const int left = qMin(a, b);
    int right = qMax(a, b);

    QAction *action_a = safeActionAt(left);
    QAction *action_b = safeActionAt(right);

    if (action_a == action_b
            || !action_a
            || !action_b
            || qobject_cast<SpecialMenuAction*>(action_a)
            || qobject_cast<SpecialMenuAction*>(action_b))
        return false; // nothing to do

    right = qMin(right, realActionCount());
    if (right < 0)
        return false; // nothing to do

    formWindow()->beginCommand(QApplication::translate("Command", "Move action"));

    QAction *action_b_before = safeActionAt(right + 1);

    QDesignerFormWindowInterface *fw = formWindow();
    RemoveActionFromCommand *cmd1 = new RemoveActionFromCommand(fw);
    cmd1->init(this, action_b, action_b_before, false);
    fw->commandHistory()->push(cmd1);

    QAction *action_a_before = safeActionAt(left + 1);

    InsertActionIntoCommand *cmd2 = new InsertActionIntoCommand(fw);
    cmd2->init(this, action_b, action_a_before, false);
    fw->commandHistory()->push(cmd2);

    RemoveActionFromCommand *cmd3 = new RemoveActionFromCommand(fw);
    cmd3->init(this, action_a, action_b, false);
    fw->commandHistory()->push(cmd3);

    InsertActionIntoCommand *cmd4 = new InsertActionIntoCommand(fw);
    cmd4->init(this, action_a, action_b_before, true);
    fw->commandHistory()->push(cmd4);

    fw->endCommand();

    return true;
}

void QDesignerMenuBar::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
}

void QDesignerMenuBar::keyReleaseEvent(QKeyEvent *event)
{
    event->ignore();
}

void QDesignerMenuBar::updateCurrentAction(bool selectAction)
{
    update();

    if (!selectAction)
        return;

    QAction *action = currentAction();
    if (!action || action == m_addMenu)
        return;

    QMenu *menu = action->menu();
    if (!menu)
        return;

    QDesignerObjectInspector *oi = 0;
    if (QDesignerFormWindowInterface *fw = formWindow())
        oi = qobject_cast<QDesignerObjectInspector *>(fw->core()->objectInspector());

    if (!oi)
        return;

    oi->clearSelection();
    oi->selectObject(menu);
}

QT_END_NAMESPACE
