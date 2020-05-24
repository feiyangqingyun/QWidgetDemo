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

#include "abstractformwindowmanager.h"

#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerFormWindowManagerInterface

    \brief The QDesignerFormWindowManagerInterface class allows you to
    manipulate the collection of form windows in Qt Designer, and
    control Qt Designer's form editing actions.

    \inmodule QtDesigner

    QDesignerFormWindowManagerInterface is not intended to be
    instantiated directly. \QD uses the form window manager to
    control the various form windows in its workspace. You can
    retrieve an interface to \QD's form window manager using
    the QDesignerFormEditorInterface::formWindowManager()
    function. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindowmanager.cpp 0

    When implementing a custom widget plugin, a pointer to \QD's
    current QDesignerFormEditorInterface object (\c formEditor in the
    example above) is provided by the
    QDesignerCustomWidgetInterface::initialize() function's parameter.
    You must subclass the QDesignerCustomWidgetInterface to expose
    your plugin to Qt Designer.

    The form window manager interface provides the createFormWindow()
    function that enables you to create a new form window which you
    can add to the collection of form windows that the manager
    maintains, using the addFormWindow() slot. It also provides the
    formWindowCount() function returning the number of form windows
    currently under the manager's control, the formWindow() function
    returning the form window associated with a given index, and the
    activeFormWindow() function returning the currently selected form
    window. The removeFormWindow() slot allows you to reduce the
    number of form windows the manager must maintain, and the
    setActiveFormWindow() slot allows you to change the form window
    focus in \QD's workspace.

    In addition, QDesignerFormWindowManagerInterface contains a
    collection of functions that enables you to intervene and control
    \QD's form editing actions. All these functions return the
    original action, making it possible to propagate the function
    further after intervention.

    Finally, the interface provides three signals which are emitted
    when a form window is added, when the currently selected form
    window changes, or when a form window is removed, respectively. All
    the signals carry the form window in question as their parameter.

    \sa QDesignerFormEditorInterface, QDesignerFormWindowInterface
*/

// ------------- QDesignerFormWindowManagerInterfacePrivate

struct QDesignerFormWindowManagerInterfacePrivate {
    QDesignerFormWindowManagerInterfacePrivate();
    QAction *m_simplifyLayoutAction;
    QAction *m_formLayoutAction;
};

QDesignerFormWindowManagerInterfacePrivate::QDesignerFormWindowManagerInterfacePrivate() :
    m_simplifyLayoutAction(0),
    m_formLayoutAction(0)
{
}

typedef QMap<const QDesignerFormWindowManagerInterface *, QDesignerFormWindowManagerInterfacePrivate *> FormWindowManagerPrivateMap;

Q_GLOBAL_STATIC(FormWindowManagerPrivateMap, g_FormWindowManagerPrivateMap)

/*!
    Constructs an interface with the given \a parent for the form window
    manager.
*/
QDesignerFormWindowManagerInterface::QDesignerFormWindowManagerInterface(QObject *parent)
    : QObject(parent)
{
    g_FormWindowManagerPrivateMap()->insert(this, new QDesignerFormWindowManagerInterfacePrivate);
}

/*!
    Destroys the interface for the form window manager.
*/
QDesignerFormWindowManagerInterface::~QDesignerFormWindowManagerInterface()
{
    FormWindowManagerPrivateMap *fwmpm = g_FormWindowManagerPrivateMap();
    const FormWindowManagerPrivateMap::iterator it = fwmpm->find(this);
    Q_ASSERT(it !=  fwmpm->end());
    delete it.value();
    fwmpm->erase(it);
}

/*!
    Allows you to intervene and control \QD's "cut" action. The function
    returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionCut() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "copy" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionCopy() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "paste" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionPaste() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "delete" action. The function
    returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionDelete() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "select all" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionSelectAll() const
{
    return 0;
}

/*!
    Allows you to intervene and control the action of lowering a form
    window in \QD's workspace. The function returns the original
    action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionLower() const
{
    return 0;
}

/*!
    Allows you to intervene and control the action of raising of a
    form window in \QD's workspace. The function returns the original
    action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionRaise() const
{
    return 0;
}

/*!
    Allows you to intervene and control a request for horizontal
    layout for a form window in \QD's workspace. The function returns
    the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionHorizontalLayout() const
{
    return 0;
}

/*!
    Allows you to intervene and control a request for vertical layout
    for a form window in \QD's workspace. The function returns the
    original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionVerticalLayout() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "split horizontal"
    action. The function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionSplitHorizontal() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "split vertical"
    action. The function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionSplitVertical() const
{
    return 0;
}

/*!
    Allows you to intervene and control a request for grid layout for
    a form window in \QD's workspace. The function returns the
    original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionGridLayout() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "form layout" action. The
    function returns the original action.

FormWindowManagerPrivateMap *fwmpm = g_FormWindowManagerPrivateMap();    \sa QAction
    \since 4.4
*/

QAction *QDesignerFormWindowManagerInterface::actionFormLayout() const
{
    const QDesignerFormWindowManagerInterfacePrivate *d = g_FormWindowManagerPrivateMap()->value(this);
    Q_ASSERT(d);
    return d->m_formLayoutAction;
}

/*!
    Sets the "form layout" action to \a action.

    \internal
    \since 4.4
*/

void QDesignerFormWindowManagerInterface::setActionFormLayout(QAction *action)
{
    QDesignerFormWindowManagerInterfacePrivate *d = g_FormWindowManagerPrivateMap()->value(this);
    Q_ASSERT(d);
    d->m_formLayoutAction = action;
}

/*!
    Allows you to intervene and control \QD's "break layout" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionBreakLayout() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "adjust size" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionAdjustSize() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "simplify layout" action. The
    function returns the original action.

    \sa QAction
    \since 4.4
*/

QAction *QDesignerFormWindowManagerInterface::actionSimplifyLayout() const
{
    const QDesignerFormWindowManagerInterfacePrivate *d = g_FormWindowManagerPrivateMap()->value(this);
    Q_ASSERT(d);
    return d->m_simplifyLayoutAction;
}

/*!
    Sets the "simplify layout" action to \a action.

    \internal
    \since 4.4
*/

void QDesignerFormWindowManagerInterface::setActionSimplifyLayout(QAction *action)
{
    QDesignerFormWindowManagerInterfacePrivate *d = g_FormWindowManagerPrivateMap()->value(this);
    Q_ASSERT(d);
    d->m_simplifyLayoutAction = action;
}

/*!
   Returns the currently active form window in \QD's workspace.

   \sa setActiveFormWindow(), removeFormWindow()
*/
QDesignerFormWindowInterface *QDesignerFormWindowManagerInterface::activeFormWindow() const
{
    return 0;
}

/*!
    Returns a pointer to \QD's current QDesignerFormEditorInterface
    object.
*/
QDesignerFormEditorInterface *QDesignerFormWindowManagerInterface::core() const
{
    return 0;
}

/*!
   Adds the given \a formWindow to the collection of windows that
   \QD's form window manager maintains.

   \sa formWindowAdded()
*/
void QDesignerFormWindowManagerInterface::addFormWindow(QDesignerFormWindowInterface *formWindow)
{
    Q_UNUSED(formWindow);
}

/*!
   Removes the given \a formWindow from the collection of windows that
   \QD's form window manager maintains.

   \sa formWindow(), formWindowRemoved()
*/
void QDesignerFormWindowManagerInterface::removeFormWindow(QDesignerFormWindowInterface *formWindow)
{
    Q_UNUSED(formWindow);
}

/*!
   Sets the given \a formWindow to be the currently active form window in
   \QD's workspace.

   \sa activeFormWindow(), activeFormWindowChanged()
*/
void QDesignerFormWindowManagerInterface::setActiveFormWindow(QDesignerFormWindowInterface *formWindow)
{
    Q_UNUSED(formWindow);
}

/*!
   Returns the number of form windows maintained by \QD's form window
   manager.
*/
int QDesignerFormWindowManagerInterface::formWindowCount() const
{
    return 0;
}

/*!
   Returns the form window at the given \a index.

   \sa setActiveFormWindow(), removeFormWindow()
*/
QDesignerFormWindowInterface *QDesignerFormWindowManagerInterface::formWindow(int index) const
{
    Q_UNUSED(index);
    return 0;
}

/*!
  \fn QDesignerFormWindowInterface *QDesignerFormWindowManagerInterface::createFormWindow(QWidget *parent, Qt::WindowFlags flags)

   Creates a form window with the given \a parent and the given window
   \a flags.

   \sa addFormWindow()
*/
QDesignerFormWindowInterface *QDesignerFormWindowManagerInterface::createFormWindow(QWidget *parentWidget, Qt::WindowFlags flags)
{
    Q_UNUSED(parentWidget);
    Q_UNUSED(flags);
    return 0;
}

/*!
    Allows you to intervene and control \QD's "undo" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionUndo() const
{
    return 0;
}

/*!
    Allows you to intervene and control \QD's "redo" action. The
    function returns the original action.

    \sa QAction
*/
QAction *QDesignerFormWindowManagerInterface::actionRedo() const
{
    return 0;
}

/*!
    \fn void QDesignerFormWindowManagerInterface::formWindowAdded(QDesignerFormWindowInterface *formWindow)

    This signal is emitted when a new form window is added to the
    collection of windows that \QD's form window manager maintains. A
    pointer to the new \a formWindow is passed as an argument.

    \sa addFormWindow(), setActiveFormWindow()
*/

/*!
    \fn void QDesignerFormWindowManagerInterface::formWindowRemoved(QDesignerFormWindowInterface *formWindow)

    This signal is emitted when a form window is removed from the
    collection of windows that \QD's form window manager maintains. A
    pointer to the removed \a formWindow is passed as an argument.

    \sa removeFormWindow()
*/

/*!
    \fn void QDesignerFormWindowManagerInterface::activeFormWindowChanged(QDesignerFormWindowInterface *formWindow)

    This signal is emitted when the contents of the currently active
    form window in \QD's workspace changed. A pointer to the currently
    active \a formWindow is passed as an argument.

    \sa activeFormWindow()
*/

/*!
    \fn void QDesignerFormWindowManagerInterface::dragItems(const QList<QDesignerDnDItemInterface*> &item_list)

    \internal
*/

QT_END_NAMESPACE
