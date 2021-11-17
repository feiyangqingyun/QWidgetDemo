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

#include "script_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerScriptExtension
    \brief The QDesignerScriptExtension class allows you to generate a
    per-widget \l{QtScript} {Qt Script} snippet to be executed while
    building the form.

    \internal
    \inmodule QtDesigner
    \since 4.3

    On saving the form, the extension is queried for a script snippet
    to be associated with the widget while saving the UI file.
    This script is then run after creating the widget by \l uic or
    QUiLoader.

    As opposed to \l QDesignerCustomWidgetInterface::codeTemplate(),
    it allows for applying an internal state of the widget
    that can be manipulated using \QD.

    Such a state might for example be the contents of a custom item view widget,
    for which an editor is provided by the QDesignerTaskMenuExtension.

    While saving the form, the state is serialized as a QVariantMap of
    \QD-supported properties, which is stored in the UI file. This is
    handled by data() and setData().

    For item view contents, there might be for example a key that determines
    the number of items and other keys that contain the actual items following
    a naming scheme (\c numItems, \c item1, \c item2, ...).

    On saving, script() is invoked, which should return a script snippet that
    applies the state to the widget while building the form.

    \sa {Creating Custom Widgets for Qt Designer#Using Qt Script to Aid in Building Forms}{Creating Custom Widgets for Qt Designer}, QtScript
*/

/*!
    Destroys the extension.
*/

QDesignerScriptExtension::~QDesignerScriptExtension()
{
}

/*!
    \fn virtual QString QDesignerScriptExtension::script() const

    Returns a script snippet to be associated with the widget.
*/

/*!
    \fn virtual QVariantMap QDesignerScriptExtension::data() const

    Returns a map of variants describing the internal state to be
    stored in the UI file.
*/

/*!
    \fn virtual void QDesignerScriptExtension::setData(const QVariantMap &data)

    Applies the internal state stored in \a data to the widget while loading a form.
*/

QT_END_NAMESPACE
