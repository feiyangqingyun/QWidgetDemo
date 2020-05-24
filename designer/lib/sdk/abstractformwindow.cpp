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

#include "abstractformwindow.h"

#include <widgetfactory_p.h>

#include <QtGui/QTabBar>
#include <QtGui/QSizeGrip>
#include <QtGui/QAbstractButton>
#include <QtGui/QToolBox>
#include <QtGui/QMenuBar>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QToolBar>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerFormWindowInterface

    \brief The QDesignerFormWindowInterface class allows you to query
    and manipulate form windows appearing in Qt Designer's workspace.

    \inmodule QtDesigner

    QDesignerFormWindowInterface provides information about
    the associated form window as well as allowing its properties to be
    altered. The interface is not intended to be instantiated
    directly, but to provide access to \QD's current form windows
    controlled by \QD's \l {QDesignerFormWindowManagerInterface}{form
    window manager}.

    If you are looking for the form window containing a specific
    widget, you can use the static
    QDesignerFormWindowInterface::findFormWindow() function:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 0

    But in addition, you can access any of the current form windows
    through \QD's form window manager: Use the
    QDesignerFormEditorInterface::formWindowManager() function to
    retrieve an interface to the manager. Once you have this
    interface, you have access to all of \QD's current form windows
    through the QDesignerFormWindowManagerInterface::formWindow()
    function. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 1

    The pointer to \QD's current QDesignerFormEditorInterface object
    (\c formEditor in the example above) is provided by the
    QDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the QDesignerCustomWidgetInterface class to expose your
    plugin to \QD.

    Once you have the form window, you can query its properties. For
    example, a plain custom widget plugin is managed by \QD only at
    its top level, i.e. none of its child widgets can be resized in
    \QD's workspace. But QDesignerFormWindowInterface provides you
    with functions that enables you to control whether a widget should
    be managed by \QD, or not:

    \snippet doc/src/snippets/code/tools_designer_src_lib_sdk_abstractformwindow.cpp 2

    The complete list of functions concerning widget management is:
    isManaged(), manageWidget() and unmanageWidget(). There is also
    several associated signals: widgetManaged(), widgetRemoved(),
    aboutToUnmanageWidget() and widgetUnmanaged().

    In addition to controlling the management of widgets, you can
    control the current selection in the form window using the
    selectWidget(), clearSelection() and emitSelectionChanged()
    functions, and the selectionChanged() signal.

    You can also retrieve information about where the form is stored
    using absoluteDir(), its include files using includeHints(), and
    its layout and pixmap functions using layoutDefault(),
    layoutFunction() and pixmapFunction(). You can find out whether
    the form window has been modified (but not saved) or not, using
    the isDirty() function. You can retrieve its author(), its
    contents(), its fileName(), associated comment() and
    exportMacro(), its mainContainer(), its features(), its grid() and
    its resourceFiles().

    The interface provides you with functions and slots allowing you
    to alter most of this information as well. The exception is the
    directory storing the form window. Finally, there is several
    signals associated with changes to the information mentioned above
    and to the form window in general.

    \sa QDesignerFormWindowCursorInterface,
    QDesignerFormEditorInterface, QDesignerFormWindowManagerInterface
*/

/*!
    \enum QDesignerFormWindowInterface::FeatureFlag

    This enum describes the features that are available and can be
    controlled by the form window interface. These values are used
    when querying the form window to determine which features it
    supports:

    \value EditFeature      Form editing
    \value GridFeature      Grid display and snap-to-grid facilities for editing
    \value TabOrderFeature  Tab order management
    \value DefaultFeature   Support for default features (form editing and grid)

    \sa hasFeature(), features()
*/

/*!
    Constructs a form window interface with the given \a parent and
    the specified window \a flags.
*/
QDesignerFormWindowInterface::QDesignerFormWindowInterface(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

/*!
    Destroys the form window interface.
*/
QDesignerFormWindowInterface::~QDesignerFormWindowInterface()
{
}

/*!
    Returns a pointer to \QD's current QDesignerFormEditorInterface
    object.
*/
QDesignerFormEditorInterface *QDesignerFormWindowInterface::core() const
{
    return 0;
}

/*!
    \fn QDesignerFormWindowInterface *QDesignerFormWindowInterface::findFormWindow(QWidget *widget)

    Returns the form window interface for the given \a widget.
*/

static inline bool stopFindAtTopLevel(const QObject *w, bool stopAtMenu)
{
    // Do we need to go beyond top levels when looking for the form window?
    // 1) A dialog has a window attribute at the moment it is created
    //    before it is properly embedded into a form window. The property
    //    sheet queries the layout attributes precisely at this moment.
    // 2) In the case of floating docks and toolbars, we also need to go beyond the top level window.
    // 3) In the case of menu editing, we don't want to block events from the
    //    Designer menu, so, we say stop.
    // Note that there must be no false positives for dialogs parented on
    // the form (for example, the "change object name" dialog), else, its
    // events will be blocked.

    if (stopAtMenu && w->inherits("QDesignerMenu"))
        return true;
    return !qdesigner_internal::WidgetFactory::isFormEditorObject(w);
}

QDesignerFormWindowInterface *QDesignerFormWindowInterface::findFormWindow(QWidget *w)
{
    while (w != 0) {
        if (QDesignerFormWindowInterface *fw = qobject_cast<QDesignerFormWindowInterface*>(w)) {
            return fw;
        } else {
            if (w->isWindow() && stopFindAtTopLevel(w, true))
                break;
        }

        w = w->parentWidget();
    }

    return 0;
}

/*!
    \fn QDesignerFormWindowInterface *QDesignerFormWindowInterface::findFormWindow(QObject *object)

    Returns the form window interface for the given \a object.

    \since 4.4
*/

QDesignerFormWindowInterface *QDesignerFormWindowInterface::findFormWindow(QObject *object)
{
    while (object != 0) {
        if (QDesignerFormWindowInterface *fw = qobject_cast<QDesignerFormWindowInterface*>(object)) {
            return fw;
        } else {
            QWidget *w = qobject_cast<QWidget *>(object);
            // QDesignerMenu is a window, so stopFindAtTopLevel(w) returns 0.
            // However, we want to find the form window for QActions of a menu.
            // If this check is inside stopFindAtTopLevel(w), it will break designer
            // menu editing (e.g. when clicking on items inside an opened menu)
            if (w && w->isWindow() && stopFindAtTopLevel(w, false))
                break;

        }

        object = object->parent();
    }

    return 0;
}

/*!
    \fn virtual QString QDesignerFormWindowInterface::fileName() const

    Returns the file name of the UI file that describes the form
    currently being shown.

    \sa setFileName()
*/

/*!
    \fn virtual QDir QDesignerFormWindowInterface::absoluteDir() const

    Returns the absolute location of the directory containing the form
    shown in the form window.
*/

/*!
    \fn virtual QString QDesignerFormWindowInterface::contents() const

    Returns details of the contents of the form currently being
    displayed in the window.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setContents(QIODevice *device)

    Sets the form's contents using data obtained from the given \a device.

    Data can be read from QFile objects or any other subclass of QIODevice.
*/

/*!
    \fn virtual Feature QDesignerFormWindowInterface::features() const

    Returns a combination of the features provided by the form window
    associated with the interface. The value returned can be tested
    against the \l Feature enum values to determine which features are
    supported by the window.

    \sa setFeatures(), hasFeature()
*/

/*!
    \fn virtual bool QDesignerFormWindowInterface::hasFeature(Feature feature) const

    Returns true if the form window offers the specified \a feature;
    otherwise returns false.

    \sa features()
*/

/*!
    \fn virtual QString QDesignerFormWindowInterface::author() const

    Returns details of the author or creator of the form currently
    being displayed in the window.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setAuthor(const QString &author)

    Sets the details for the author or creator of the form to the \a
    author specified.
*/

/*!
    \fn virtual QString QDesignerFormWindowInterface::comment() const

    Returns comments about the form currently being displayed in the window.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setComment(const QString &comment)

    Sets the information about the form to the \a comment
    specified. This information should be a human-readable comment
    about the form.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::layoutDefault(int *margin, int *spacing)

    Fills in the default margin and spacing for the form's default
    layout in the \a margin and \a spacing variables specified.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setLayoutDefault(int margin, int spacing)

    Sets the default \a margin and \a spacing for the form's layout.

    \sa layoutDefault()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::layoutFunction(QString *margin, QString *spacing)

    Fills in the current margin and spacing for the form's layout in
    the \a margin and \a spacing variables specified.
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setLayoutFunction(const QString &margin, const QString &spacing)

    Sets the \a margin and \a spacing for the form's layout.

    The default layout properties will be replaced by the
    corresponding layout functions when \c uic generates code for the
    form, that is, if the functions are specified. This is useful when
    different environments requires different layouts for the same
    form.

    \sa layoutFunction()
*/

/*!
    \fn virtual QString QDesignerFormWindowInterface::pixmapFunction() const

    Returns the name of the function used to load pixmaps into the
    form window.

    \sa setPixmapFunction()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setPixmapFunction(const QString &pixmapFunction)

    Sets the function used to load pixmaps into the form window
    to the given \a pixmapFunction.

    \sa pixmapFunction()
*/

/*!
    \fn virtual QString QDesignerFormWindowInterface::exportMacro() const

    Returns the export macro associated with the form currently being
    displayed in the window.  The export macro is used when the form
    is compiled to create a widget plugin.

    \sa {Creating Custom Widgets for Qt Designer}
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setExportMacro(const QString &exportMacro)

    Sets the form window's export macro to \a exportMacro. The export
    macro is used when building a widget plugin to export the form's
    interface to other components.
*/

/*!
    \fn virtual QStringList QDesignerFormWindowInterface::includeHints() const

    Returns a list of the header files that will be included in the
    form window's associated UI file.

    Header files may be local, i.e. relative to the project's
    directory, \c "mywidget.h", or global, i.e. part of Qt or the
    compilers standard libraries: \c <QtGui/QWidget>.

    \sa setIncludeHints()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setIncludeHints(const QStringList &includeHints)

    Sets the header files that will be included in the form window's
    associated UI file to the specified \a includeHints.

    Header files may be local, i.e. relative to the project's
    directory, \c "mywidget.h", or global, i.e. part of Qt or the
    compilers standard libraries: \c <QtGui/QWidget>.

    \sa includeHints()
*/

/*!
    \fn virtual QDesignerFormWindowCursorInterface *QDesignerFormWindowInterface::cursor() const

    Returns the cursor interface used by the form window.
*/

/*!
    \fn virtual int QDesignerFormWindowInterface::toolCount() const

    Returns the number of tools available.

    \internal
*/

/*!
    \fn virtual int QDesignerFormWindowInterface::currentTool() const

    Returns the index of the current tool in use.

    \sa setCurrentTool()

    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setCurrentTool(int index)

    Sets the current tool to be the one with the given \a index.

    \sa currentTool()

    \internal
*/

/*!
    \fn virtual QDesignerFormWindowToolInterface *QDesignerFormWindowInterface::tool(int index) const

    Returns an interface to the tool with the given \a index.

    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::registerTool(QDesignerFormWindowToolInterface *tool)

    Registers the given \a tool with the form window.

    \internal
*/

/*!
    \fn virtual QPoint QDesignerFormWindowInterface::grid() const = 0

    Returns the grid spacing used by the form window.

    \sa setGrid()
*/

/*!
    \fn virtual QWidget *QDesignerFormWindowInterface::mainContainer() const

    Returns the main container widget for the form window.

    \sa setMainContainer()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setMainContainer(QWidget *mainContainer)

    Sets the main container widget on the form to the specified \a
    mainContainer.

    \sa mainContainer(), mainContainerChanged()
*/

/*!
    \fn virtual bool QDesignerFormWindowInterface::isManaged(QWidget *widget) const

    Returns true if the specified \a widget is managed by the form
    window; otherwise returns false.

    \sa manageWidget()
*/

/*!
    \fn virtual bool QDesignerFormWindowInterface::isDirty() const

    Returns true if the form window is "dirty" (modified but not
    saved); otherwise returns false.

    \sa setDirty()
*/

/*!
    \fn virtual QUndoStack *QDesignerFormWindowInterface::commandHistory() const

    Returns an object that can be used to obtain the commands used so
    far in the construction of the form.

    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::beginCommand(const QString &description)

    Begins execution of a command with the given \a
    description. Commands are executed between beginCommand() and
    endCommand() function calls to ensure that they are recorded on
    the undo stack.

    \sa endCommand()

    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::endCommand()

    Ends execution of the current command.

    \sa beginCommand()

    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::simplifySelection(QList<QWidget*> *widgets) const

    Simplifies the selection of widgets specified by \a widgets.

    \sa selectionChanged()
    \internal
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::emitSelectionChanged()

    Emits the selectionChanged() signal.

    \sa selectWidget(), clearSelection()
*/

/*!
    \fn virtual QStringList QDesignerFormWindowInterface::resourceFiles() const

    Returns a list of paths to resource files that are currently being
    used by the form window.

    \sa addResourceFile(), removeResourceFile()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::addResourceFile(const QString &path)

    Adds the resource file at the given \a path to those used by the form.

    \sa resourceFiles(), resourceFilesChanged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::removeResourceFile(const QString &path)

    Removes the resource file at the specified \a path from the list
    of those used by the form.

    \sa resourceFiles(), resourceFilesChanged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::ensureUniqueObjectName(QObject *object)

    Ensures that the specified \a object has a unique name amongst the
    other objects on the form.

    \internal
*/

// Slots

/*!
    \fn virtual void QDesignerFormWindowInterface::manageWidget(QWidget *widget)

    Instructs the form window to manage the specified \a widget.

    \sa isManaged(), unmanageWidget(), widgetManaged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::unmanageWidget(QWidget *widget)

    Instructs the form window not to manage the specified \a widget.

    \sa aboutToUnmanageWidget(), widgetUnmanaged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setFeatures(Feature features)

    Enables the specified \a features for the form window.

    \sa features(), featureChanged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setDirty(bool dirty)

    If \a dirty is true, the form window is marked as dirty, meaning
    that it is modified but not saved. If \a dirty is false, the form
    window is considered to be unmodified.

    \sa isDirty()
*/

/*!
\fn virtual void QDesignerFormWindowInterface::clearSelection(bool update)

    Clears the current selection in the form window. If \a update is
    true, the emitSelectionChanged() function is called, emitting the
    selectionChanged() signal.

    \sa selectWidget()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::selectWidget(QWidget *widget, bool select)

    If \a select is true, the given \a widget is selected; otherwise
    the \a widget is deselected.

    \sa clearSelection(), selectionChanged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setGrid(const QPoint &grid)

    Sets the grid size for the form window to the point specified by
    \a grid. In this function, the coordinates in the QPoint are used
    to specify the dimensions of a rectangle in the grid.

    \sa grid()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setFileName(const QString &fileName)

    Sets the file name for the form to the given \a fileName.

    \sa fileName(), fileNameChanged()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::setContents(const QString &contents)

    Sets the contents of the form using data read from the specified
    \a contents string.

    \sa contents()
*/

/*!
    \fn virtual void QDesignerFormWindowInterface::editWidgets()

    Switches the form window into editing mode.

    \sa \l {Qt Designer's Form Editing Mode}

    \internal
*/

// Signals

/*!
    \fn void QDesignerFormWindowInterface::mainContainerChanged(QWidget *mainContainer)

    This signal is emitted whenever the main container changes.
    The new container is specified by \a mainContainer.

    \sa setMainContainer()
*/

/*!
    \fn void QDesignerFormWindowInterface::toolChanged(int toolIndex)

    This signal is emitted whenever the current tool changes.
    The specified \a toolIndex is the index of the new tool in the list of
    tools in the widget box.

    \internal
*/

/*!
    \fn void QDesignerFormWindowInterface::fileNameChanged(const QString &fileName)

    This signal is emitted whenever the file name of the form changes.
    The new file name is specified by \a fileName.

    \sa setFileName()
*/

/*!
    \fn void QDesignerFormWindowInterface::featureChanged(Feature feature)

    This signal is emitted whenever a feature changes in the form.
    The new feature is specified by \a feature.

    \sa setFeatures()
*/

/*!
    \fn void QDesignerFormWindowInterface::selectionChanged()

    This signal is emitted whenever the selection in the form changes.

    \sa selectWidget(), clearSelection()
*/

/*!
    \fn void QDesignerFormWindowInterface::geometryChanged()

    This signal is emitted whenever the form's geometry changes.
*/

/*!
    \fn void QDesignerFormWindowInterface::resourceFilesChanged()

    This signal is emitted whenever the list of resource files used by the
    form changes.

    \sa resourceFiles()
*/

/*!
    \fn void QDesignerFormWindowInterface::widgetManaged(QWidget *widget)

    This signal is emitted whenever a widget on the form becomes managed.
    The newly managed widget is specified by \a widget.

    \sa manageWidget()
*/

/*!
    \fn void QDesignerFormWindowInterface::widgetUnmanaged(QWidget *widget)

    This signal is emitted whenever a widget on the form becomes unmanaged.
    The newly released widget is specified by \a widget.

    \sa unmanageWidget(), aboutToUnmanageWidget()
*/

/*!
    \fn void QDesignerFormWindowInterface::aboutToUnmanageWidget(QWidget *widget)

    This signal is emitted whenever a widget on the form is about to
    become unmanaged.  When this signal is emitted, the specified \a
    widget is still managed, and a widgetUnmanaged() signal will
    follow, indicating when it is no longer managed.

    \sa unmanageWidget(), isManaged()
*/

/*!
    \fn void QDesignerFormWindowInterface::activated(QWidget *widget)

    This signal is emitted whenever a widget is activated on the form.
    The activated widget is specified by \a widget.
*/

/*!
    \fn void QDesignerFormWindowInterface::changed()

    This signal is emitted whenever a form is changed.
*/

/*!
    \fn void QDesignerFormWindowInterface::widgetRemoved(QWidget *widget)

    This signal is emitted whenever a widget is removed from the form.
    The widget that was removed is specified by \a widget.
*/

/*!
    \fn void QDesignerFormWindowInterface::objectRemoved(QObject *object)

    This signal is emitted whenever an object (such as
    an action or a QButtonGroup) is removed from the form.
    The object that was removed is specified by \a object.

   \since 4.5
*/

QT_END_NAMESPACE
