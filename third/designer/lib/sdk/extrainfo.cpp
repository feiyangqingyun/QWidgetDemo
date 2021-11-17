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

#include "extrainfo.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerExtraInfoExtension
    \brief The QDesignerExtraInfoExtension class provides extra information about a widget in
    Qt Designer.
    \inmodule QtDesigner
    \internal
*/

/*!
    Returns the path to the working directory used by this extension.*/
QString QDesignerExtraInfoExtension::workingDirectory() const
{
    return m_workingDirectory;
}

/*!
    Sets the path to the working directory used by the extension to \a workingDirectory.*/
void QDesignerExtraInfoExtension::setWorkingDirectory(const QString &workingDirectory)
{
    m_workingDirectory = workingDirectory;
}

/*!
    \fn virtual QDesignerExtraInfoExtension::~QDesignerExtraInfoExtension()

    Destroys the extension.
*/

/*!
    \fn virtual QDesignerFormEditorInterface *QDesignerExtraInfoExtension::core() const = 0

    \omit
    ### Description required
    \endomit
*/

/*!
    \fn virtual QWidget *QDesignerExtraInfoExtension::widget() const = 0

    Returns the widget described by this extension.
*/

/*!
    \fn virtual bool QDesignerExtraInfoExtension::saveUiExtraInfo(DomUI *ui) = 0

    Saves the information about the user interface specified by \a ui, and returns true if
    successful; otherwise returns false.
*/

/*!
    \fn virtual bool QDesignerExtraInfoExtension::loadUiExtraInfo(DomUI *ui) = 0

    Loads extra information about the user interface specified by \a ui, and returns true if
    successful; otherwise returns false.
*/

/*!
    \fn virtual bool QDesignerExtraInfoExtension::saveWidgetExtraInfo(DomWidget *widget) = 0

    Saves the information about the specified \a widget, and returns true if successful;
    otherwise returns false.
*/

/*!
    \fn virtual bool QDesignerExtraInfoExtension::loadWidgetExtraInfo(DomWidget *widget) = 0

    Loads extra information about the specified \a widget, and returns true if successful;
    otherwise returns false.
*/

QT_END_NAMESPACE
