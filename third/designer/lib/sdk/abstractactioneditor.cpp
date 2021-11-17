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

#include "abstractactioneditor.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerActionEditorInterface

    \brief The QDesignerActionEditorInterface class allows you to
    change the focus of Qt Designer's action editor.

    \inmodule QtDesigner

    The QDesignerActionEditorInterface class is not intended to be
    instantiated directly. You can retrieve an interface to \QD's
    action editor using the
    QDesignerFormEditorInterface::actionEditor() function.

    You can control which actions that are available in the action
    editor's window using the manageAction() and unmanageAction()
    functions. An action that is managed by \QD is available in the
    action editor while an unmanaged action is ignored.

    QDesignerActionEditorInterface also provides the core() function
    that you can use to retrieve a pointer to \QD's current
    QDesignerFormEditorInterface object, and the setFormWindow()
    function that enables you to change the currently selected form
    window.

    \sa QDesignerFormEditorInterface, QDesignerFormWindowInterface
*/

/*!
    Constructs an action editor interface with the given \a parent and
    the specified window \a flags.
*/
QDesignerActionEditorInterface::QDesignerActionEditorInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the action editor interface.
*/
QDesignerActionEditorInterface::~QDesignerActionEditorInterface()
{
}

/*!
    Returns a pointer to \QD's current QDesignerFormEditorInterface
    object.
*/
QDesignerFormEditorInterface *QDesignerActionEditorInterface::core() const
{
    return 0;
}

/*!
    \fn void QDesignerActionEditorInterface::setFormWindow(QDesignerFormWindowInterface *formWindow)

    Sets the currently selected form window to \a formWindow.

*/

/*!
    \fn void QDesignerActionEditorInterface::manageAction(QAction *action)

    Instructs \QD to manage the specified \a action. An action that is
    managed by \QD is available in the action editor.

    \sa unmanageAction()
*/

/*!
    \fn void QDesignerActionEditorInterface::unmanageAction(QAction *action)

    Instructs \QD to ignore the specified \a action. An unmanaged
    action is not available in the action editor.

    \sa manageAction()
*/

QT_END_NAMESPACE
