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

#ifndef QDESIGNER_MENU_H
#define QDESIGNER_MENU_H

#include "shared_global_p.h"

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QPixmap>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

class QTimer;
class QLineEdit;

class QDesignerFormWindowInterface;
class QDesignerActionProviderExtension;
class QDesignerMenu;
class QDesignerMenuBar;
class QPainter;
class QMimeData;

namespace qdesigner_internal {
    class CreateSubmenuCommand;
    class ActionInsertionCommand;
}

class QDESIGNER_SHARED_EXPORT QDesignerMenu: public QMenu
{
    Q_OBJECT
public:
    QDesignerMenu(QWidget *parent = 0);
    virtual ~QDesignerMenu();

    bool eventFilter(QObject *object, QEvent *event);

    QDesignerFormWindowInterface *formWindow() const;
    QDesignerActionProviderExtension *actionProvider();

    QDesignerMenu *parentMenu() const;
    QDesignerMenuBar *parentMenuBar() const;

    virtual void setVisible(bool visible);

    void adjustSpecialActions();

    bool interactive(bool i);
    void createRealMenuAction(QAction *action);
    void removeRealMenu(QAction *action);

    static void drawSelection(QPainter *p, const QRect &r);

    bool dragging() const;

    void closeMenuChain();

    void moveLeft();
    void moveRight();
    void moveUp(bool ctrl);
    void moveDown(bool ctrl);

    // Helper for MenuTaskMenu extension
    void deleteAction(QAction *a);

private slots:
    void slotAddSeparator();
    void slotRemoveSelectedAction();
    void slotShowSubMenuNow();
    void slotDeactivateNow();
    void slotAdjustSizeNow();

protected:
    virtual void actionEvent(QActionEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void showEvent(QShowEvent *event);

    bool handleEvent(QWidget *widget, QEvent *event);
    bool handleMouseDoubleClickEvent(QWidget *widget, QMouseEvent *event);
    bool handleMousePressEvent(QWidget *widget, QMouseEvent *event);
    bool handleMouseReleaseEvent(QWidget *widget, QMouseEvent *event);
    bool handleMouseMoveEvent(QWidget *widget, QMouseEvent *event);
    bool handleContextMenuEvent(QWidget *widget, QContextMenuEvent *event);
    bool handleKeyPressEvent(QWidget *widget, QKeyEvent *event);

    void startDrag(const QPoint &pos, Qt::KeyboardModifiers modifiers);

    void adjustIndicator(const QPoint &pos);
    int findAction(const QPoint &pos) const;

    QAction *currentAction() const;
    int realActionCount() const;
    enum ActionDragCheck { NoActionDrag, ActionDragOnSubMenu, AcceptActionDrag };
    ActionDragCheck checkAction(QAction *action) const;

    void showSubMenu(QAction *action);

    enum LeaveEditMode {
        Default = 0,
        ForceAccept
    };

    void enterEditMode();
    void leaveEditMode(LeaveEditMode mode);
    void showLineEdit();

    QAction *createAction(const QString &text, bool separator = false);
    QDesignerMenu *findOrCreateSubMenu(QAction *action);

    QAction *safeActionAt(int index) const;
    QAction *safeMenuAction(QDesignerMenu *menu) const;
    bool swap(int a, int b);

    void hideSubMenu();
    void deleteAction();
    void deactivateMenu();

    bool canCreateSubMenu(QAction *action) const;
    QDesignerMenu *findRootMenu() const;
    QDesignerMenu *findActivatedMenu() const;

    QRect subMenuPixmapRect(QAction *action) const;
    bool hasSubMenuPixmap(QAction *action) const;

    void selectCurrentAction();

private:
    bool hideSubMenuOnCursorKey();
    bool showSubMenuOnCursorKey();
    const QPixmap m_subMenuPixmap;

    QPoint m_startPosition;
    int m_currentIndex;
    QAction *m_addItem;
    QAction *m_addSeparator;
    QHash<QAction*, QDesignerMenu*> m_subMenus;
    QTimer *m_showSubMenuTimer;
    QTimer *m_deactivateWindowTimer;
    QTimer *m_adjustSizeTimer;
    bool m_interactive;
    QLineEdit *m_editor;
    bool m_dragging;
    int m_lastSubMenuIndex;

    friend class qdesigner_internal::CreateSubmenuCommand;
    friend class qdesigner_internal::ActionInsertionCommand;
};

QT_END_NAMESPACE

#endif // QDESIGNER_MENU_H
