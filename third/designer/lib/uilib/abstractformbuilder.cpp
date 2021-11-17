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
**sw
****************************************************************************/

#include "abstractformbuilder.h"
#include "formbuilderextra_p.h"
#include "resourcebuilder_p.h"
#include "textbuilder_p.h"
#include "ui4_p.h"
#include "properties_p.h"

#include <QtCore/QVariant>
#include <QtCore/QMetaProperty>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QHash>
#include <QtCore/qdebug.h>
#include <QtCore/QCoreApplication>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFontComboBox>
#include <QtGui/QGridLayout>
#ifndef QT_NO_FORMLAYOUT
#  include <QtGui/QFormLayout>
#endif
#include <QtGui/QIcon>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QPixmap>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QScrollArea>
#include <QtGui/QAbstractButton>
#include <QtGui/QAbstractItemView>
#include <QtGui/QHeaderView>
#ifndef QFORMINTERNAL_NAMESPACE
#  include <private/qlayout_p.h> // Compiling within Designer
#endif

#include <QtXml/QXmlStreamReader>

#include <QtGui/QDialog>

// containers
#include <QtGui/QToolBox>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QMenuBar>
#include <QtGui/QDockWidget>
#include <QtGui/QMdiArea>
#include <QtGui/QWorkspace>
#include <QtGui/QWizard>
#include <QtGui/QWizardPage>

#include <QtCore/qdebug.h>

#include <limits.h>

Q_DECLARE_METATYPE(QWidgetList)

static const char *buttonGroupPropertyC = "buttonGroup";

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
using namespace QFormInternal;
#endif

class QFriendlyLayout: public QLayout
{
public:
    inline QFriendlyLayout() { Q_ASSERT(0); }

#ifdef QFORMINTERNAL_NAMESPACE
    friend class QFormInternal::QAbstractFormBuilder;
#else
    friend class QAbstractFormBuilder;
#endif
};

/*!
    \class QAbstractFormBuilder

    \brief The QAbstractFormBuilder class provides a default
    implementation for classes that create user interfaces at
    run-time.

    \inmodule QtDesigner

    QAbstractFormBuilder provides a standard interface and a default
    implementation for constructing forms from user interface
    files. It is not intended to be instantiated directly. Use the
    QFormBuilder class to create user interfaces from UI files at
    run-time. For example:

    \snippet doc/src/snippets/code/tools_designer_src_lib_uilib_abstractformbuilder.cpp 0

    To override certain aspects of the form builder's behavior,
    subclass QAbstractFormBuilder and reimplement the relevant virtual
    functions:

    \list
    \o load() handles reading of UI format files from arbitrary
       QIODevices, and construction of widgets from the XML data
       that they contain.
    \o save() handles saving of widget details in UI format to
       arbitrary QIODevices.
    \o workingDirectory() and setWorkingDirectory() control the
       directory in which forms are held. The form builder looks for
       other resources on paths relative to this directory.
    \endlist

    The QFormBuilder class is typically used by custom components and
    applications that embed \QD. Standalone applications that need to
    dynamically generate user interfaces at run-time use the
    QUiLoader, found in the QtUiTools module.

    \sa {QtUiTools Module}
*/

/*!
    Constructs a new form builder.*/
QAbstractFormBuilder::QAbstractFormBuilder() :
    m_defaultMargin(INT_MIN),
    m_defaultSpacing(INT_MIN)
{
    setResourceBuilder(new QResourceBuilder());
    setTextBuilder(new QTextBuilder());
}

/*!
    Destroys the form builder.*/
QAbstractFormBuilder::~QAbstractFormBuilder()
{
    QFormBuilderExtra::removeInstance(this);
}


/*!
    \fn QWidget *QAbstractFormBuilder::load(QIODevice *device, QWidget *parent)

    Loads an XML representation of a widget from the given \a device,
    and constructs a new widget with the specified \a parent.

    \sa save()
*/
QWidget *QAbstractFormBuilder::load(QIODevice *dev, QWidget *parentWidget)
{
    QXmlStreamReader reader;
    reader.setDevice(dev);
    DomUI ui;
    bool initialized = false;

    const QString uiElement = QLatin1String("ui");
    while (!reader.atEnd()) {
        if (reader.readNext() == QXmlStreamReader::StartElement) {
            if (reader.name().compare(uiElement, Qt::CaseInsensitive) == 0) {
                ui.read(reader);
                initialized = true;
            } else {
                reader.raiseError(QCoreApplication::translate("QAbstractFormBuilder", "Unexpected element <%1>").arg(reader.name().toString()));
            }
        }
    }
    if (reader.hasError()) {
        uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "An error has occurred while reading the UI file at line %1, column %2: %3")
                                .arg(reader.lineNumber()).arg(reader.columnNumber())
                                .arg(reader.errorString()));
        return 0;
    }
    if (!initialized) {
        uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "Invalid UI file: The root element <ui> is missing."));
        return 0;
    }

    QWidget *widget = create(&ui, parentWidget);
    return widget;
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::create(DomUI *ui, QWidget *parentWidget)
{
    typedef QFormBuilderExtra::ButtonGroupHash ButtonGroupHash;

    QFormBuilderExtra *formBuilderPrivate = QFormBuilderExtra::instance(this);
    formBuilderPrivate->clear();
    if (const DomLayoutDefault *def = ui->elementLayoutDefault()) {
       m_defaultMargin = def->hasAttributeMargin() ? def->attributeMargin() : INT_MIN;
        m_defaultSpacing = def->hasAttributeSpacing() ? def->attributeSpacing() : INT_MIN;
    }

    DomWidget *ui_widget = ui->elementWidget();
    if (!ui_widget)
        return 0;

    initialize(ui);

    if (const DomButtonGroups *domButtonGroups = ui->elementButtonGroups())
        formBuilderPrivate->registerButtonGroups(domButtonGroups);

    if (QWidget *widget = create(ui_widget, parentWidget)) {
        // Reparent button groups that were actually created to main container for them to be found in the signal/slot part
        const ButtonGroupHash &buttonGroups = formBuilderPrivate->buttonGroups();
        if (!buttonGroups.empty()) {
            const ButtonGroupHash::const_iterator cend = buttonGroups.constEnd();
            for (ButtonGroupHash::const_iterator it = buttonGroups.constBegin(); it != cend; ++it)
                if (it.value().second)
                    it.value().second->setParent(widget);
        }
        createConnections(ui->elementConnections(), widget);
        createResources(ui->elementResources()); // maybe this should go first, before create()...
        applyTabStops(widget, ui->elementTabStops());
        formBuilderPrivate->applyInternalProperties();
        reset();
        formBuilderPrivate->clear();
        return widget;
    }
    formBuilderPrivate->clear();
    return 0;
}

/*!
    \internal
    Retrieve relevant information from the custom widgets section.
    Called by create(DomUI *, QWidget *); call manually if you
    just use create(DomWidget *, QWidget *) on some child widget of DomUI.
 */

void QAbstractFormBuilder::initialize(const DomUI *ui)
{
    typedef QList<DomCustomWidget*> DomCustomWidgetList;

    DomCustomWidgets *domCustomWidgets  = ui->elementCustomWidgets();
    createCustomWidgets(domCustomWidgets);

    if (domCustomWidgets) {
        const DomCustomWidgetList customWidgets = domCustomWidgets->elementCustomWidget();
        if (!customWidgets.empty()) {
            QFormBuilderExtra *formBuilderPrivate = QFormBuilderExtra::instance(this);
            const DomCustomWidgetList::const_iterator cend = customWidgets.constEnd();
            for (DomCustomWidgetList::const_iterator it = customWidgets.constBegin(); it != cend; ++it)
                formBuilderPrivate->storeCustomWidgetData((*it)->elementClass(), *it);
        }
    }
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::create(DomWidget *ui_widget, QWidget *parentWidget)
{
    QWidget *w = createWidget(ui_widget->attributeClass(), parentWidget, ui_widget->attributeName());
    if (!w)
        return 0;

    applyProperties(w, ui_widget->elementProperty());

    foreach (DomAction *ui_action, ui_widget->elementAction()) {
        QAction *child_action = create(ui_action, w);
        Q_UNUSED( child_action );
    }

    foreach (DomActionGroup *ui_action_group, ui_widget->elementActionGroup()) {
        QActionGroup *child_action_group = create(ui_action_group, w);
        Q_UNUSED( child_action_group );
    }

    QWidgetList children;
    foreach (DomWidget *ui_child, ui_widget->elementWidget()) {
        if (QWidget *child  = create(ui_child, w)) {
            children += child;
        } else {
            const QString className = ui_child->elementClass().empty() ? QString() : ui_child->elementClass().front();
            uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "The creation of a widget of the class '%1' failed.").arg(className));
        }
    }

    foreach (DomLayout *ui_lay, ui_widget->elementLayout()) {
        QLayout *child_lay = create(ui_lay, 0, w);
        Q_UNUSED( child_lay );
    }

    const QList<DomActionRef *> addActions = ui_widget->elementAddAction();
    if (!addActions.empty()) {
        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
        foreach (DomActionRef *ui_action_ref, addActions) {
            const QString name = ui_action_ref->attributeName();
            if (name == strings.separator) {
                QAction *sep = new QAction(w);
                sep->setSeparator(true);
                w->addAction(sep);
                addMenuAction(sep);
            } else if (QAction *a = m_actions.value(name)) {
                w->addAction(a);
            } else if (QActionGroup *g = m_actionGroups.value(name)) {
                w->addActions(g->actions());
            } else if (QMenu *menu = qFindChild<QMenu*>(w, name)) {
                w->addAction(menu->menuAction());
                addMenuAction(menu->menuAction());
            }
        }
    }

    loadExtraInfo(ui_widget, w, parentWidget);
#ifndef QT_FORMBUILDER_NO_SCRIPT
    QString scriptErrorMessage;
    QFormBuilderExtra *extra = QFormBuilderExtra::instance(this);
    extra->formScriptRunner().run(ui_widget,
                                  extra->customWidgetScript(ui_widget->attributeClass()),
                                  w, children, &scriptErrorMessage);
#endif
    addItem(ui_widget, w, parentWidget);

    if (qobject_cast<QDialog *>(w) && parentWidget)
        w->setAttribute(Qt::WA_Moved, false); // So that QDialog::setVisible(true) will center it

    const QStringList zOrderNames = ui_widget->elementZOrder();
    if (!zOrderNames.isEmpty()) {
        QList<QWidget *> zOrder = qVariantValue<QWidgetList>(w->property("_q_zOrder"));
        foreach (const QString &widgetName, zOrderNames) {
            if (QWidget *child = qFindChild<QWidget*>(w, widgetName)) {
                if (child->parentWidget() == w) {
                    zOrder.removeAll(child);
                    zOrder.append(child);
                    child->raise();
                }
            }
        }
        w->setProperty("_q_zOrder", qVariantFromValue(zOrder));
    }

    return w;
}

/*!
    \internal
*/
QAction *QAbstractFormBuilder::create(DomAction *ui_action, QObject *parent)
{
    QAction *a = createAction(parent, ui_action->attributeName());
    if (!a)
        return 0;

    m_actions.insert(ui_action->attributeName(), a);
    applyProperties(a, ui_action->elementProperty());
    return a;
}

/*!
    \internal
*/
QActionGroup *QAbstractFormBuilder::create(DomActionGroup *ui_action_group, QObject *parent)
{
    QActionGroup *a = createActionGroup(parent, ui_action_group->attributeName());
    if (!a)
        return 0;
    m_actionGroups.insert(ui_action_group->attributeName(), a);
    applyProperties(a, ui_action_group->elementProperty());

    foreach (DomAction *ui_action, ui_action_group->elementAction()) {
        QAction *child_action = create(ui_action, a);
        Q_UNUSED( child_action );
    }

    foreach (DomActionGroup *g, ui_action_group->elementActionGroup()) {
        QActionGroup *child_action_group = create(g, parent);
        Q_UNUSED( child_action_group );
    }

    return a;
}

// figure out the toolbar area of a DOM attrib list.
// By legacy, it is stored as an integer. As of 4.3.0, it is the enumeration value.
Qt::ToolBarArea QAbstractFormBuilder::toolbarAreaFromDOMAttributes(const DomPropertyHash &attributes) {
    const DomProperty *attr = attributes.value(QFormBuilderStrings::instance().toolBarAreaAttribute);
    if (!attr)
        return Qt::TopToolBarArea;
    switch(attr->kind()) {
    case DomProperty::Number:
        return static_cast<Qt::ToolBarArea>(attr->elementNumber());
    case DomProperty::Enum:
        return enumKeyOfObjectToValue<QAbstractFormBuilderGadget, Qt::ToolBarArea>("toolBarArea",  attr->elementEnum().toLatin1());
    default:
        break;
    }
    return Qt::TopToolBarArea;
}

/*!
    \internal
*/
bool QAbstractFormBuilder::addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const DomPropertyHash attributes = propertyMap(ui_widget->elementAttribute());

    if (parentWidget == 0)
        return true;
    // Check special cases. First: Custom container
    const QString className = QLatin1String(parentWidget->metaObject()->className());
    const QString addPageMethod = QFormBuilderExtra::instance(this)->customWidgetAddPageMethod(className);
    if (!addPageMethod.isEmpty()) {
        // If this fails ( non-existent or non-slot), use ContainerExtension in Designer, else it can't be helped
        return QMetaObject::invokeMethod(parentWidget, addPageMethod.toUtf8().constData(), Qt::DirectConnection, Q_ARG(QWidget*, widget));
    }

    if (QMainWindow *mw = qobject_cast<QMainWindow*>(parentWidget)) {

#ifndef QT_NO_MENUBAR
        // the menubar
        if (QMenuBar *menuBar = qobject_cast<QMenuBar*>(widget)) {
            mw->setMenuBar(menuBar);
            return true;
        }
#endif

#ifndef QT_NO_TOOLBAR
        // apply the toolbar's attributes
        else if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) {
            mw->addToolBar(toolbarAreaFromDOMAttributes(attributes), toolBar);
            // check break
            if (const DomProperty *attr = attributes.value(strings.toolBarBreakAttribute))
                if (attr->elementBool() == strings.trueValue)
                    mw->insertToolBarBreak (toolBar);

            return true;
        }
#endif

#ifndef QT_NO_STATUSBAR
        // statusBar
        else if (QStatusBar *statusBar = qobject_cast<QStatusBar*>(widget)) {
            mw->setStatusBar(statusBar);
            return true;
        }
#endif

#ifndef QT_NO_DOCKWIDGET
        // apply the dockwidget's attributes
        else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(widget)) {
            if (const DomProperty *attr = attributes.value(strings.dockWidgetAreaAttribute)) {
                Qt::DockWidgetArea area = static_cast<Qt::DockWidgetArea>(attr->elementNumber());
                if (!dockWidget->isAreaAllowed(area)) {
                    if (dockWidget->isAreaAllowed(Qt::LeftDockWidgetArea))
                        area = Qt::LeftDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::RightDockWidgetArea))
                        area = Qt::RightDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::TopDockWidgetArea))
                        area = Qt::TopDockWidgetArea;
                    else if (dockWidget->isAreaAllowed(Qt::BottomDockWidgetArea))
                        area = Qt::BottomDockWidgetArea;
                }
                mw->addDockWidget(area, dockWidget);
            } else {
                mw->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
            }
            return true;
        }
#endif

        else if (! mw->centralWidget()) {
            mw->setCentralWidget(widget);
            return true;
        }
    }

#ifndef QT_NO_TABWIDGET
    else if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(parentWidget)) {
        widget->setParent(0);

        const int tabIndex = tabWidget->count();
        if (const DomProperty *titleP = attributes.value(strings.titleAttribute, 0))
            tabWidget->addTab(widget, toString(titleP->elementString()));
        else
            tabWidget->addTab(widget, strings.defaultTitle);

        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            tabWidget->setTabIcon(tabIndex, qvariant_cast<QIcon>(nativeValue));
        }

#ifndef QT_NO_TOOLTIP
        if (const DomProperty *ptoolTip = attributes.value(strings.toolTipAttribute)) {
            tabWidget->setTabToolTip(tabIndex, toString(ptoolTip->elementString()));
        }
#endif

#ifndef QT_NO_WHATSTHIS
        if (const DomProperty *pwhatsThis = attributes.value(strings.whatsThisAttribute)) {
            tabWidget->setTabWhatsThis(tabIndex, toString(pwhatsThis->elementString()));
        }
#endif

        return true;
    }
#endif

#ifndef QT_NO_TOOLBOX
    else if (QToolBox *toolBox = qobject_cast<QToolBox*>(parentWidget)) {
        const int tabIndex = toolBox->count();
        if (const DomProperty *labelP =  attributes.value(strings.labelAttribute, 0))
            toolBox->addItem(widget, toString(labelP->elementString()));
        else
            toolBox->addItem(widget, strings.defaultTitle);

        if (DomProperty *picon = attributes.value(strings.iconAttribute)) {
            QVariant v = resourceBuilder()->loadResource(workingDirectory(), picon);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            toolBox->setItemIcon(tabIndex, qvariant_cast<QIcon>(nativeValue));
        }

#ifndef QT_NO_TOOLTIP
        if (const DomProperty *ptoolTip = attributes.value(strings.toolTipAttribute)) {
            toolBox->setItemToolTip(tabIndex, toString(ptoolTip->elementString()));
        }
#endif

        return true;
    }
#endif

#ifndef QT_NO_STACKEDWIDGET
    else if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(parentWidget)) {
        stackedWidget->addWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_SPLITTER
    else if (QSplitter *splitter = qobject_cast<QSplitter*>(parentWidget)) {
        splitter->addWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_MDIAREA
    else if (QMdiArea *mdiArea = qobject_cast<QMdiArea*>(parentWidget)) {
        mdiArea->addSubWindow(widget);
        return true;
    }
#endif

#ifndef QT_NO_WORKSPACE
    else if (QWorkspace *ws = qobject_cast<QWorkspace*>(parentWidget)) {
        ws->addWindow(widget);
        return true;
    }
#endif

#ifndef QT_NO_DOCKWIDGET
    else if (QDockWidget *dockWidget = qobject_cast<QDockWidget*>(parentWidget)) {
        dockWidget->setWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_SCROLLAREA
    else if (QScrollArea *scrollArea = qobject_cast<QScrollArea*>(parentWidget)) {
        scrollArea->setWidget(widget);
        return true;
    }
#endif

#ifndef QT_NO_WIZARD
     else if (QWizard *wizard = qobject_cast<QWizard *>(parentWidget)) {
         QWizardPage *page = qobject_cast<QWizardPage*>(widget);
         if (!page) {
             uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "Attempt to add child that is not of class QWizardPage to QWizard."));
             return false;
         }
         wizard->addPage(page);
         return true;
     }
#endif
    return false;
}

/*!
    \internal
*/
void QAbstractFormBuilder::layoutInfo(DomLayout *ui_layout, QObject *parent, int *margin, int *spacing)
{
    Q_UNUSED(parent)
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

    int mar = INT_MIN;
    int spac = INT_MIN;
    if (const DomProperty *p = properties.value(strings.marginProperty, 0))
        mar = p->elementNumber();

    if (const DomProperty *p = properties.value(strings.spacingProperty, 0))
        spac = p->elementNumber();

#ifdef Q_OS_MAC
    // here we recognize UI file < 4.3 (no we don't store margin property)
    if (mar != INT_MIN) {
        const int defaultMargin = parent->inherits("QLayoutWidget") ? 0 : 9;
        if (mar == defaultMargin)
            mar = INT_MIN;
        if (spac == 6)
            spac = INT_MIN;

        if (mar == INT_MIN || spac == INT_MIN) {
            QList<DomProperty *> properties = ui_layout->elementProperty();
            QMutableListIterator<DomProperty *> it(properties);
            while (it.hasNext()) {
                DomProperty *prop = it.next();
                if ((mar == INT_MIN && prop->attributeName() == strings.marginProperty) ||
                        (spac == INT_MIN && prop->attributeName() == strings.spacingProperty)) {
                    it.remove();
                    delete prop;
                }
            }
            ui_layout->setElementProperty(properties);
        }
    }
#endif
    if (margin)
        *margin = mar;
    if (spacing)
        *spacing = spac;
}

/*!
    \internal
*/
QLayout *QAbstractFormBuilder::create(DomLayout *ui_layout, QLayout *parentLayout, QWidget *parentWidget)
{
    QObject *p = parentLayout;

    if (p == 0)
        p = parentWidget;

    Q_ASSERT(p != 0);

    bool tracking = false;

    if (p == parentWidget && parentWidget->layout()) {
        tracking = true;
        p = parentWidget->layout();
    }

    QLayout *layout = createLayout(ui_layout->attributeClass(), p, ui_layout->hasAttributeName() ? ui_layout->attributeName() : QString());

    if (layout == 0)
        return 0;

    if (tracking && layout->parent() == 0) {
        QBoxLayout *box = qobject_cast<QBoxLayout*>(parentWidget->layout());
        if (!box) {  // only QBoxLayout is supported
            const QString widgetClass = QString::fromUtf8(parentWidget->metaObject()->className());
            const QString layoutClass = QString::fromUtf8(parentWidget->layout()->metaObject()->className());
            const QString msg = QCoreApplication::translate("QAbstractFormBuilder", "Attempt to add a layout to a widget '%1' (%2) which already has a layout of non-box type %3.\n"
                                            "This indicates an inconsistency in the ui-file.").
                                            arg(parentWidget->objectName(), widgetClass, layoutClass);
            uiLibWarning(msg);
            return 0;
        }
        box->addLayout(layout);
    }

    int margin = INT_MIN, spacing = INT_MIN;
    layoutInfo(ui_layout, p, &margin, &spacing);

    if (margin != INT_MIN) {
        layout->setMargin(margin);
    } else {
        const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
        int left, top, right, bottom;
        left = top = right = bottom = -1;
        layout->getContentsMargins(&left, &top, &right, &bottom);

        const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

        if (const DomProperty *p = properties.value(strings.leftMarginProperty, 0))
            left = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.topMarginProperty, 0))
            top = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.rightMarginProperty, 0))
            right = p->elementNumber();

        if (const DomProperty *p = properties.value(strings.bottomMarginProperty, 0))
            bottom = p->elementNumber();

        layout->setContentsMargins(left, top, right, bottom);
    }

    if (spacing != INT_MIN) {
        layout->setSpacing(spacing);
    } else {
        QGridLayout *grid = qobject_cast<QGridLayout *>(layout);
        if (grid) {
            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
            const DomPropertyHash properties = propertyMap(ui_layout->elementProperty());

            if (const DomProperty *p = properties.value(strings.horizontalSpacingProperty, 0))
                grid->setHorizontalSpacing(p->elementNumber());
            if (const DomProperty *p = properties.value(strings.verticalSpacingProperty, 0))
                grid->setVerticalSpacing(p->elementNumber());
        }
    }

    applyProperties(layout, ui_layout->elementProperty());

    foreach (DomLayoutItem *ui_item, ui_layout->elementItem()) {
        if (QLayoutItem *item = create(ui_item, layout, parentWidget)) {
            addItem(ui_item, item, layout);
        }
    }
    // Check the box stretch attributes
    if (QBoxLayout *box = qobject_cast<QBoxLayout*>(layout)) {
        const QString boxStretch = ui_layout->attributeStretch();
        if (!boxStretch.isEmpty())
            QFormBuilderExtra::setBoxLayoutStretch(boxStretch, box);
    }
    // Check the grid stretch/minimum size attributes
    if (QGridLayout *grid = qobject_cast<QGridLayout*>(layout)) {
        // Stretch
        const QString gridRowStretch = ui_layout->attributeRowStretch();
        if (!gridRowStretch.isEmpty())
            QFormBuilderExtra::setGridLayoutRowStretch(gridRowStretch, grid);
        const QString gridColumnStretch = ui_layout->attributeColumnStretch();
        if (!gridColumnStretch.isEmpty())
            QFormBuilderExtra::setGridLayoutColumnStretch(gridColumnStretch, grid);
        // Minimum size
        const QString gridColumnMinimumWidth = ui_layout->attributeColumnMinimumWidth();
        if (!gridColumnMinimumWidth.isEmpty())
            QFormBuilderExtra::setGridLayoutColumnMinimumWidth(gridColumnMinimumWidth, grid);
        const QString gridRowMinimumHeight = ui_layout->attributeRowMinimumHeight();
        if (!gridRowMinimumHeight.isEmpty())
            QFormBuilderExtra::setGridLayoutRowMinimumHeight(gridRowMinimumHeight, grid);
    }
    return layout;
}

#ifndef QT_NO_FORMLAYOUT
static inline QFormLayout::ItemRole formLayoutRole(int column, int colspan)
{
    if (colspan > 1)
        return QFormLayout::SpanningRole;
    return column == 0 ? QFormLayout::LabelRole : QFormLayout::FieldRole;
}
#endif

/*!
    \internal
*/
bool QAbstractFormBuilder::addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout)
{
    // Calling addChildWidget(), etc.  is required to maintain consistency of the layouts,
    // see documentation of addItem(), which should ideally not be used.
    if (item->widget()) {
        static_cast<QFriendlyLayout*>(layout)->addChildWidget(item->widget());
    } else if (item->layout()) {
        static_cast<QFriendlyLayout*>(layout)->addChildLayout(item->layout());
    } else if (item->spacerItem()) {
        // nothing to do
    } else {
        return false;
    }

    if (QGridLayout *grid = qobject_cast<QGridLayout*>(layout)) {
        const int rowSpan = ui_item->hasAttributeRowSpan() ? ui_item->attributeRowSpan() : 1;
        const int colSpan = ui_item->hasAttributeColSpan() ? ui_item->attributeColSpan() : 1;
        grid->addItem(item, ui_item->attributeRow(), ui_item->attributeColumn(),
                        rowSpan, colSpan, item->alignment());
        return true;
    }
#ifndef QT_NO_FORMLAYOUT
    if (QFormLayout *form = qobject_cast<QFormLayout *>(layout)) {
        const int row =  ui_item->attributeRow();
        const int colSpan = ui_item->hasAttributeColSpan() ? ui_item->attributeColSpan() : 1;
        form->setItem(row, formLayoutRole(ui_item->attributeColumn(), colSpan), item);
        return true;
    }

#endif
    layout->addItem(item);
    return true;
}

/*!
    \internal
*/
QLayoutItem *QAbstractFormBuilder::create(DomLayoutItem *ui_layoutItem, QLayout *layout, QWidget *parentWidget)
{
    switch (ui_layoutItem->kind()) {
    case DomLayoutItem::Widget: {
        if (QWidget *w = create(ui_layoutItem->elementWidget(), parentWidget))
#ifdef QFORMINTERNAL_NAMESPACE // uilib
            return new QWidgetItemV2(w);
#else                         // Within Designer: Use factory method that returns special items that refuse to shrink to 0,0
            return QLayoutPrivate::createWidgetItem(layout, w);
#endif
        qWarning() << QCoreApplication::translate("QAbstractFormBuilder", "Empty widget item in %1 '%2'.").arg(QString::fromUtf8(layout->metaObject()->className()), layout->objectName());
        return 0;
    }
    case DomLayoutItem::Spacer: {
        QSize size(0, 0);
        QSizePolicy::Policy sizeType = QSizePolicy::Expanding;
        bool isVspacer = false;

        const DomSpacer *ui_spacer = ui_layoutItem->elementSpacer();

        const QMetaEnum sizePolicy_enum  = metaEnum<QAbstractFormBuilderGadget>("sizeType");
        const QMetaEnum orientation_enum =  metaEnum<QAbstractFormBuilderGadget>("orientation");

        const QList<DomProperty *> spacerProperties =  ui_spacer->elementProperty();
        if (!spacerProperties.empty()) {
            const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
            foreach (DomProperty *p, spacerProperties) {
                const QVariant v = toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p); // ### remove me
                if (v.isNull())
                    continue;
                if (p->attributeName() == strings.sizeHintProperty && p->kind() == DomProperty::Size) {
                    size = v.toSize();  // ###  remove me
                } else if (p->attributeName() == strings.sizeTypeProperty && p->kind() == DomProperty::Enum) {
                    sizeType = static_cast<QSizePolicy::Policy>(v.toInt());
                } else if (p->attributeName() == strings.orientationProperty && p->kind() == DomProperty::Enum) {
                    const Qt::Orientation o = static_cast<Qt::Orientation>(v.toInt());
                    isVspacer = (o == Qt::Vertical);
                }
            }
        }

        QSpacerItem *spacer = 0;
        if (isVspacer)
            spacer = new QSpacerItem(size.width(), size.height(), QSizePolicy::Minimum, sizeType);
        else
            spacer = new QSpacerItem(size.width(), size.height(), sizeType, QSizePolicy::Minimum);
        return spacer; }

    case DomLayoutItem::Layout:
        return create(ui_layoutItem->elementLayout(), layout, parentWidget);

    default:
        break;
    }

    return 0;
}

/*!
    \internal
*/
void QAbstractFormBuilder::applyProperties(QObject *o, const QList<DomProperty*> &properties)
{
    typedef QList<DomProperty*> DomPropertyList;

    if (properties.empty())
        return;

    QFormBuilderExtra *fb = QFormBuilderExtra::instance(this);

    const DomPropertyList::const_iterator cend = properties.constEnd();
    for (DomPropertyList::const_iterator it = properties.constBegin(); it != cend; ++it) {
        const QVariant v = toVariant(o->metaObject(), *it);
        if (!v.isNull()) {
            const  QString attributeName = (*it)->attributeName();
            if (!fb->applyPropertyInternally(o, attributeName, v))
                o->setProperty(attributeName.toUtf8(), v);
        }
    }
}


/*!
    \internal
    Check whether a property is applied internally by QAbstractFormBuilder. Call this
   from overwritten applyProperties().
*/

bool QAbstractFormBuilder::applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value)
{
    return QFormBuilderExtra::instance(this)->applyPropertyInternally(o,propertyName, value);
}

/*!
    \internal
*/

QVariant QAbstractFormBuilder::toVariant(const QMetaObject *meta, DomProperty *p)
{
    return domPropertyToVariant(this, meta, p);
}

/*!
    \internal
*/
void QAbstractFormBuilder::setupColorGroup(QPalette &palette, QPalette::ColorGroup colorGroup,
            DomColorGroup *group)
{
    // old format
    const QList<DomColor*> colors = group->elementColor();
    for (int role = 0; role < colors.size(); ++role) {
        const DomColor *color = colors.at(role);
        const QColor c(color->elementRed(), color->elementGreen(), color->elementBlue());
        palette.setColor(colorGroup, QPalette::ColorRole(role), c);
    }

    // new format
    const QMetaEnum colorRole_enum = metaEnum<QAbstractFormBuilderGadget>("colorRole");

    const QList<DomColorRole*> colorRoles = group->elementColorRole();
    for (int role = 0; role < colorRoles.size(); ++role) {
        const DomColorRole *colorRole = colorRoles.at(role);

        if (colorRole->hasAttributeRole()) {
            const int r = colorRole_enum.keyToValue(colorRole->attributeRole().toLatin1());
            if (r != -1) {
                const QBrush br = setupBrush(colorRole->elementBrush());
                palette.setBrush(colorGroup, static_cast<QPalette::ColorRole>(r), br);
            }
        }
    }
}

/*!
    \internal
*/
DomColorGroup *QAbstractFormBuilder::saveColorGroup(const QPalette &palette)
{

    const QMetaEnum colorRole_enum = metaEnum<QAbstractFormBuilderGadget>("colorRole");

    DomColorGroup *group = new DomColorGroup();
    QList<DomColorRole*> colorRoles;

    const uint mask = palette.resolve();
    for (int role = QPalette::WindowText; role < QPalette::NColorRoles; ++role) {
        if (mask & (1 << role)) {
            QBrush br = palette.brush(QPalette::ColorRole(role));

            DomColorRole *colorRole = new DomColorRole();
            colorRole->setElementBrush(saveBrush(br));
            colorRole->setAttributeRole(QLatin1String(colorRole_enum.valueToKey(role)));
            colorRoles.append(colorRole);
        }
    }

    group->setElementColorRole(colorRoles);
    return group;
}

/*!
    \internal
*/
QBrush QAbstractFormBuilder::setupBrush(DomBrush *brush)
{
    QBrush br;
    if (!brush->hasAttributeBrushStyle())
        return br;

    const Qt::BrushStyle style = enumKeyOfObjectToValue<QAbstractFormBuilderGadget, Qt::BrushStyle>("brushStyle", brush->attributeBrushStyle().toLatin1());

    if (style == Qt::LinearGradientPattern ||
            style == Qt::RadialGradientPattern ||
            style == Qt::ConicalGradientPattern) {
        const QMetaEnum gradientType_enum = metaEnum<QAbstractFormBuilderGadget>("gradientType");
        const QMetaEnum gradientSpread_enum = metaEnum<QAbstractFormBuilderGadget>("gradientSpread");
        const QMetaEnum gradientCoordinate_enum = metaEnum<QAbstractFormBuilderGadget>("gradientCoordinate");

        const DomGradient *gradient = brush->elementGradient();
        const QGradient::Type type = enumKeyToValue<QGradient::Type>(gradientType_enum, gradient->attributeType().toLatin1());


        QGradient *gr = 0;

        if (type == QGradient::LinearGradient) {
            gr = new QLinearGradient(QPointF(gradient->attributeStartX(), gradient->attributeStartY()),
                            QPointF(gradient->attributeEndX(), gradient->attributeEndY()));
        } else if (type == QGradient::RadialGradient) {
            gr = new QRadialGradient(QPointF(gradient->attributeCentralX(), gradient->attributeCentralY()),
                            gradient->attributeRadius(),
                            QPointF(gradient->attributeFocalX(), gradient->attributeFocalY()));
        } else if (type == QGradient::ConicalGradient) {
            gr = new QConicalGradient(QPointF(gradient->attributeCentralX(), gradient->attributeCentralY()),
                            gradient->attributeAngle());
        }
        if (!gr)
            return br;

        const QGradient::Spread spread = enumKeyToValue<QGradient::Spread>(gradientSpread_enum, gradient->attributeSpread().toLatin1());
        gr->setSpread(spread);

        const QGradient::CoordinateMode coord = enumKeyToValue<QGradient::CoordinateMode>(gradientCoordinate_enum, gradient->attributeCoordinateMode().toLatin1());
        gr->setCoordinateMode(coord);

        const QList<DomGradientStop *> stops = gradient->elementGradientStop();
        QListIterator<DomGradientStop *> it(stops);
        while (it.hasNext()) {
            const DomGradientStop *stop = it.next();
            const DomColor *color = stop->elementColor();
            gr->setColorAt(stop->attributePosition(), QColor::fromRgb(color->elementRed(),
                            color->elementGreen(), color->elementBlue(), color->attributeAlpha()));
        }
        br = QBrush(*gr);
        delete gr;
    } else if (style == Qt::TexturePattern) {
        const DomProperty *texture = brush->elementTexture();
        if (texture && texture->kind() == DomProperty::Pixmap) {
            br.setTexture(domPropertyToPixmap(texture));
        }
    } else {
        const DomColor *color = brush->elementColor();
        br.setColor(QColor::fromRgb(color->elementRed(),
                            color->elementGreen(), color->elementBlue(), color->attributeAlpha()));
        br.setStyle((Qt::BrushStyle)style);
    }
    return br;
}

/*!
    \internal
*/
DomBrush *QAbstractFormBuilder::saveBrush(const QBrush &br)
{
    const QMetaEnum brushStyle_enum = metaEnum<QAbstractFormBuilderGadget>("brushStyle");

    DomBrush *brush = new DomBrush();
    const Qt::BrushStyle style = br.style();
    brush->setAttributeBrushStyle(QLatin1String(brushStyle_enum.valueToKey(style)));
    if (style == Qt::LinearGradientPattern ||
                style == Qt::RadialGradientPattern ||
                style == Qt::ConicalGradientPattern) {
        const QMetaEnum gradientType_enum = metaEnum<QAbstractFormBuilderGadget>("gradientType");
        const QMetaEnum gradientSpread_enum = metaEnum<QAbstractFormBuilderGadget>("gradientSpread");
        const QMetaEnum gradientCoordinate_enum = metaEnum<QAbstractFormBuilderGadget>("gradientCoordinate");

        DomGradient *gradient = new DomGradient();
        const QGradient *gr = br.gradient();
        const QGradient::Type type = gr->type();
        gradient->setAttributeType(QLatin1String(gradientType_enum.valueToKey(type)));
        gradient->setAttributeSpread(QLatin1String(gradientSpread_enum.valueToKey(gr->spread())));
        gradient->setAttributeCoordinateMode(QLatin1String(gradientCoordinate_enum.valueToKey(gr->coordinateMode())));
        QList<DomGradientStop *> stops;
        QGradientStops st = gr->stops();
        QVectorIterator<QPair<qreal, QColor> > it(st);
        while (it.hasNext()) {
            const QPair<qreal, QColor> pair = it.next();
            DomGradientStop *stop = new DomGradientStop();
            stop->setAttributePosition(pair.first);
            DomColor *color = new DomColor();
            color->setElementRed(pair.second.red());
            color->setElementGreen(pair.second.green());
            color->setElementBlue(pair.second.blue());
            color->setAttributeAlpha(pair.second.alpha());
            stop->setElementColor(color);
            stops.append(stop);
        }
        gradient->setElementGradientStop(stops);
        if (type == QGradient::LinearGradient) {
            QLinearGradient *lgr = (QLinearGradient *)(gr);
            gradient->setAttributeStartX(lgr->start().x());
            gradient->setAttributeStartY(lgr->start().y());
            gradient->setAttributeEndX(lgr->finalStop().x());
            gradient->setAttributeEndY(lgr->finalStop().y());
        } else if (type == QGradient::RadialGradient) {
            QRadialGradient *rgr = (QRadialGradient *)(gr);
            gradient->setAttributeCentralX(rgr->center().x());
            gradient->setAttributeCentralY(rgr->center().y());
            gradient->setAttributeFocalX(rgr->focalPoint().x());
            gradient->setAttributeFocalY(rgr->focalPoint().y());
            gradient->setAttributeRadius(rgr->radius());
        } else if (type == QGradient::ConicalGradient) {
            QConicalGradient *cgr = (QConicalGradient *)(gr);
            gradient->setAttributeCentralX(cgr->center().x());
            gradient->setAttributeCentralY(cgr->center().y());
            gradient->setAttributeAngle(cgr->angle());
        }

        brush->setElementGradient(gradient);
    } else if (style == Qt::TexturePattern) {
        const QPixmap pixmap = br.texture();
        if (!pixmap.isNull()) {
            DomProperty *p = new DomProperty;
            setPixmapProperty(*p,  pixmapPaths(pixmap));
            brush->setElementTexture(p);
        }
    } else {
        QColor c = br.color();
        DomColor *color = new DomColor();
        color->setElementRed(c.red());
        color->setElementGreen(c.green());
        color->setElementBlue(c.blue());
        color->setAttributeAlpha(c.alpha());
        brush->setElementColor(color);
    }
    return brush;
}

/*!
    \internal
*/
QWidget *QAbstractFormBuilder::createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name)
{
    Q_UNUSED(widgetName);
    Q_UNUSED(parentWidget);
    Q_UNUSED(name);
    return 0;
}

/*!
    \internal
*/
QLayout *QAbstractFormBuilder::createLayout(const QString &layoutName, QObject *parent, const QString &name)
{
    Q_UNUSED(layoutName);
    Q_UNUSED(parent);
    Q_UNUSED(name);
    return 0;
}

/*!
    \internal
*/
QAction *QAbstractFormBuilder::createAction(QObject *parent, const QString &name)
{
    QAction *action = new QAction(parent);
    action->setObjectName(name);
    return action;
}

/*!
    \internal
*/
QActionGroup *QAbstractFormBuilder::createActionGroup(QObject *parent, const QString &name)
{
    QActionGroup *g = new QActionGroup(parent);
    g->setObjectName(name);
    return g;
}

/*!
    \fn void QAbstractFormBuilder::save(QIODevice *device, QWidget *widget)

    Saves an XML representation of the given \a widget to the
    specified \a device in the standard UI file format.

    \sa load()*/
void QAbstractFormBuilder::save(QIODevice *dev, QWidget *widget)
{
    DomWidget *ui_widget = createDom(widget, 0);
    Q_ASSERT( ui_widget != 0 );

    DomUI *ui = new DomUI();
    ui->setAttributeVersion(QLatin1String("4.0"));
    ui->setElementWidget(ui_widget);

    saveDom(ui, widget);

    QXmlStreamWriter writer(dev);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(1);
    writer.writeStartDocument();
    ui->write(writer);
    writer.writeEndDocument();

    m_laidout.clear();

    delete ui;
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveDom(DomUI *ui, QWidget *widget)
{
    ui->setElementClass(widget->objectName());

    if (DomConnections *ui_connections = saveConnections()) {
        ui->setElementConnections(ui_connections);
    }

    if (DomCustomWidgets *ui_customWidgets = saveCustomWidgets()) {
        ui->setElementCustomWidgets(ui_customWidgets);
    }

    if (DomTabStops *ui_tabStops = saveTabStops()) {
        ui->setElementTabStops(ui_tabStops);
    }

    if (DomResources *ui_resources = saveResources()) {
        ui->setElementResources(ui_resources);
    }
    if (DomButtonGroups *ui_buttonGroups = saveButtonGroups(widget))
        ui->setElementButtonGroups(ui_buttonGroups);
}

/*!
    \internal
*/
DomConnections *QAbstractFormBuilder::saveConnections()
{
    return new DomConnections;
}

/*!
    \internal
*/

DomWidget *QAbstractFormBuilder::createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive)
{
    DomWidget *ui_widget = new DomWidget();
    ui_widget->setAttributeClass(QLatin1String(widget->metaObject()->className()));
    ui_widget->setElementProperty(computeProperties(widget));

    if (recursive) {
        if (QLayout *layout = widget->layout()) {
            if (DomLayout *ui_layout = createDom(layout, 0, ui_parentWidget)) {
                QList<DomLayout*> ui_layouts;
                ui_layouts.append(ui_layout);

                ui_widget->setElementLayout(ui_layouts);
            }
        }
    }

    // widgets, actions and action groups
    QList<DomWidget*> ui_widgets;
    QList<DomAction*> ui_actions;
    QList<DomActionGroup*> ui_action_groups;

    QList<QObject*> children;

    // splitters need to store their children in the order specified by child indexes,
    // not the order of the child list.
#ifndef QT_NO_SPLITTER
    if (const QSplitter *splitter = qobject_cast<const QSplitter*>(widget)) {
        const int count = splitter->count();
        for (int i = 0; i < count; ++i)
            children.append(splitter->widget(i));
    } else
#endif
    {
        QList<QObject *> childObjects = widget->children();

        const QList<QWidget *> list = qVariantValue<QWidgetList>(widget->property("_q_widgetOrder"));
        foreach (QWidget *w, list) {
            if (childObjects.contains(w)) {
                children.append(w);
                childObjects.removeAll(w);
            }
        }
        children += childObjects;

        const QList<QWidget *> zOrder = qVariantValue<QWidgetList>(widget->property("_q_zOrder"));
        if (list != zOrder) {
            QStringList zOrderList;
            QListIterator<QWidget* > itZOrder(zOrder);
            while (itZOrder.hasNext())
                zOrderList.append(itZOrder.next()->objectName());
            ui_widget->setElementZOrder(zOrderList);
        }
    }

    foreach (QObject *obj, children) {
        if (QWidget *childWidget = qobject_cast<QWidget*>(obj)) {
            if (m_laidout.contains(childWidget) || recursive == false)
                continue;

            if (QMenu *menu = qobject_cast<QMenu *>(childWidget)) {
                QList<QAction *> actions = menu->parentWidget()->actions();
                QListIterator<QAction *> it(actions);
                bool found = false;
                while (it.hasNext()) {
                    if (it.next()->menu() == menu)
                        found = true;
                }
                if (!found)
                    continue;
            }

            if (DomWidget *ui_child = createDom(childWidget, ui_widget)) {
                ui_widgets.append(ui_child);
            }
        } else if (QAction *childAction = qobject_cast<QAction*>(obj)) {
            if (childAction->actionGroup() != 0) {
                // it will be added later.
                continue;
            }

            if (DomAction *ui_action = createDom(childAction)) {
                ui_actions.append(ui_action);
            }
        } else if (QActionGroup *childActionGroup = qobject_cast<QActionGroup*>(obj)) {
            if (DomActionGroup *ui_action_group = createDom(childActionGroup)) {
                ui_action_groups.append(ui_action_group);
            }
        }
    }

    // add-action
    QList<DomActionRef*> ui_action_refs;
    foreach (QAction *action, widget->actions()) {
        if (DomActionRef *ui_action_ref = createActionRefDom(action)) {
            ui_action_refs.append(ui_action_ref);
        }
    }

    if (recursive)
        ui_widget->setElementWidget(ui_widgets);

    ui_widget->setElementAction(ui_actions);
    ui_widget->setElementActionGroup(ui_action_groups);
    ui_widget->setElementAddAction(ui_action_refs);

    saveExtraInfo(widget, ui_widget, ui_parentWidget);

    return ui_widget;
}

/*!
    \internal
*/
DomActionRef *QAbstractFormBuilder::createActionRefDom(QAction *action)
{
    QString name = action->objectName();

    if (action->menu() != 0)
        name = action->menu()->objectName();

    DomActionRef *ui_action_ref = new DomActionRef();
    if (action->isSeparator())
        ui_action_ref->setAttributeName(QFormBuilderStrings::instance().separator);
    else
        ui_action_ref->setAttributeName(name);

    return ui_action_ref;
}

/*!
    \internal
*/
DomLayout *QAbstractFormBuilder::createDom(QLayout *layout, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_layout)
    DomLayout *lay = new DomLayout();
    lay->setAttributeClass(QLatin1String(layout->metaObject()->className()));
    const QString objectName = layout->objectName();
    if (!objectName.isEmpty())
        lay->setAttributeName(objectName);
    lay->setElementProperty(computeProperties(layout));

    QList<DomLayoutItem*> ui_items;

    QMap<QObject *, QLayoutItem *> objectToItem;
    QList<QLayoutItem *> spacerItems;
    QList<QLayoutItem *> newList;

    for (int idx=0; layout->itemAt(idx); ++idx) {
        QLayoutItem *item = layout->itemAt(idx);
        if (item->widget())
            objectToItem[item->widget()] = item;
        else if (item->layout())
            objectToItem[item->layout()] = item;
        else if (item->spacerItem())
            spacerItems.append(item);
        newList.append(item);
    }

    if (qobject_cast<QGridLayout *>(layout)) {
        newList.clear();
        QList<QObject *> childrenList = layout->parentWidget()->children();
        foreach (QObject *o, childrenList) {
            if (objectToItem.contains(o))
                newList.append(objectToItem[o]);
        }
        newList += spacerItems;
    }

    foreach (QLayoutItem *item, newList) {
        DomLayoutItem *ui_item = createDom(item, lay, ui_parentWidget);
        if (ui_item)
            ui_items.append(ui_item);
    }

    lay->setElementItem(ui_items);

    return lay;
}

/*!
    \internal
*/
DomLayoutItem *QAbstractFormBuilder::createDom(QLayoutItem *item, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    DomLayoutItem *ui_item = new DomLayoutItem();

    if (item->widget())  {
        ui_item->setElementWidget(createDom(item->widget(), ui_parentWidget));
        m_laidout.insert(item->widget(), true);
    } else if (item->layout()) {
        ui_item->setElementLayout(createDom(item->layout(), ui_layout, ui_parentWidget));
    } else if (item->spacerItem()) {
        ui_item->setElementSpacer(createDom(item->spacerItem(), ui_layout, ui_parentWidget));
    }

    return ui_item;
}

/*!
    \internal
*/
DomSpacer *QAbstractFormBuilder::createDom(QSpacerItem *spacer, DomLayout *ui_layout, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_layout);
    Q_UNUSED(ui_parentWidget);

    DomSpacer *ui_spacer = new DomSpacer();
    QList<DomProperty*> properties;

    DomProperty *prop = 0;
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    // sizeHint property
    prop = new DomProperty();
    prop->setAttributeName(strings.sizeHintProperty);
    prop->setElementSize(new DomSize());
    prop->elementSize()->setElementWidth(spacer->sizeHint().width());
    prop->elementSize()->setElementHeight(spacer->sizeHint().height());
    properties.append(prop);

    // orientation property
    prop = new DomProperty(); // ### we don't implemented the case where expandingDirections() is both Vertical and Horizontal
    prop->setAttributeName(strings.orientationProperty);
    prop->setElementEnum((spacer->expandingDirections() & Qt::Horizontal) ? strings.qtHorizontal : strings.qtVertical);
    properties.append(prop);

    ui_spacer->setElementProperty(properties);
    return ui_spacer;
}

/*!
    \internal
*/
DomProperty *QAbstractFormBuilder::createProperty(QObject *obj, const QString &pname, const QVariant &v)
{
    if (!checkProperty(obj, pname)) {
        return 0;
    }
    return variantToDomProperty(this, obj->metaObject(), pname, v);
}

/*!
    \internal
*/
QList<DomProperty*> QAbstractFormBuilder::computeProperties(QObject *obj)
{
    QList<DomProperty*> lst;

    const QMetaObject *meta = obj->metaObject();

    QHash<QByteArray, bool> properties;
    const int propertyCount = meta->propertyCount();
    for(int i=0; i < propertyCount; ++i)
        properties.insert(meta->property(i).name(), true);

    const QList<QByteArray> propertyNames = properties.keys();

    const int propertyNamesCount = propertyNames.size();
    for(int i=0; i<propertyNamesCount ; ++i) {
        const QString pname = QString::fromUtf8(propertyNames.at(i));
        const QMetaProperty prop = meta->property(meta->indexOfProperty(pname.toUtf8()));

        if (!prop.isWritable() || !checkProperty(obj, QLatin1String(prop.name())))
            continue;

        const QVariant v = prop.read(obj);

        DomProperty *dom_prop = 0;
        if (v.type() == QVariant::Int) {
            dom_prop = new DomProperty();

            if (prop.isFlagType())
                uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "Flags property are not supported yet."));

            if (prop.isEnumType()) {
                QString scope = QString::fromUtf8(prop.enumerator().scope());
                if (scope.size())
                    scope += QString::fromUtf8("::");
                const QString e = QString::fromUtf8(prop.enumerator().valueToKey(v.toInt()));
                if (e.size())
                    dom_prop->setElementEnum(scope + e);
            } else
                dom_prop->setElementNumber(v.toInt());
            dom_prop->setAttributeName(pname);
        } else {
            dom_prop = createProperty(obj, pname, v);
        }

        if (!dom_prop || dom_prop->kind() == DomProperty::Unknown)
            delete dom_prop;
        else
            lst.append(dom_prop);
    }

    return lst;
}


/*!
   \internal
   \typedef QAbstractFormBuilder::DomPropertyHash
   \typedef QAbstractFormBuilder::IconPaths
*/


/*!
    \internal
*/
QAbstractFormBuilder::DomPropertyHash QAbstractFormBuilder::propertyMap(const QList<DomProperty*> &properties)
{
    DomPropertyHash map;

    foreach (DomProperty *p, properties)
        map.insert(p->attributeName(), p);

    return map;
}

/*!
    \internal
*/
bool QAbstractFormBuilder::checkProperty(QObject *obj, const QString &prop) const
{
    Q_UNUSED(obj);
    Q_UNUSED(prop);

    return true;
}

/*!
    \internal
*/
QString QAbstractFormBuilder::toString(const DomString *str)
{
    return str ? str->text() : QString();
}

/*!
    \internal
*/
void QAbstractFormBuilder::applyTabStops(QWidget *widget, DomTabStops *tabStops)
{
    if (!tabStops)
        return;

    QWidget *lastWidget = 0;

    const QStringList l = tabStops->elementTabStop();
    for (int i=0; i<l.size(); ++i) {
        const QString name = l.at(i);

        QWidget *child = qFindChild<QWidget*>(widget, name);
        if (!child) {
            uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "While applying tab stops: The widget '%1' could not be found.").arg(name));
            continue;
        }

        if (i == 0) {
            lastWidget = qFindChild<QWidget*>(widget, name);
            continue;
        } else if (!child || !lastWidget) {
            continue;
        }

        QWidget::setTabOrder(lastWidget, child);

        lastWidget = qFindChild<QWidget*>(widget, name);
    }
}

/*!
    \internal
*/
DomCustomWidgets *QAbstractFormBuilder::saveCustomWidgets()
{
    return 0;
}

/*!
    \internal
*/
DomTabStops *QAbstractFormBuilder::saveTabStops()
{
    return 0;
}

/*!
    \internal
*/
DomResources *QAbstractFormBuilder::saveResources()
{
    return 0;
}

/*!
    \internal
    \since 4.5
*/

DomButtonGroups *QAbstractFormBuilder::saveButtonGroups(const QWidget *mainContainer)
{
    // Save fst order buttongroup children of maincontainer
    typedef QList<QButtonGroup*> ButtonGroupList;
    const QObjectList mchildren = mainContainer->children();
    if (mchildren.empty())
        return 0;
    QList<DomButtonGroup*> domGroups;
    const QObjectList::const_iterator cend = mchildren.constEnd();
    for (QObjectList::const_iterator it = mchildren.constBegin(); it != cend; ++it)
        if (QButtonGroup *bg = qobject_cast<QButtonGroup *>(*it))
            if (DomButtonGroup* dg = createDom(bg))
                domGroups.push_back(dg);

    if (domGroups.empty())
        return 0;
    DomButtonGroups *rc = new DomButtonGroups;
    rc->setElementButtonGroup(domGroups);
    return rc;
}

// VC6 would not find templated members, so we use statics and this utter hack.
class FriendlyFB : public QAbstractFormBuilder {
public:
    using QAbstractFormBuilder::saveResource;
    using QAbstractFormBuilder::saveText;
    using QAbstractFormBuilder::resourceBuilder;
    using QAbstractFormBuilder::textBuilder;
    using QAbstractFormBuilder::toVariant;
};

template<class T>
static void storeItemFlags(const T *item, QList<DomProperty*> *properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    static const Qt::ItemFlags defaultFlags = T().flags();
    static const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");

    if (item->flags() != defaultFlags) {
        DomProperty *p = new DomProperty;
        p->setAttributeName(strings.flagsAttribute);
        p->setElementSet(QString::fromAscii(itemFlags_enum.valueToKeys(item->flags())));
        properties->append(p);
    }
}

#ifndef Q_CC_RVCT
// RVCT does not accept static inline functions if one argument is templated type
// For this reason all necessary function variants are explicityly written for it.
template<class T>
static void storeItemProps(QAbstractFormBuilder *abstractFormBuilder, const T *item,
        QList<DomProperty*> *properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = formBuilder->saveText(it.second, item->data(it.first.second))))
            properties->append(p);

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((v = item->data(it.first)).isValid() &&
            (p = variantToDomProperty(abstractFormBuilder,
                static_cast<const QMetaObject *>(&QAbstractFormBuilderGadget::staticMetaObject),
                it.second, v)))
            properties->append(p);

    if ((p = formBuilder->saveResource(item->data(Qt::DecorationPropertyRole))))
        properties->append(p);
}

template<class T>
static void storeItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, const T *item,
        QList<DomProperty*> *properties)
{
    storeItemProps<T>(abstractFormBuilder, item, properties);
    storeItemFlags<T>(item, properties);
}

template<class T>
static void loadItemProps(QAbstractFormBuilder *abstractFormBuilder, T *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = properties.value(it.second))) {
            v = formBuilder->textBuilder()->loadText(p);
            QVariant nativeValue = formBuilder->textBuilder()->toNativeValue(v);
            item->setData(it.first.first, qVariantValue<QString>(nativeValue));
            item->setData(it.first.second, v);
        }

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((p = properties.value(it.second)) &&
            (v = formBuilder->toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p)).isValid())
            item->setData(it.first, v);

    if ((p = properties.value(strings.iconAttribute))) {
        v = formBuilder->resourceBuilder()->loadResource(formBuilder->workingDirectory(), p);
        QVariant nativeValue = formBuilder->resourceBuilder()->toNativeValue(v);
        item->setIcon(qVariantValue<QIcon>(nativeValue));
        item->setData(Qt::DecorationPropertyRole, v);
    }
}

template<class T>
static void loadItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, T *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    static const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");

    loadItemProps<T>(abstractFormBuilder, item, properties);

    DomProperty *p;
    if ((p = properties.value(strings.flagsAttribute)) && p->kind() == DomProperty::Set)
        item->setFlags(enumKeysToValue<Qt::ItemFlags>(itemFlags_enum, p->elementSet().toAscii()));
}

#else

static void storeItemProps(QAbstractFormBuilder *abstractFormBuilder, const QTableWidgetItem *item,
        QList<DomProperty*> *properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = formBuilder->saveText(it.second, item->data(it.first.second))))
            properties->append(p);

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((v = item->data(it.first)).isValid() &&
            (p = variantToDomProperty(abstractFormBuilder,
                static_cast<const QMetaObject *>(&QAbstractFormBuilderGadget::staticMetaObject),
                it.second, v)))
            properties->append(p);

    if ((p = formBuilder->saveResource(item->data(Qt::DecorationPropertyRole))))
        properties->append(p);
}

static void storeItemProps(QAbstractFormBuilder *abstractFormBuilder, const QListWidgetItem *item,
        QList<DomProperty*> *properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = formBuilder->saveText(it.second, item->data(it.first.second))))
            properties->append(p);

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((v = item->data(it.first)).isValid() &&
            (p = variantToDomProperty(abstractFormBuilder,
                static_cast<const QMetaObject *>(&QAbstractFormBuilderGadget::staticMetaObject),
                it.second, v)))
            properties->append(p);

    if ((p = formBuilder->saveResource(item->data(Qt::DecorationPropertyRole))))
        properties->append(p);
}

static void storeItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, const QTableWidgetItem *item,
        QList<DomProperty*> *properties)
{
    storeItemProps(abstractFormBuilder, item, properties);
    storeItemFlags(item, properties);
}

static void storeItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, const QListWidgetItem *item,
        QList<DomProperty*> *properties)
{
    storeItemProps(abstractFormBuilder, item, properties);
    storeItemFlags(item, properties);
}

static void loadItemProps(QAbstractFormBuilder *abstractFormBuilder, QTableWidgetItem *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = properties.value(it.second))) {
            v = formBuilder->textBuilder()->loadText(p);
            QVariant nativeValue = formBuilder->textBuilder()->toNativeValue(v);
            item->setData(it.first.first, qVariantValue<QString>(nativeValue));
            item->setData(it.first.second, v);
        }

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((p = properties.value(it.second)) &&
            (v = formBuilder->toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p)).isValid())
            item->setData(it.first, v);

    if ((p = properties.value(strings.iconAttribute))) {
        v = formBuilder->resourceBuilder()->loadResource(formBuilder->workingDirectory(), p);
        QVariant nativeValue = formBuilder->resourceBuilder()->toNativeValue(v);
        item->setIcon(qVariantValue<QIcon>(nativeValue));
        item->setData(Qt::DecorationPropertyRole, v);
    }
}

static void loadItemProps(QAbstractFormBuilder *abstractFormBuilder, QListWidgetItem *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    FriendlyFB * const formBuilder = static_cast<FriendlyFB *>(abstractFormBuilder);

    DomProperty *p;
    QVariant v;

    foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
        if ((p = properties.value(it.second))) {
            v = formBuilder->textBuilder()->loadText(p);
            QVariant nativeValue = formBuilder->textBuilder()->toNativeValue(v);
            item->setData(it.first.first, qVariantValue<QString>(nativeValue));
            item->setData(it.first.second, v);
        }

    foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
        if ((p = properties.value(it.second)) &&
            (v = formBuilder->toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p)).isValid())
            item->setData(it.first, v);

    if ((p = properties.value(strings.iconAttribute))) {
        v = formBuilder->resourceBuilder()->loadResource(formBuilder->workingDirectory(), p);
        QVariant nativeValue = formBuilder->resourceBuilder()->toNativeValue(v);
        item->setIcon(qVariantValue<QIcon>(nativeValue));
        item->setData(Qt::DecorationPropertyRole, v);
    }
}

static void loadItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, QTableWidgetItem *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    static const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");

    loadItemProps(abstractFormBuilder, item, properties);

    DomProperty *p;
    if ((p = properties.value(strings.flagsAttribute)) && p->kind() == DomProperty::Set)
        item->setFlags(enumKeysToValue<Qt::ItemFlags>(itemFlags_enum, p->elementSet().toAscii()));
}

static void loadItemPropsNFlags(QAbstractFormBuilder *abstractFormBuilder, QListWidgetItem *item,
        const QHash<QString, DomProperty*> &properties)
{
    static const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    static const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");

    loadItemProps(abstractFormBuilder, item, properties);

    DomProperty *p;
    if ((p = properties.value(strings.flagsAttribute)) && p->kind() == DomProperty::Set)
        item->setFlags(enumKeysToValue<Qt::ItemFlags>(itemFlags_enum, p->elementSet().toAscii()));
}

#endif

/*!
    \internal
*/
void QAbstractFormBuilder::saveTreeWidgetExtraInfo(QTreeWidget *treeWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);

    QList<DomColumn*> columns;
    DomProperty *p;
    QVariant v;
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    // save the header
    for (int c = 0; c<treeWidget->columnCount(); ++c) {
        DomColumn *column = new DomColumn;

        QList<DomProperty*> properties;

        foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles) {
            p = saveText(it.second, treeWidget->headerItem()->data(c, it.first.second));
            // Prevent uic 4.4.X from crashing if it cannot find a column text
            if (!p && it.first.first == Qt::EditRole && it.second == QLatin1String("text")) {
                DomString *defaultHeader = new DomString;
                defaultHeader->setText(QString::number(c + 1));
                defaultHeader->setAttributeNotr(QLatin1String("true"));
                p = new DomProperty;
                p->setAttributeName(it.second);
                p->setElementString(defaultHeader);
            }
            if (p)
                properties.append(p);
        }

        foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
            if ((v = treeWidget->headerItem()->data(c, it.first)).isValid() &&
                (p = variantToDomProperty(this, &QAbstractFormBuilderGadget::staticMetaObject, it.second, v)))
                properties.append(p);

        if ((p = saveResource(treeWidget->headerItem()->data(c, Qt::DecorationPropertyRole))))
            properties.append(p);

        column->setElementProperty(properties);
        columns.append(column);
    }

    ui_widget->setElementColumn(columns);

    QList<DomItem *> items = ui_widget->elementItem();

    QQueue<QPair<QTreeWidgetItem *, DomItem *> > pendingQueue;
    for (int i = 0; i < treeWidget->topLevelItemCount(); i++)
        pendingQueue.enqueue(qMakePair(treeWidget->topLevelItem(i), (DomItem *)0));

    while (!pendingQueue.isEmpty()) {
        const QPair<QTreeWidgetItem *, DomItem *> pair = pendingQueue.dequeue();
        QTreeWidgetItem *item = pair.first;
        DomItem *parentDomItem = pair.second;

        DomItem *currentDomItem = new DomItem;

        QList<DomProperty*> properties;
        for (int c = 0; c < treeWidget->columnCount(); c++) {
            foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
                if ((p = saveText(it.second, item->data(c, it.first.second))))
                    properties.append(p);

            foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
                if ((v = item->data(c, it.first)).isValid() &&
                    (p = variantToDomProperty(this, &QAbstractFormBuilderGadget::staticMetaObject, it.second, v)))
                    properties.append(p);

            if ((p = saveResource(item->data(c, Qt::DecorationPropertyRole))))
                properties.append(p);
        }
        storeItemFlags(item, &properties);
        currentDomItem->setElementProperty(properties);

        if (parentDomItem) {
            QList<DomItem *> childrenItems = parentDomItem->elementItem();
            childrenItems.append(currentDomItem);
            parentDomItem->setElementItem(childrenItems);
        } else
            items.append(currentDomItem);

        for (int i = 0; i < item->childCount(); i++)
            pendingQueue.enqueue(qMakePair(item->child(i), currentDomItem));
    }

    ui_widget->setElementItem(items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveTableWidgetExtraInfo(QTableWidget *tableWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);

    // save the horizontal header
    QList<DomColumn*> columns;
    for (int c = 0; c < tableWidget->columnCount(); c++) {
        QList<DomProperty*> properties;
        QTableWidgetItem *item = tableWidget->horizontalHeaderItem(c);
        if (item)
            storeItemProps(this, item, &properties);

        DomColumn *column = new DomColumn;
        column->setElementProperty(properties);
        columns.append(column);
    }
    ui_widget->setElementColumn(columns);

    // save the vertical header
    QList<DomRow*> rows;
    for (int r = 0; r < tableWidget->rowCount(); r++) {
        QList<DomProperty*> properties;
        QTableWidgetItem *item = tableWidget->verticalHeaderItem(r);
        if (item)
            storeItemProps(this, item, &properties);

        DomRow *row = new DomRow;
        row->setElementProperty(properties);
        rows.append(row);
    }
    ui_widget->setElementRow(rows);

    QList<DomItem *> items = ui_widget->elementItem();
    for (int r = 0; r < tableWidget->rowCount(); r++)
        for (int c = 0; c < tableWidget->columnCount(); c++) {
            QTableWidgetItem *item = tableWidget->item(r, c);
            if (item) {
                QList<DomProperty*> properties;
                storeItemPropsNFlags(this, item, &properties);

                DomItem *domItem = new DomItem;
                domItem->setAttributeRow(r);
                domItem->setAttributeColumn(c);
                domItem->setElementProperty(properties);
                items.append(domItem);
            }
        }

    ui_widget->setElementItem(items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveListWidgetExtraInfo(QListWidget *listWidget, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);

    QList<DomItem*> ui_items = ui_widget->elementItem();
    for (int i=0; i<listWidget->count(); ++i) {
        QList<DomProperty*> properties;
        storeItemPropsNFlags(this, listWidget->item(i), &properties);

        DomItem *ui_item = new DomItem();
        ui_item->setElementProperty(properties);
        ui_items.append(ui_item);
    }

    ui_widget->setElementItem(ui_items);
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveComboBoxExtraInfo(QComboBox *comboBox, DomWidget *ui_widget, DomWidget *ui_parentWidget)
{
    Q_UNUSED(ui_parentWidget);
    QList<DomItem*> ui_items = ui_widget->elementItem();

    const int count = comboBox->count();
    for (int i=0; i < count; ++i) {
        // We might encounter items for which both builders return 0 in Designer
        // (indicating a custom combo adding items in the constructor). Ignore those.
        DomProperty *textProperty = saveText(QFormBuilderStrings::instance().textAttribute,
                                             comboBox->itemData(i, Qt::DisplayPropertyRole));
        DomProperty *iconProperty = saveResource(comboBox->itemData(i, Qt::DecorationPropertyRole));
        if (textProperty || iconProperty) {
            QList<DomProperty*> properties;
            if (textProperty)
                properties.push_back(textProperty);
            if (iconProperty)
                properties.push_back(iconProperty);

            DomItem *ui_item = new DomItem();
            ui_item->setElementProperty(properties);
            ui_items.push_back(ui_item);
        }
    }

    ui_widget->setElementItem(ui_items);
}

// Return the buttongroups assigned to a button except the internal one
// (with empty object name) used by Q3ButtonGroup.
static inline const QButtonGroup *formButtonGroup(const QAbstractButton *widget)
{
    const QButtonGroup *buttonGroup = widget->group();
    if (!buttonGroup)
        return 0;
    if (buttonGroup->objectName().isEmpty()) {
        if (const QWidget *parent = widget->parentWidget())
            if (!qstrcmp(parent->metaObject()->className(), "Q3ButtonGroup"))
                return 0;
    }
    return buttonGroup;
}

/*!
    \internal
    \since 4.5
*/

void QAbstractFormBuilder::saveButtonExtraInfo(const QAbstractButton *widget, DomWidget *ui_widget, DomWidget *)
{
    typedef QList<DomProperty*> DomPropertyList;
    if (const QButtonGroup *buttonGroup = formButtonGroup(widget)) {
        DomPropertyList attributes = ui_widget->elementAttribute();
        DomString *domString = new DomString();
        domString->setText(buttonGroup->objectName());
        DomProperty *domProperty = new DomProperty();
        domProperty->setAttributeName(QLatin1String(buttonGroupPropertyC));
        domProperty->setElementString(domString);
        attributes += domProperty;
        ui_widget->setElementAttribute(attributes);
    }
}

/*!
    \internal
    \since 4.5
*/
void QAbstractFormBuilder::saveItemViewExtraInfo(const QAbstractItemView *itemView,
                                                 DomWidget *ui_widget, DomWidget *)
{
    //
    // Special handling for qtableview/qtreeview fake header attributes
    //
    static QStringList realPropertyNames =
            (QStringList() << QLatin1String("visible")
                           << QLatin1String("cascadingSectionResizes")
                           << QLatin1String("defaultSectionSize")
                           << QLatin1String("highlightSections")
                           << QLatin1String("minimumSectionSize")
                           << QLatin1String("showSortIndicator")
                           << QLatin1String("stretchLastSection"));

    if (const QTreeView *treeView = qobject_cast<const QTreeView*>(itemView)) {
        QList<DomProperty*> viewProperties = ui_widget->elementAttribute();
        QList<DomProperty*> headerProperties = computeProperties(treeView->header());
        foreach (const QString &realPropertyName, realPropertyNames) {
            const QString upperPropertyName = realPropertyName.at(0).toUpper()
                                              + realPropertyName.mid(1);
            const QString fakePropertyName = QLatin1String("header") + upperPropertyName;
            foreach (DomProperty *property, headerProperties) {
                if (property->attributeName() == realPropertyName) {
                    property->setAttributeName(fakePropertyName);
                    viewProperties << property;
                }
            }
        }
        ui_widget->setElementAttribute(viewProperties);
    } else if (const QTableView *tableView = qobject_cast<const QTableView*>(itemView)) {
        static QStringList headerPrefixes =
                (QStringList() << QLatin1String("horizontalHeader")
                               << QLatin1String("verticalHeader"));

        QList<DomProperty*> viewProperties = ui_widget->elementAttribute();
        foreach (const QString &headerPrefix, headerPrefixes) {
            QList<DomProperty*> headerProperties;
            if (headerPrefix == QLatin1String("horizontalHeader"))
                headerProperties = computeProperties(tableView->horizontalHeader());
            else
                headerProperties = computeProperties(tableView->verticalHeader());
            foreach (const QString &realPropertyName, realPropertyNames) {
                const QString upperPropertyName = realPropertyName.at(0).toUpper()
                                                  + realPropertyName.mid(1);
                const QString fakePropertyName = headerPrefix + upperPropertyName;
                foreach (DomProperty *property, headerProperties) {
                    if (property->attributeName() == realPropertyName) {
                        property->setAttributeName(fakePropertyName);
                        viewProperties << property;
                    }
                }
            }
        }
        ui_widget->setElementAttribute(viewProperties);
    }
}

/*!
    \internal
    \since 4.4
*/

void QAbstractFormBuilder::setResourceBuilder(QResourceBuilder *builder)
{
    QFormBuilderExtra::instance(this)->setResourceBuilder(builder);
}

/*!
    \internal
    \since 4.4
*/

QResourceBuilder *QAbstractFormBuilder::resourceBuilder() const
{
    return QFormBuilderExtra::instance(this)->resourceBuilder();
}

/*!
    \internal
    \since 4.5
*/

void QAbstractFormBuilder::setTextBuilder(QTextBuilder *builder)
{
    QFormBuilderExtra::instance(this)->setTextBuilder(builder);
}

/*!
    \internal
    \since 4.5
*/

QTextBuilder *QAbstractFormBuilder::textBuilder() const
{
    return QFormBuilderExtra::instance(this)->textBuilder();
}

/*!
    \internal
*/
void QAbstractFormBuilder::saveExtraInfo(QWidget *widget, DomWidget *ui_widget,
                                         DomWidget *ui_parentWidget)
{
    if (QListWidget *listWidget = qobject_cast<QListWidget*>(widget)) {
        saveListWidgetExtraInfo(listWidget, ui_widget, ui_parentWidget);
    } else if (QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(widget)) {
        saveTreeWidgetExtraInfo(treeWidget, ui_widget, ui_parentWidget);
    } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget*>(widget)) {
        saveTableWidgetExtraInfo(tableWidget, ui_widget, ui_parentWidget);
    } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(widget)) {
        if (!qobject_cast<QFontComboBox*>(widget))
            saveComboBoxExtraInfo(comboBox, ui_widget, ui_parentWidget);
    } else if(QAbstractButton *ab = qobject_cast<QAbstractButton *>(widget)) {
        saveButtonExtraInfo(ab, ui_widget, ui_parentWidget);
    }
    if (QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(widget)) {
        saveItemViewExtraInfo(itemView, ui_widget, ui_parentWidget);
    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadListWidgetExtraInfo(DomWidget *ui_widget, QListWidget *listWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");

    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        const DomPropertyHash properties = propertyMap(ui_item->elementProperty());
        QListWidgetItem *item = new QListWidgetItem(listWidget);
#ifndef Q_CC_RVCT
        loadItemPropsNFlags<QListWidgetItem>(this, item, properties);
#else
        loadItemPropsNFlags(this, item, properties);
#endif        
    }

    DomProperty *currentRow = propertyMap(ui_widget->elementProperty()).value(strings.currentRowProperty);
    if (currentRow)
        listWidget->setCurrentRow(currentRow->elementNumber());
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadTreeWidgetExtraInfo(DomWidget *ui_widget, QTreeWidget *treeWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    const QMetaEnum itemFlags_enum = metaEnum<QAbstractFormBuilderGadget>("itemFlags");
    const QList<DomColumn*> columns = ui_widget->elementColumn();
    if (columns.count() > 0)
        treeWidget->setColumnCount(columns.count());

    for (int i = 0; i<columns.count(); ++i) {
        const DomColumn *c = columns.at(i);
        const DomPropertyHash properties = propertyMap(c->elementProperty());

        DomProperty *p;
        QVariant v;

        foreach (const QFormBuilderStrings::RoleNName &it, strings.itemRoles)
            if ((p = properties.value(it.second)) &&
                (v = toVariant(&QAbstractFormBuilderGadget::staticMetaObject, p)).isValid())
                treeWidget->headerItem()->setData(i, it.first, v);

        foreach (const QFormBuilderStrings::TextRoleNName &it, strings.itemTextRoles)
            if ((p = properties.value(it.second))) {
                v = textBuilder()->loadText(p);
                QVariant nativeValue = textBuilder()->toNativeValue(v);
                treeWidget->headerItem()->setData(i, it.first.first, qVariantValue<QString>(nativeValue));
                treeWidget->headerItem()->setData(i, it.first.second, v);
            }

        if ((p = properties.value(strings.iconAttribute))) {
            v = resourceBuilder()->loadResource(workingDirectory(), p);
            QVariant nativeValue = resourceBuilder()->toNativeValue(v);
            treeWidget->headerItem()->setIcon(i, qVariantValue<QIcon>(nativeValue));
            treeWidget->headerItem()->setData(i, Qt::DecorationPropertyRole, v);
        }
    }

    QQueue<QPair<DomItem *, QTreeWidgetItem *> > pendingQueue;
    foreach (DomItem *ui_item, ui_widget->elementItem())
        pendingQueue.enqueue(qMakePair(ui_item, (QTreeWidgetItem *)0));

    while (!pendingQueue.isEmpty()) {
        const QPair<DomItem *, QTreeWidgetItem *> pair = pendingQueue.dequeue();
        const DomItem *domItem = pair.first;
        QTreeWidgetItem *parentItem = pair.second;

        QTreeWidgetItem *currentItem = 0;

        if (parentItem)
            currentItem = new QTreeWidgetItem(parentItem);
        else
            currentItem = new QTreeWidgetItem(treeWidget);

        const QList<DomProperty *> properties = domItem->elementProperty();
        int col = -1;
        foreach (DomProperty *property, properties) {
            if (property->attributeName() == strings.flagsAttribute && !property->elementSet().isEmpty()) {
                currentItem->setFlags(enumKeysToValue<Qt::ItemFlags>(itemFlags_enum, property->elementSet().toAscii()));
            } else if (property->attributeName() == strings.textAttribute && property->elementString()) {
                col++;
                QVariant textV = textBuilder()->loadText(property);
                QVariant nativeValue = textBuilder()->toNativeValue(textV);
                currentItem->setText(col, qVariantValue<QString>(nativeValue));
                currentItem->setData(col, Qt::DisplayPropertyRole, textV);
            } else if (col >= 0) {
                if (property->attributeName() == strings.iconAttribute) {
                    QVariant v = resourceBuilder()->loadResource(workingDirectory(), property);
                    if (v.isValid()) {
                        QVariant nativeValue = resourceBuilder()->toNativeValue(v);
                        currentItem->setIcon(col, qVariantValue<QIcon>(nativeValue));
                        currentItem->setData(col, Qt::DecorationPropertyRole, v);
                    }
                } else {
                    QVariant v;
                    int role = strings.treeItemRoleHash.value(property->attributeName(), (Qt::ItemDataRole)-1);
                    if (role >= 0) {
                        if ((v = toVariant(&QAbstractFormBuilderGadget::staticMetaObject, property)).isValid())
                            currentItem->setData(col, role, v);
                    } else {
                        QPair<Qt::ItemDataRole, Qt::ItemDataRole> rolePair =
                            strings.treeItemTextRoleHash.value(property->attributeName(),
                                         qMakePair((Qt::ItemDataRole)-1, (Qt::ItemDataRole)-1));
                        if (rolePair.first >= 0) {
                            QVariant textV = textBuilder()->loadText(property);
                            QVariant nativeValue = textBuilder()->toNativeValue(textV);
                            currentItem->setData(col, rolePair.first, qVariantValue<QString>(nativeValue));
                            currentItem->setData(col, rolePair.second, textV);
                        }
                    }
                }
            }
        }

        foreach (DomItem *childItem, domItem->elementItem())
            pendingQueue.enqueue(qMakePair(childItem, currentItem));

    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadTableWidgetExtraInfo(DomWidget *ui_widget, QTableWidget *tableWidget, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);

    const QList<DomColumn*> columns = ui_widget->elementColumn();
    if (columns.count() > 0)
        tableWidget->setColumnCount(columns.count());
    for (int i = 0; i< columns.count(); i++) {
        DomColumn *c = columns.at(i);
        const DomPropertyHash properties = propertyMap(c->elementProperty());

        if (!properties.isEmpty()) {
            QTableWidgetItem *item = new QTableWidgetItem;
            loadItemProps(this, item, properties);
            tableWidget->setHorizontalHeaderItem(i, item);
        }
    }

    const QList<DomRow*> rows = ui_widget->elementRow();
    if (rows.count() > 0)
        tableWidget->setRowCount(rows.count());
    for (int i = 0; i< rows.count(); i++) {
        const DomRow *r = rows.at(i);
        const DomPropertyHash properties = propertyMap(r->elementProperty());

        if (!properties.isEmpty()) {
            QTableWidgetItem *item = new QTableWidgetItem;
            loadItemProps(this, item, properties);
            tableWidget->setVerticalHeaderItem(i, item);
        }
    }

    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        if (ui_item->hasAttributeRow() && ui_item->hasAttributeColumn()) {
            const DomPropertyHash properties = propertyMap(ui_item->elementProperty());
            QTableWidgetItem *item = new QTableWidgetItem;
            loadItemPropsNFlags(this, item, properties);
            tableWidget->setItem(ui_item->attributeRow(), ui_item->attributeColumn(), item);
        }
    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadComboBoxExtraInfo(DomWidget *ui_widget, QComboBox *comboBox, QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    foreach (DomItem *ui_item, ui_widget->elementItem()) {
        const DomPropertyHash properties = propertyMap(ui_item->elementProperty());
        QString text;
        QIcon icon;
        QVariant textData;
        QVariant iconData;

        DomProperty *p = 0;

        p = properties.value(strings.textAttribute);
        if (p && p->elementString()) {
             textData = textBuilder()->loadText(p);
             text = qVariantValue<QString>(textBuilder()->toNativeValue(textData));
        }

        p = properties.value(strings.iconAttribute);
        if (p) {
             iconData = resourceBuilder()->loadResource(workingDirectory(), p);
             icon = qVariantValue<QIcon>(resourceBuilder()->toNativeValue(iconData));
        }

        comboBox->addItem(icon, text);
        comboBox->setItemData((comboBox->count()-1), iconData, Qt::DecorationPropertyRole);
        comboBox->setItemData((comboBox->count()-1), textData, Qt::DisplayPropertyRole);
    }

    DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
    if (currentIndex)
        comboBox->setCurrentIndex(currentIndex->elementNumber());
}

// Get the button group name out of a widget's attribute list
static QString buttonGroupName(const DomWidget *ui_widget)
{
    typedef QList<DomProperty*> DomPropertyList;
    const DomPropertyList attributes = ui_widget->elementAttribute();
    if (attributes.empty())
        return QString();
    const QString buttonGroupProperty = QLatin1String(buttonGroupPropertyC);
    const DomPropertyList::const_iterator cend = attributes.constEnd();
    for (DomPropertyList::const_iterator it = attributes.constBegin(); it != cend; ++it)
        if ( (*it)->attributeName() == buttonGroupProperty)
            return (*it)->elementString()->text();
    return QString();
}

/*!
    \internal
    \since 4.5
*/

void QAbstractFormBuilder::loadButtonExtraInfo(const DomWidget *ui_widget, QAbstractButton *button, QWidget *)
{
    typedef QFormBuilderExtra::ButtonGroupEntry ButtonGroupEntry;
    typedef QFormBuilderExtra::ButtonGroupHash ButtonGroupHash;

    const QString groupName = buttonGroupName(ui_widget);
    if (groupName.isEmpty())
        return;
    // Find entry
    QFormBuilderExtra *extra = QFormBuilderExtra::instance(this);
    ButtonGroupHash &buttonGroups = extra->buttonGroups();
    ButtonGroupHash::iterator it = buttonGroups.find(groupName);
    if (it == buttonGroups.end()) {
#ifdef QFORMINTERNAL_NAMESPACE // Suppress the warning when copying in Designer
        uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "Invalid QButtonGroup reference '%1' referenced by '%2'.").arg(groupName, button->objectName()));
#endif
        return;
    }
    // Create button group on demand?
    QButtonGroup *&group = it.value().second;
    if (group == 0) {
        group = new QButtonGroup;
        group->setObjectName(groupName);
        applyProperties(group,  it.value().first->elementProperty());
    }
    group->addButton(button);
}

/*!
    \internal
    \since 4.5
*/
void QAbstractFormBuilder::loadItemViewExtraInfo(DomWidget *ui_widget, QAbstractItemView *itemView,
                                                 QWidget *)
{
    //
    // Special handling for qtableview/qtreeview fake header attributes
    //
    static QStringList realPropertyNames =
            (QStringList() << QLatin1String("visible")
                           << QLatin1String("cascadingSectionResizes")
                           << QLatin1String("defaultSectionSize")
                           << QLatin1String("highlightSections")
                           << QLatin1String("minimumSectionSize")
                           << QLatin1String("showSortIndicator")
                           << QLatin1String("stretchLastSection"));

    if (QTreeView *treeView = qobject_cast<QTreeView*>(itemView)) {
        QList<DomProperty*> allAttributes = ui_widget->elementAttribute();
        QList<DomProperty*> headerProperties;
        foreach (const QString &realPropertyName, realPropertyNames) {
            const QString upperPropertyName = realPropertyName.at(0).toUpper()
                                              + realPropertyName.mid(1);
            const QString fakePropertyName = QLatin1String("header") + upperPropertyName;
            foreach (DomProperty *attr, allAttributes) {
                if (attr->attributeName() == fakePropertyName) {
                    attr->setAttributeName(realPropertyName);
                    headerProperties << attr;
                }
            }
        }
        applyProperties(treeView->header(), headerProperties);
    } else if (QTableView *tableView = qobject_cast<QTableView*>(itemView)) {
        static QStringList headerPrefixes =
                (QStringList() << QLatin1String("horizontalHeader")
                               << QLatin1String("verticalHeader"));

        QList<DomProperty*> allAttributes = ui_widget->elementAttribute();
        foreach (const QString &headerPrefix, headerPrefixes) {
            QList<DomProperty*> headerProperties;
            foreach (const QString &realPropertyName, realPropertyNames) {
                const QString upperPropertyName = realPropertyName.at(0).toUpper()
                                                  + realPropertyName.mid(1);
                const QString fakePropertyName = headerPrefix + upperPropertyName;
                foreach (DomProperty *attr, allAttributes) {
                    if (attr->attributeName() == fakePropertyName) {
                        attr->setAttributeName(realPropertyName);
                        headerProperties << attr;
                    }
                }
            }
            if (headerPrefix == QLatin1String("horizontalHeader"))
                applyProperties(tableView->horizontalHeader(), headerProperties);
            else
                applyProperties(tableView->verticalHeader(), headerProperties);
        }
    }
}

/*!
    \internal
*/
void QAbstractFormBuilder::loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget)
{
    const QFormBuilderStrings &strings = QFormBuilderStrings::instance();
    if (0) {
#ifndef QT_NO_LISTWIDGET
    } else if (QListWidget *listWidget = qobject_cast<QListWidget*>(widget)) {
        loadListWidgetExtraInfo(ui_widget, listWidget, parentWidget);
#endif
#ifndef QT_NO_TREEWIDGET
    } else if (QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(widget)) {
        loadTreeWidgetExtraInfo(ui_widget, treeWidget, parentWidget);
#endif
#ifndef QT_NO_TABLEWIDGET
    } else if (QTableWidget *tableWidget = qobject_cast<QTableWidget*>(widget)) {
        loadTableWidgetExtraInfo(ui_widget, tableWidget, parentWidget);
#endif
#ifndef QT_NO_COMBOBOX
    } else if (QComboBox *comboBox = qobject_cast<QComboBox*>(widget)) {
        if (!qobject_cast<QFontComboBox *>(widget))
            loadComboBoxExtraInfo(ui_widget, comboBox, parentWidget);
#endif
#ifndef QT_NO_TABWIDGET
    } else if (QTabWidget *tabWidget = qobject_cast<QTabWidget*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            tabWidget->setCurrentIndex(currentIndex->elementNumber());
#endif
#ifndef QT_NO_STACKEDWIDGET
    } else if (QStackedWidget *stackedWidget = qobject_cast<QStackedWidget*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            stackedWidget->setCurrentIndex(currentIndex->elementNumber());
#endif
#ifndef QT_NO_TOOLBOX
    } else if (QToolBox *toolBox = qobject_cast<QToolBox*>(widget)) {
        const DomProperty *currentIndex = propertyMap(ui_widget->elementProperty()).value(strings.currentIndexProperty);
        if (currentIndex)
            toolBox->setCurrentIndex(currentIndex->elementNumber());
        const DomProperty *tabSpacing = propertyMap(ui_widget->elementProperty()).value(strings.tabSpacingProperty);
        if (tabSpacing)
            toolBox->layout()->setSpacing(tabSpacing->elementNumber());
#endif
    } else if (QAbstractButton *ab = qobject_cast<QAbstractButton *>(widget)) {
        loadButtonExtraInfo(ui_widget, ab, parentWidget);
    }
    if (QAbstractItemView *itemView = qobject_cast<QAbstractItemView *>(widget)) {
        loadItemViewExtraInfo(ui_widget, itemView, parentWidget);
    }
}

/*!
    \internal
*/
QIcon QAbstractFormBuilder::nameToIcon(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QAbstractFormBuilder::nameToIcon() is obsoleted";
    return QIcon();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::iconToFilePath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::iconToFilePath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::iconToQrcPath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::iconToQrcPath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QPixmap QAbstractFormBuilder::nameToPixmap(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QAbstractFormBuilder::nameToPixmap() is obsoleted";
    return QPixmap();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::pixmapToFilePath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::pixmapToFilePath() is obsoleted";
    return QString();
}

/*!
    \internal
*/
QString QAbstractFormBuilder::pixmapToQrcPath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QAbstractFormBuilder::pixmapToQrcPath() is obsoleted";
    return QString();
}

/*!
    Returns the current working directory of the form builder.

    \sa setWorkingDirectory() */
QDir QAbstractFormBuilder::workingDirectory() const
{
    return m_workingDirectory;
}

/*!
    Sets the current working directory of the form builder to the
    specified \a directory.

    \sa workingDirectory()*/
void QAbstractFormBuilder::setWorkingDirectory(const QDir &directory)
{
    m_workingDirectory = directory;
}

/*!
    \internal
*/
DomAction *QAbstractFormBuilder::createDom(QAction *action)
{
    if (action->parentWidget() == action->menu() || action->isSeparator())
        return 0;

    DomAction *ui_action = new DomAction;
    ui_action->setAttributeName(action->objectName());

    const QList<DomProperty*> properties = computeProperties(action);
    ui_action->setElementProperty(properties);

    return ui_action;
}

/*!
    \internal
    \since 4.5
*/

DomButtonGroup *QAbstractFormBuilder::createDom(QButtonGroup *buttonGroup)
{
    if (buttonGroup->buttons().count() == 0) // Empty group left over on form?
        return 0;
    DomButtonGroup *domButtonGroup = new DomButtonGroup;
    domButtonGroup->setAttributeName(buttonGroup->objectName());

    QList<DomProperty*> properties = computeProperties(buttonGroup);
    domButtonGroup->setElementProperty(properties);
    return domButtonGroup;
}

/*!
    \internal
*/
DomActionGroup *QAbstractFormBuilder::createDom(QActionGroup *actionGroup)
{
    DomActionGroup *ui_action_group = new DomActionGroup;
    ui_action_group->setAttributeName(actionGroup->objectName());

    QList<DomProperty*> properties = computeProperties(actionGroup);
    ui_action_group->setElementProperty(properties);

    QList<DomAction*> ui_actions;

    foreach (QAction *action, actionGroup->actions()) {
        if (DomAction *ui_action = createDom(action)) {
            ui_actions.append(ui_action);
        }
    }

    ui_action_group->setElementAction(ui_actions);

    return ui_action_group;
}

/*!
    \internal
*/
void QAbstractFormBuilder::addMenuAction(QAction *action)
{
    Q_UNUSED(action);
}

/*!
    \internal
*/
void QAbstractFormBuilder::reset()
{
    m_laidout.clear();
    m_actions.clear();
    m_actionGroups.clear();
    m_defaultMargin = INT_MIN;
    m_defaultSpacing = INT_MIN;
}

/*!
    \internal
    Access meta enumeration for Qt::ToolBarArea
*/

QMetaEnum QAbstractFormBuilder::toolBarAreaMetaEnum()
{
    return metaEnum<QAbstractFormBuilderGadget>("toolBarArea");
}

/*!
    \internal
    Return paths of an icon.
*/

QAbstractFormBuilder::IconPaths QAbstractFormBuilder::iconPaths(const QIcon &icon) const
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::iconPaths() is obsoleted";
    return IconPaths();
}

/*!
    \internal
    Return paths of a pixmap.
*/

QAbstractFormBuilder::IconPaths QAbstractFormBuilder::pixmapPaths(const QPixmap &pixmap) const
{
    Q_UNUSED(pixmap);
    qWarning() << "QAbstractFormBuilder::pixmapPaths() is obsoleted";
    return IconPaths();
}

/*!
    \internal
    Set up a DOM property with icon.
*/

void QAbstractFormBuilder::setIconProperty(DomProperty &p, const IconPaths &ip) const
{
    DomResourceIcon *dpi = new DomResourceIcon;

 /* TODO
    if (!ip.second.isEmpty())
        pix->setAttributeResource(ip.second);
*/
    dpi->setText(ip.first);

    p.setAttributeName(QFormBuilderStrings::instance().iconAttribute);
    p.setElementIconSet(dpi);
}

/*!
    \internal
    Set up a DOM property with pixmap.
*/

void QAbstractFormBuilder::setPixmapProperty(DomProperty &p, const IconPaths &ip) const
{
    DomResourcePixmap *pix = new DomResourcePixmap;
    if (!ip.second.isEmpty())
        pix->setAttributeResource(ip.second);

    pix->setText(ip.first);

    p.setAttributeName(QFormBuilderStrings::instance().pixmapAttribute);
    p.setElementPixmap(pix);
}

/*!
    \internal
    Convenience. Return DOM property for icon; 0 if icon.isNull().
*/

DomProperty* QAbstractFormBuilder::iconToDomProperty(const QIcon &icon) const
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::iconToDomProperty() is obsoleted";
    return 0;
}

/*!
    \internal
    \since 4.4
*/

DomProperty *QAbstractFormBuilder::saveResource(const QVariant &v) const
{
    if (v.isNull())
        return 0;

    DomProperty *p = resourceBuilder()->saveResource(workingDirectory(), v);
    if (p)
        p->setAttributeName(QFormBuilderStrings::instance().iconAttribute);
    return p;
}

/*!
    \internal
    \since 4.5
*/

DomProperty *QAbstractFormBuilder::saveText(const QString &attributeName, const QVariant &v) const
{
    if (v.isNull())
        return 0;

    DomProperty *p = textBuilder()->saveText(v);
    if (p)
        p->setAttributeName(attributeName);
    return p;
}

/*!
    \internal
    Return the appropriate DOM pixmap for an image dom property.
    From 4.4 - unused
*/

const DomResourcePixmap *QAbstractFormBuilder::domPixmap(const DomProperty* p) {
    switch (p->kind()) {
    case DomProperty::IconSet:
        qDebug() << "** WARNING QAbstractFormBuilder::domPixmap() called for icon set!";
        break;
    case DomProperty::Pixmap:
        return p->elementPixmap();
    default:
        break;
    }
    return 0;
}

/*!
    \internal
    Create icon from DOM.
    From 4.4 - unused
*/

QIcon QAbstractFormBuilder::domPropertyToIcon(const DomResourcePixmap *icon)
{
    Q_UNUSED(icon);
    qWarning() << "QAbstractFormBuilder::domPropertyToIcon() is obsoleted";
    return QIcon();
}

/*!
    \internal
    Create icon from DOM. Assert if !domPixmap
    From 4.4 - unused
*/

QIcon QAbstractFormBuilder::domPropertyToIcon(const DomProperty* p)
{
    Q_UNUSED(p);
    qWarning() << "QAbstractFormBuilder::domPropertyToIcon() is obsoleted";
    return QIcon();
}


/*!
    \internal
    Create pixmap from DOM.
    From 4.4 - unused
*/

QPixmap QAbstractFormBuilder::domPropertyToPixmap(const DomResourcePixmap* pixmap)
{
    Q_UNUSED(pixmap);
    qWarning() << "QAbstractFormBuilder::domPropertyToPixmap() is obsoleted";
    return QPixmap();
}


/*!
    \internal
    Create pixmap from DOM. Assert if !domPixmap
    From 4.4 - unused
*/

QPixmap QAbstractFormBuilder::domPropertyToPixmap(const DomProperty* p)
{
    Q_UNUSED(p);
    qWarning() << "QAbstractFormBuilder::domPropertyToPixmap() is obsoleted";
    return QPixmap();
}

/*!
    \fn void QAbstractFormBuilder::createConnections ( DomConnections *, QWidget * )
    \internal
*/

/*!
    \fn void QAbstractFormBuilder::createCustomWidgets ( DomCustomWidgets * )
    \internal
*/

/*!
    \fn void QAbstractFormBuilder::createResources ( DomResources * )
    \internal
*/

/*!
     \fn QFormScriptRunner *QAbstractFormBuilder::formScriptRunner() const
     \internal
     \since 4.3
*/
#ifndef QT_FORMBUILDER_NO_SCRIPT
QFormScriptRunner *QAbstractFormBuilder::formScriptRunner() const
{
    return &(QFormBuilderExtra::instance(this)->formScriptRunner());
}
#endif

/*!
    Sets whether the execution of scripts is enabled to \a enabled.
    \since 4.3
    \internal
*/

void QAbstractFormBuilder::setScriptingEnabled(bool enabled)
{
#ifdef QT_FORMBUILDER_NO_SCRIPT
    if (enabled)
        uiLibWarning(QCoreApplication::translate("QAbstractFormBuilder", "This version of the uitools library is linked without script support."));
#else
    QFormScriptRunner::Options options = formScriptRunner()->options();
    if (enabled)
        options &= ~QFormScriptRunner::DisableScripts;
    else
        options |= QFormScriptRunner::DisableScripts;
    formScriptRunner()->setOptions(options);
#endif
}

/*!
    Returns whether the execution of scripts is enabled.
    \sa setScriptingEnabled()
    \since 4.3
    \internal
*/

bool QAbstractFormBuilder::isScriptingEnabled() const
{
#ifdef QT_FORMBUILDER_NO_SCRIPT
    return false;
#else
    return !(formScriptRunner()->options() & QFormScriptRunner::DisableScripts);
#endif
}

QT_END_NAMESPACE
