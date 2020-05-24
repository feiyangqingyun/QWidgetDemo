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

#include "abstractformwindowcursor.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerFormWindowCursorInterface

    \brief The QDesignerFormWindowCursorInterface class allows you to
    query and modify a form window's widget selection, and in addition
    modify the properties of all the form's widgets.

    \inmodule QtDesigner

    QDesignerFormWindowCursorInterface is a convenience class that
    provides an interface to the associated form window's text cursor;
    it provides a collection of functions that enables you to query a
    given form window's selection and change the selection's focus
    according to defined modes (MoveMode) and movements
    (MoveOperation). You can also use the interface to query the
    form's widgets and change their properties.

    The interface is not intended to be instantiated directly, but to
    provide access to the selections and widgets of \QD's current form
    windows. QDesignerFormWindowInterface always provides an
    associated cursor interface. The form window for a given widget
    can be retrieved using the static
    QDesignerFormWindowInterface::findFormWindow() functions. For
    example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindowcursor.cpp 0

    You can retrieve any of \QD's current form windows through
    \QD's \l {QDesignerFormWindowManagerInterface}{form window
    manager}.

    Once you have a form window's cursor interface, you can check if
    the form window has a selection at all using the hasSelection()
    function. You can query the form window for its total
    widgetCount() and selectedWidgetCount(). You can retrieve the
    currently selected widget (or widgets) using the current() or
    selectedWidget() functions.

    You can retrieve any of the form window's widgets using the
    widget() function, and check if a widget is selected using the
    isWidgetSelected() function. You can use the setProperty()
    function to set the selected widget's properties, and the
    setWidgetProperty() or resetWidgetProperty() functions to modify
    the properties of any given widget.

    Finally, you can change the selection by changing the text
    cursor's position() using the setPosition() and movePosition()
    functions.

    \sa QDesignerFormWindowInterface, QDesignerFormWindowManagerInterface
*/

/*!
    \enum QDesignerFormWindowCursorInterface::MoveOperation

    This enum describes the types of text cursor operation that can occur in a form window.

    \value NoMove The cursor does not move.
    \value Start  Moves the cursor to the start of the focus chain.
    \value End    Moves the cursor to the end of the focus chain.
    \value Next   Moves the cursor to the next widget in the focus chain.
    \value Prev   Moves the cursor to the previous widget in the focus chain.
    \value Left   The cursor moves to the left.
    \value Right  The cursor moves to the right.
    \value Up     The cursor moves upwards.
    \value Down   The cursor moves downwards.
*/

/*!
    \enum QDesignerFormWindowCursorInterface::MoveMode

    This enum describes the different modes that are used when the text cursor moves.

    \value MoveAnchor The anchor moves with the cursor to its new location.
    \value KeepAnchor The anchor remains at the cursor's old location.
*/

/*!
    Returns true if the specified \a widget is selected; otherwise
    returns false.
*/
bool QDesignerFormWindowCursorInterface::isWidgetSelected(QWidget *widget) const
{
    for (int index=0; index<selectedWidgetCount(); ++index) {
        if (selectedWidget(index) == widget)
            return true;
    }

    return false;
}

/*!
    \fn virtual QDesignerFormWindowCursorInterface::~QDesignerFormWindowCursorInterface()

    Destroys the cursor interface.
*/

/*!
    \fn virtual QDesignerFormWindowInterface *QDesignerFormWindowCursorInterface::formWindow() const

    Returns the form window interface associated with this cursor interface.
*/

/*!
    \fn virtual bool QDesignerFormWindowCursorInterface::movePosition(MoveOperation operation, MoveMode mode)

    Performs the given \a operation on the cursor using the specified
    \a mode, and returns true if it completed successfully; otherwise
    returns false.

    \sa position(), setPosition()
*/

/*!
    \fn virtual int QDesignerFormWindowCursorInterface::position() const

    Returns the cursor position.

    \sa setPosition(), movePosition()
*/

/*!
    \fn virtual void QDesignerFormWindowCursorInterface::setPosition(int position, MoveMode mode = MoveAnchor)

    Sets the position of the cursor to the given \a position using the
    \a mode to specify how it is moved there.

    \sa position(), movePosition()
*/

/*!
    \fn virtual QWidget *QDesignerFormWindowCursorInterface::current() const

    Returns the currently selected widget in the form window.

    \sa selectedWidget()
*/

/*!
    \fn virtual int QDesignerFormWindowCursorInterface::widgetCount() const

    Returns the number of widgets in the form window.

    \sa selectedWidgetCount()
*/

/*!
    \fn virtual QWidget *QDesignerFormWindowCursorInterface::widget(int index) const

    Returns the widget with the given \a index in the list of widgets
    in the form window.

    \sa selectedWidget()
*/

/*!
    \fn virtual bool QDesignerFormWindowCursorInterface::hasSelection() const

    Returns true if the form window contains a selection; otherwise
    returns false.
*/

/*!
    \fn virtual int QDesignerFormWindowCursorInterface::selectedWidgetCount() const

    Returns the number of selected widgets in the form window.

    \sa widgetCount()
*/

/*!
    \fn virtual QWidget *QDesignerFormWindowCursorInterface::selectedWidget(int index) const

    Returns the widget with the given \a index in the list of selected
    widgets.

    \sa current(), widget()
*/

/*!
    \fn virtual void QDesignerFormWindowCursorInterface::setProperty(const QString &name, const QVariant &value)

    Sets the property with the given \a name for the currently
    selected widget to the specified \a value.

    \sa setWidgetProperty(), resetWidgetProperty()
*/

/*!
    \fn virtual void QDesignerFormWindowCursorInterface::setWidgetProperty(QWidget *widget, const QString &name, const QVariant &value)

    Sets the property with the given \a name for the given \a widget
    to the specified \a value.

    \sa resetWidgetProperty(), setProperty()
*/

/*!
    \fn virtual void QDesignerFormWindowCursorInterface::resetWidgetProperty(QWidget *widget, const QString &name)

    Resets the property with the given \a name for the specified \a
    widget to its default value.

    \sa setProperty(), setWidgetProperty()
*/

QT_END_NAMESPACE
