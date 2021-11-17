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

#include "abstractwidgetbox.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerWidgetBoxInterface

    \brief The QDesignerWidgetBoxInterface class allows you to control
    the contents of Qt Designer's widget box.

    \inmodule QtDesigner

    QDesignerWidgetBoxInterface contains a collection of functions
    that is typically used to manipulate the contents of \QD's widget
    box.

    \QD uses an XML file to populate its widget box. The name of that
    file is one of the widget box's properties, and you can retrieve
    it using the fileName() function.

    QDesignerWidgetBoxInterface also provides the save() function that
    saves the contents of the widget box in the file specified by the
    widget box's file name property. If you have made changes to the
    widget box, for example by dropping a widget into the widget box,
    without calling the save() function, the original content can be
    restored by a simple invocation of the load() function:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 0

    The QDesignerWidgetBoxInterface class is not intended to be
    instantiated directly. You can retrieve an interface to Qt
    Designer's widget box using the
    QDesignerFormEditorInterface::widgetBox() function. A pointer to
    \QD's current QDesignerFormEditorInterface object (\c formEditor
    in the example above) is provided by the
    QDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the QDesignerCustomWidgetInterface to expose your plugin
    to \QD.

    If you want to save your changes, and at the same time preserve
    the original contents, you can use the save() function combined
    with the setFileName() function to save your changes into another
    file. Remember to store the name of the original file first:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 1

    Then you can restore the original contents of the widget box by
    resetting the file name to the original file and calling load():

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 2

    In a similar way, you can later use your customized XML file:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractwidgetbox.cpp 3


    \sa QDesignerFormEditorInterface
*/

/*!
    Constructs a widget box interface with the given \a parent and
    the specified window \a flags.
*/
QDesignerWidgetBoxInterface::QDesignerWidgetBoxInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the widget box interface.
*/
QDesignerWidgetBoxInterface::~QDesignerWidgetBoxInterface()
{
}

/*!
    \internal
*/
int QDesignerWidgetBoxInterface::findOrInsertCategory(const QString &categoryName)
{
    int count = categoryCount();
    for (int index=0; index<count; ++index) {
        Category c = category(index);
        if (c.name() == categoryName)
            return index;
    }

    addCategory(Category(categoryName));
    return count;
}

/*!
    \internal
    \fn int QDesignerWidgetBoxInterface::categoryCount() const
*/

/*!
    \internal
    \fn Category QDesignerWidgetBoxInterface::category(int cat_idx) const
*/

/*!
    \internal
    \fn void QDesignerWidgetBoxInterface::addCategory(const Category &cat)
*/

/*!
    \internal
    \fn void QDesignerWidgetBoxInterface::removeCategory(int cat_idx)
*/

/*!
    \internal
    \fn int QDesignerWidgetBoxInterface::widgetCount(int cat_idx) const
*/

/*!
    \internal
    \fn Widget QDesignerWidgetBoxInterface::widget(int cat_idx, int wgt_idx) const
*/

/*!
    \internal
    \fn void QDesignerWidgetBoxInterface::addWidget(int cat_idx, const Widget &wgt)
*/

/*!
    \internal
    \fn void QDesignerWidgetBoxInterface::removeWidget(int cat_idx, int wgt_idx)
*/

/*!
    \internal
    \fn void QDesignerWidgetBoxInterface::dropWidgets(const QList<QDesignerDnDItemInterface*> &item_list, const QPoint &global_mouse_pos)

*/

/*!
    \fn void QDesignerWidgetBoxInterface::setFileName(const QString &fileName)

    Sets the XML file that \QD will use to populate its widget box, to
    \a fileName. You must call load() to update the widget box with
    the new XML file.

    \sa fileName(), load()
*/

/*!
    \fn QString QDesignerWidgetBoxInterface::fileName() const

    Returns the name of the XML file \QD is currently using to
    populate its widget box.

    \sa setFileName()
*/

/*!
    \fn bool QDesignerWidgetBoxInterface::load()

    Populates \QD's widget box by loading (or reloading) the currently
    specified XML file. Returns true if the file is successfully
    loaded; otherwise false.

    \sa setFileName()
*/

/*!
    \fn bool QDesignerWidgetBoxInterface::save()

    Saves the contents of \QD's widget box in the file specified by
    the fileName() function. Returns true if the content is
    successfully saved; otherwise false.

    \sa fileName(), setFileName()
*/


/*!
    \internal

    \class QDesignerWidgetBoxInterface::Widget

    \brief The Widget class specified a widget in Qt Designer's widget
    box component.
*/

/*!
    \enum QDesignerWidgetBoxInterface::Widget::Type

    \value Default
    \value Custom
*/

/*!
    \fn QDesignerWidgetBoxInterface::Widget::Widget(const QString &aname, const QString &xml, const QString &icon_name, Type atype)
*/

/*!
    \fn QString QDesignerWidgetBoxInterface::Widget::name() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Widget::setName(const QString &aname)
*/

/*!
    \fn QString QDesignerWidgetBoxInterface::Widget::domXml() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Widget::setDomXml(const QString &xml)
*/

/*!
    \fn QString QDesignerWidgetBoxInterface::Widget::iconName() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Widget::setIconName(const QString &icon_name)
*/

/*!
    \fn Type QDesignerWidgetBoxInterface::Widget::type() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Widget::setType(Type atype)
*/

/*!
    \fn bool QDesignerWidgetBoxInterface::Widget::isNull() const
*/


/*!
    \class QDesignerWidgetBoxInterface::Category
    \brief The Category class specifies a category in Qt Designer's widget box component.
    \internal
*/

/*!
    \enum QDesignerWidgetBoxInterface::Category::Type

    \value Default
    \value Scratchpad
*/

/*!
    \fn QDesignerWidgetBoxInterface::Category::Category(const QString &aname, Type atype)
*/

/*!
    \fn QString QDesignerWidgetBoxInterface::Category::name() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Category::setName(const QString &aname)
*/

/*!
    \fn int QDesignerWidgetBoxInterface::Category::widgetCount() const
*/

/*!
    \fn Widget QDesignerWidgetBoxInterface::Category::widget(int idx) const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Category::removeWidget(int idx)
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Category::addWidget(const Widget &awidget)
*/

/*!
    \fn Type QDesignerWidgetBoxInterface::Category::type() const
*/

/*!
    \fn void QDesignerWidgetBoxInterface::Category::setType(Type atype)
*/

/*!
    \fn bool QDesignerWidgetBoxInterface::Category::isNull() const
*/

/*!
    \typedef QDesignerWidgetBoxInterface::CategoryList
    \internal
*/

/*!
    \typedef QDesignerWidgetBoxInterface::WidgetList
    \internal
*/

QT_END_NAMESPACE
