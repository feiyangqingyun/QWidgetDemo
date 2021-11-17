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

#include "abstractpromotioninterface.h"

QT_BEGIN_NAMESPACE

QDesignerPromotionInterface::~QDesignerPromotionInterface()
{
}

/*!
    \class QDesignerPromotionInterface

    \brief The QDesignerPromotionInterface provides functions for modifying
           the promoted classes in Designer.
    \inmodule QtDesigner
    \internal
    \since 4.3
*/

/*!
    \class QDesignerPromotionInterface::PromotedClass
    A pair of database items containing the base class and the promoted class.

    \typedef QDesignerPromotionInterface::PromotedClasses
    A list of PromotedClass items.

    virtual QDesignerPromotionInterface::PromotedClasses promotedClasses()  const

    Returns a list of promoted classes along with their base classes in alphabetical order.
    It can be used to populate tree models for editing promoted widgets.

*/

/*!
    \fn virtual QSet<QString> QDesignerPromotionInterface::referencedPromotedClassNames()  const

    Returns a set of promoted classed that are referenced by the currently opened forms.
*/

/*! 
    \fn virtual bool QDesignerPromotionInterface::addPromotedClass(const QString &baseClass, const QString &className, const QString &includeFile, QString *errorMessage)

    Add a promoted class named \a with the base class \a and include file \a includeFile. Returns \c true on success or \c false along
    with an error message in \a errorMessage on failure.
*/

/*! 
    \fn  virtual bool QDesignerPromotionInterface::removePromotedClass(const QString &className, QString *errorMessage)

    Remove the promoted class named \a className unless it is referenced by a form. Returns \c true on success or \c false along
    with an error message in \a errorMessage on failure.
*/

/*! 
    \fn  virtual bool QDesignerPromotionInterface::changePromotedClassName(const QString &oldClassName, const QString &newClassName,  QString *errorMessage)

    Change the class name of a promoted class from \a oldClassName to  \a newClassName. Returns \c true on success or \c false along
    with an error message in \a errorMessage on failure.
*/

/*! 
    \fn  virtual bool QDesignerPromotionInterface::setPromotedClassIncludeFile(const QString &className, const QString &includeFile, QString *errorMessage)

    Change the include file of a promoted class named \a className to be \a includeFile. Returns \c true on success or \c false along
    with an error message in \a errorMessage on failure.
*/

/*! \fn virtual QList<QDesignerWidgetDataBaseItemInterface *> QDesignerPromotionInterface::promotionBaseClasses() const

     Return a list of base classes that are suitable for promotion.
*/

QT_END_NAMESPACE
