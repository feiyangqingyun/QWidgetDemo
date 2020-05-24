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

#include "abstractnewformwidget_p.h"
#include <newformwidget_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerNewFormWidgetInterface
    \since 4.5
    \internal

    \brief QDesignerNewFormWidgetInterface provides an interface for chooser
           widgets that can be used within "New Form" dialogs and wizards.
           It presents the user with a list of choices taken from built-in
           templates, pre-defined template paths and suitable custom widgets.
           It provides a static creation function that returns \QD's
           implementation.

    \inmodule QtDesigner
*/

/*!
    Constructs a QDesignerNewFormWidgetInterface object.
*/

QDesignerNewFormWidgetInterface::QDesignerNewFormWidgetInterface(QWidget *parent) :
    QWidget(parent)
{
}

/*!
    Destroys the QDesignerNewFormWidgetInterface object.
*/

QDesignerNewFormWidgetInterface::~QDesignerNewFormWidgetInterface()
{
}

/*!
    Creates an instance of the QDesignerNewFormWidgetInterface as a child
    of \a parent using \a core.
*/

QDesignerNewFormWidgetInterface *QDesignerNewFormWidgetInterface::createNewFormWidget(QDesignerFormEditorInterface *core, QWidget *parent)
{
    return new qdesigner_internal::NewFormWidget(core, parent);
}

/*!
    \fn bool QDesignerNewFormWidgetInterface::hasCurrentTemplate() const

    Returns whether a form template is currently selected.
*/

/*!
    \fn QString QDesignerNewFormWidgetInterface::currentTemplate(QString *errorMessage = 0)

    Returns the contents of the currently selected template. If the method fails,
    an empty string is returned and \a errorMessage receives an error message.
*/

// Signals

/*!
    \fn void QDesignerNewFormWidgetInterface::templateActivated()

    This signal is emitted whenever the user activates a template by double-clicking.
*/

/*!
    \fn void QDesignerNewFormWidgetInterface::currentTemplateChanged(bool templateSelected)

    This signal is emitted whenever the user changes the current template.
    \a templateSelected indicates whether a template is currently selected.
*/

QT_END_NAMESPACE
