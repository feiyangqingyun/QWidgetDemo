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

#include "morphmenu_p.h"
#include "formwindowbase_p.h"
#include "widgetfactory_p.h"
#include "qdesigner_formwindowcommand_p.h"
#include "qlayout_widget_p.h"
#include "layoutinfo_p.h"
#include "qdesigner_propertycommand_p.h"

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>

#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QLayout>
#include <QtGui/QUndoStack>

#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QTabWidget>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBox>
#include <QtGui/QAbstractItemView>
#include <QtGui/QAbstractButton>
#include <QtGui/QAbstractSpinBox>
#include <QtGui/QTextEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QLabel>

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QSignalMapper>
#include <QtCore/QDebug>

Q_DECLARE_METATYPE(QWidgetList)

QT_BEGIN_NAMESPACE

// Helpers for the dynamic properties that store Z/Widget order
static const char *widgetOrderPropertyC = "_q_widgetOrder";
static const char *zOrderPropertyC = "_q_zOrder";

/* Morphing in Designer:
 * It is possible to morph:
 * - Non-Containers into similar widgets by category
 * - Simple page containers into similar widgets or page-based containers with
 *   a single page (in theory also into a QLayoutWidget, but this might
 *   not always be appropriate).
 * - Page-based containers into page-based containers or simple containers if
 *   they have just one page
 * [Page based containers meaning here having a container extension]
 * Morphing types are restricted to the basic Qt types. Morphing custom
 * widgets is considered risky since they might have unmanaged layouts
 * or the like.
 *
 * Requirements:
 * - The widget must be on a non-laid out parent or in a layout managed
 *   by Designer
 * - Its child widgets must be non-laid out or in a layout managed
 *   by Designer
 * Note that child widgets can be
 * - On the widget itself in the case of simple containers
 * - On several pages in the case of page-based containers
 * This is what is called 'childContainers' in the code (the widget itself
 * or the list of container extension pages).
 *
 * The Morphing process encompasses:
 * - Create a target widget and apply properties as far as applicable
 *   If the target widget has a container extension, add a sufficient
 *   number of pages.
 * - Transferring the child widgets over to the new childContainers.
 *   In the case of a managed layout on a childContainer, this is simply
 *   set on the target childContainer, which is a new Qt 4.5
 *   functionality.
 * - Replace the widget itself in the parent layout
 */

namespace qdesigner_internal {

enum MorphCategory {
    MorphCategoryNone, MorphSimpleContainer, MorphPageContainer, MorphItemView,
    MorphButton, MorphSpinBox, MorphTextEdit
};

// Determine category of a widget
static MorphCategory category(const QWidget *w)
{
    // Simple containers: Exact match
    const QMetaObject *mo = w->metaObject();
    if (mo == &QWidget::staticMetaObject || mo == &QFrame::staticMetaObject || mo == &QGroupBox::staticMetaObject || mo == &QLayoutWidget::staticMetaObject)
        return MorphSimpleContainer;
    if (mo == &QTabWidget::staticMetaObject || mo == &QStackedWidget::staticMetaObject || mo == &QToolBox::staticMetaObject)
        return MorphPageContainer;
    if (qobject_cast<const QAbstractItemView*>(w))
        return MorphItemView;
    if (qobject_cast<const QAbstractButton *>(w))
        return MorphButton;
    if (qobject_cast<const QAbstractSpinBox *>(w))
        return MorphSpinBox;
    if (qobject_cast<const QPlainTextEdit *>(w) || qobject_cast<const QTextEdit*>(w))
        return MorphTextEdit;

    return MorphCategoryNone;
}

/* Return the similar classes of a category. This is currently restricted
 * to the known Qt classes with no precautions to parse the Widget Database
 * (which is too risky, custom classes might have container extensions
 * or non-managed layouts, etc.). */

static QStringList classesOfCategory(MorphCategory cat)
{
    typedef QMap<MorphCategory, QStringList> CandidateCache;
    static CandidateCache candidateCache;
    CandidateCache::iterator it = candidateCache.find(cat);
    if (it == candidateCache.end()) {
        it = candidateCache.insert(cat, QStringList());
        QStringList &l = it.value();
        switch (cat) {
        case MorphCategoryNone:
            break;
        case MorphSimpleContainer:
            // Do not  generally allow to morph into a layout.
            // This can be risky in case of container pages,etc.
            l << QLatin1String("QWidget") << QLatin1String("QFrame") <<  QLatin1String("QGroupBox");
            break;
        case MorphPageContainer:
            l << QLatin1String("QTabWidget") <<  QLatin1String("QStackedWidget") << QLatin1String("QToolBox");
            break;
        case MorphItemView:
            l << QLatin1String("QListView") << QLatin1String("QListWidget")
              << QLatin1String("QTreeView") << QLatin1String("QTreeWidget")
              << QLatin1String("QTableView") << QLatin1String("QTableWidget")
              << QLatin1String("QColumnView");
            break;
        case MorphButton:
            l << QLatin1String("QCheckBox") << QLatin1String("QRadioButton")
              << QLatin1String("QPushButton") << QLatin1String("QToolButton")
              << QLatin1String("QCommandLinkButton");
            break;
        case MorphSpinBox:
              l << QLatin1String("QDateTimeEdit") << QLatin1String("QDateEdit")
                << QLatin1String("QTimeEdit")
                << QLatin1String("QSpinBox") << QLatin1String("QDoubleSpinBox");
            break;
        case MorphTextEdit:
             l << QLatin1String("QTextEdit") << QLatin1String("QPlainTextEdit");
            break;
        }
    }
    return it.value();
}

// Return the widgets containing the children to be transferred to. This is the
// widget itself in most cases, except for QDesignerContainerExtension cases
static QWidgetList childContainers(const QDesignerFormEditorInterface *core, QWidget *w)
{
    if (const QDesignerContainerExtension *ce = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), w)) {
        QWidgetList children;
        if (const int count = ce->count()) {
            for (int i = 0; i < count; i++)
                children.push_back(ce->widget(i));
        }
        return children;
    }
    QWidgetList self;
    self.push_back(w);
    return self;
}

// Suggest a suitable objectname for the widget to be morphed into
// Replace the class name parts: 'xxFrame' -> 'xxGroupBox', 'frame' -> 'groupBox'
static QString suggestObjectName(const QString &oldClassName, const QString &newClassName, const QString &oldName)
{
    QString oldClassPart = oldClassName;
    QString newClassPart = newClassName;
    if (oldClassPart.startsWith(QLatin1Char('Q')))
        oldClassPart.remove(0, 1);
    if (newClassPart.startsWith(QLatin1Char('Q')))
        newClassPart.remove(0, 1);

    QString newName = oldName;
    newName.replace(oldClassPart, newClassPart);
    oldClassPart[0] = oldClassPart.at(0).toLower();
    newClassPart[0] = newClassPart.at(0).toLower();
    newName.replace(oldClassPart, newClassPart);
    return newName;
}

// Find the label whose buddy the widget is.
QLabel *buddyLabelOf(QDesignerFormWindowInterface *fw, QWidget *w)
{
    typedef QList<QLabel*> LabelList;
    const LabelList labelList = qFindChildren<QLabel*>(fw);
    if (labelList.empty())
        return 0;
    const LabelList::const_iterator cend = labelList.constEnd();
    for (LabelList::const_iterator it = labelList.constBegin(); it != cend; ++it )
        if ( (*it)->buddy() == w)
            return *it;
    return 0;
}

// Replace widgets in a widget-list type dynamic property of the parent
// used for Z-order, etc.
static void replaceWidgetListDynamicProperty(QWidget *parentWidget,
                                             QWidget *oldWidget, QWidget *newWidget,
                                             const char *name)
{
    QWidgetList list = qVariantValue<QWidgetList>(parentWidget->property(name));
    const int index = list.indexOf(oldWidget);
    if (index != -1) {
        list.replace(index, newWidget);
         parentWidget->setProperty(name, qVariantFromValue(list));
    }
}

/* Morph a widget into another class. Use the static addMorphMacro() to
 * add a respective command sequence to the undo stack as it emits signals 
 * which cause other commands to be added. */
class MorphWidgetCommand : public QDesignerFormWindowCommand
{
    Q_DISABLE_COPY(MorphWidgetCommand)
public:

    explicit MorphWidgetCommand(QDesignerFormWindowInterface *formWindow);
    ~MorphWidgetCommand();

    // Convenience to add a morph command sequence macro
    static bool addMorphMacro(QDesignerFormWindowInterface *formWindow, QWidget *w, const QString &newClass);

    bool init(QWidget *widget, const QString &newClass);

    QString newWidgetName() const { return m_afterWidget->objectName(); }

    virtual void redo();
    virtual void undo();

    static QStringList candidateClasses(QDesignerFormWindowInterface *fw, QWidget *w);

private:
    static bool canMorph(QDesignerFormWindowInterface *fw, QWidget *w, int *childContainerCount = 0, MorphCategory *cat = 0);
    void morph(QWidget *before, QWidget *after);

    QWidget *m_beforeWidget;
    QWidget *m_afterWidget;
};

bool MorphWidgetCommand::addMorphMacro(QDesignerFormWindowInterface *fw, QWidget *w, const QString &newClass)
{
    MorphWidgetCommand *morphCmd = new MorphWidgetCommand(fw);
    if (!morphCmd->init(w, newClass)) {
        qWarning("*** Unable to create a MorphWidgetCommand");
        delete morphCmd;
        return false;
    }
    QLabel *buddyLabel = buddyLabelOf(fw, w);
    // Need a macro since it adds further commands
    QUndoStack *us = fw->commandHistory();
    us->beginMacro(morphCmd->text());
    // Have the signal slot/buddy editors add their commands to delete widget
    if (FormWindowBase *fwb = qobject_cast<FormWindowBase*>(fw))
        fwb->emitWidgetRemoved(w);

    const QString newWidgetName = morphCmd->newWidgetName();
    us->push(morphCmd);

    // restore buddy using the QByteArray name.
    if (buddyLabel) {
        SetPropertyCommand *buddyCmd = new SetPropertyCommand(fw);
        buddyCmd->init(buddyLabel, QLatin1String("buddy"), QVariant(newWidgetName.toUtf8()));
        us->push(buddyCmd);
    }
    us->endMacro();
    return true;
}

MorphWidgetCommand::MorphWidgetCommand(QDesignerFormWindowInterface *formWindow)  :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_beforeWidget(0),
    m_afterWidget(0)
{
}

MorphWidgetCommand::~MorphWidgetCommand()
{
}

bool MorphWidgetCommand::init(QWidget *widget, const QString &newClassName)
{
    QDesignerFormWindowInterface *fw = formWindow();
    QDesignerFormEditorInterface *core = fw->core();

    if (!canMorph(fw, widget))
        return false;

    const QString oldClassName = WidgetFactory::classNameOf(core, widget);
    const QString oldName = widget->objectName();
    //: MorphWidgetCommand description
    setText(QApplication::translate("Command", "Morph %1/'%2' into %3").arg(oldClassName, oldName, newClassName));

    m_beforeWidget = widget;
    m_afterWidget = core->widgetFactory()->createWidget(newClassName, fw);
    if (!m_afterWidget)
        return false;

    // Set object name. Do not unique it (as to maintain it).
    m_afterWidget->setObjectName(suggestObjectName(oldClassName, newClassName, oldName));

    // If the target has a container extension, we add enough new pages to take
    // up the children of the before widget
    if (QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_afterWidget)) {
        if (const int pageCount = childContainers(core, m_beforeWidget).size()) {
            const QString qWidget = QLatin1String("QWidget");
            const QString containerName = m_afterWidget->objectName();
            for (int i = 0; i < pageCount; i++) {
                QString name = containerName;
                name += QLatin1String("Page");
                name += QString::number(i + 1);
                QWidget *page = core->widgetFactory()->createWidget(qWidget);
                page->setObjectName(name);
                fw->ensureUniqueObjectName(page);
                c->addWidget(page);
                core->metaDataBase()->add(page);
            }
        }
    }

    // Copy over applicable properties
    const QDesignerPropertySheetExtension *beforeSheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), widget);
    QDesignerPropertySheetExtension *afterSheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), m_afterWidget);
    const QString objectNameProperty = QLatin1String("objectName");
    const int count = beforeSheet->count();
    for (int i = 0; i < count; i++)
        if (beforeSheet->isVisible(i) && beforeSheet->isChanged(i)) {
            const QString name = beforeSheet->propertyName(i);
            if (name != objectNameProperty) {
                const int afterIndex = afterSheet->indexOf(name);
                if (afterIndex != -1 && afterSheet->isVisible(afterIndex) && afterSheet->propertyGroup(afterIndex) == beforeSheet->propertyGroup(i)) {
                    afterSheet->setProperty(i, beforeSheet->property(i));
                    afterSheet->setChanged(i, true);
                } else {
                    // Some mismatch. The rest won't match, either
                    break;
                }
            }
        }
    return true;
}

void MorphWidgetCommand::redo()
{
    morph(m_beforeWidget, m_afterWidget);
}

void  MorphWidgetCommand::undo()
{
    morph(m_afterWidget, m_beforeWidget);
}

void MorphWidgetCommand::morph(QWidget *before, QWidget *after)
{
    QDesignerFormWindowInterface *fw = formWindow();

    fw->unmanageWidget(before);

    const QRect oldGeom = before->geometry();
    QWidget *parent = before->parentWidget();
    Q_ASSERT(parent);
    /* Morphing consists of main 2 steps
     * 1) Move over children (laid out, non-laid out)
     * 2) Register self with new parent (laid out, non-laid out) */

    // 1) Move children. Loop over child containers
    QWidgetList beforeChildContainers = childContainers(fw->core(), before);
    QWidgetList afterChildContainers = childContainers(fw->core(), after);
    Q_ASSERT(beforeChildContainers.size() == afterChildContainers.size());
    const int childContainerCount = beforeChildContainers.size();
    for (int i = 0; i < childContainerCount; i++) {
        QWidget *beforeChildContainer = beforeChildContainers.at(i);
        QWidget *afterChildContainer = afterChildContainers.at(i);
        if (QLayout *childLayout = beforeChildContainer->layout()) {
            // Laid-out: Move the layout (since 4.5)
            afterChildContainer->setLayout(childLayout);
        } else {
            // Non-Laid-out: Reparent, move over
            const QObjectList c = beforeChildContainer->children();
            const QObjectList::const_iterator cend = c.constEnd();
            for (QObjectList::const_iterator it =  c.constBegin(); it != cend; ++it) {
                if ( (*it)->isWidgetType()) {
                    QWidget *w = static_cast<QWidget*>(*it);
                    if (fw->isManaged(w)) {
                        const QRect geom = w->geometry();
                        w->setParent(afterChildContainer);
                        w->setGeometry(geom);
                    }
                }
            }
        }
        afterChildContainer->setProperty(widgetOrderPropertyC, beforeChildContainer->property(widgetOrderPropertyC));
        afterChildContainer->setProperty(zOrderPropertyC, beforeChildContainer->property(zOrderPropertyC));
    }

    // 2) Replace the actual widget in the parent layout
    after->setGeometry(oldGeom);
    if (QLayout *containingLayout = LayoutInfo::managedLayout(fw->core(), parent)) {
        LayoutHelper *lh = LayoutHelper::createLayoutHelper(LayoutInfo::layoutType(fw->core(), containingLayout));
        Q_ASSERT(lh);
        lh->replaceWidget(containingLayout, before, after);
        delete lh;
    } else {
        before->hide();
        before->setParent(0);
        after->setParent(parent);
        after->setGeometry(oldGeom);
    }

    // Check various properties: Z order, form tab order
    replaceWidgetListDynamicProperty(parent, before, after, widgetOrderPropertyC);
    replaceWidgetListDynamicProperty(parent, before, after, zOrderPropertyC);

    QDesignerMetaDataBaseItemInterface *formItem = fw->core()->metaDataBase()->item(fw);
    QWidgetList tabOrder = formItem->tabOrder();
    const int tabIndex = tabOrder.indexOf(before);
    if (tabIndex != -1) {
        tabOrder.replace(tabIndex, after);
        formItem->setTabOrder(tabOrder);
    }

    after->show();
    fw->manageWidget(after);

    fw->clearSelection(false);
    fw->selectWidget(after);
}

/* Check if morphing is possible. It must be a valid category and the parent/
 * child relationships must be either non-laidout or directly on
 * Designer-managed layouts. */
bool MorphWidgetCommand::canMorph(QDesignerFormWindowInterface *fw, QWidget *w, int *ptrToChildContainerCount, MorphCategory *ptrToCat)
{
    if (ptrToChildContainerCount)
        *ptrToChildContainerCount = 0;
    const MorphCategory cat = category(w);
    if (ptrToCat)
        *ptrToCat = cat;
    if (cat == MorphCategoryNone)
        return false;

    QDesignerFormEditorInterface *core = fw->core();
    // Don't know how to fiddle class names in Jambi..
    if (qt_extension<QDesignerLanguageExtension *>(core->extensionManager(), core))
        return false;
    if (!fw->isManaged(w) || w == fw->mainContainer())
        return false;
    // Check the parent relationship. We accept only managed parent widgets
    // with a single, managed layout in which widget is a member.
    QWidget *parent = w->parentWidget();
    if (parent == 0)
        return false;
    if (QLayout *pl = LayoutInfo::managedLayout(core, parent))
        if (pl->indexOf(w) < 0 || !core->metaDataBase()->item(pl))
            return false;
    // Check Widget database
    const QDesignerWidgetDataBaseInterface *wdb = core->widgetDataBase();
    const int wdbindex = wdb->indexOfObject(w);
    if (wdbindex == -1)
        return false;
    const bool isContainer = wdb->item(wdbindex)->isContainer();
    if (!isContainer)
        return true;
    // Check children. All child containers must be non-laid-out or have managed layouts
    const QWidgetList pages = childContainers(core, w);
    const int pageCount = pages.size();
    if (ptrToChildContainerCount)
        *ptrToChildContainerCount = pageCount;
    if (pageCount) {
        for (int i = 0; i < pageCount; i++)
            if (QLayout *cl = pages.at(i)->layout())
                if (!core->metaDataBase()->item(cl))
                    return false;
    }
    return true;
}

QStringList MorphWidgetCommand::candidateClasses(QDesignerFormWindowInterface *fw, QWidget *w)
{
    int childContainerCount;
    MorphCategory cat;
    if (!canMorph(fw, w, &childContainerCount, &cat))
        return QStringList();

    QStringList rc = classesOfCategory(cat);
    switch (cat) {
        // Frames, etc can always be morphed into one-page page containers
    case MorphSimpleContainer:
        rc += classesOfCategory(MorphPageContainer);
        break;
        // Multipage-Containers can be morphed into simple containers if they
        // have 1 page.
    case MorphPageContainer:
        if (childContainerCount == 1)
            rc += classesOfCategory(MorphSimpleContainer);
        break;
    default:
        break;
    }
    return rc;
}

// MorphMenu
MorphMenu::MorphMenu(QObject *parent) :
    QObject(parent),
    m_subMenuAction(0),
    m_menu(0),
    m_mapper(0),
    m_widget(0),
    m_formWindow(0)
{
}

void MorphMenu::populate(QWidget *w, QDesignerFormWindowInterface *fw, ActionList& al)
{
    if (populateMenu(w, fw))
        al.push_back(m_subMenuAction);
}

void MorphMenu::populate(QWidget *w, QDesignerFormWindowInterface *fw, QMenu& m)
{
    if (populateMenu(w, fw))
        m.addAction(m_subMenuAction);
}

void MorphMenu::slotMorph(const QString &newClassName)
{
    MorphWidgetCommand::addMorphMacro(m_formWindow, m_widget, newClassName);
}

bool MorphMenu::populateMenu(QWidget *w, QDesignerFormWindowInterface *fw)
{
    m_widget = 0;
    m_formWindow = 0;

    // Clear menu
    if (m_subMenuAction) {
        m_subMenuAction->setVisible(false);
        m_menu->clear();
    }

    // Checks: Must not be main container
    if (w == fw->mainContainer())
        return false;

    const QStringList c = MorphWidgetCommand::candidateClasses(fw, w);
    if (c.empty())
        return false;

    // Pull up
    m_widget = w;
    m_formWindow = fw;
    const QString oldClassName = WidgetFactory::classNameOf(fw->core(), w);

    if (!m_subMenuAction) {
        m_subMenuAction = new QAction(tr("Morph into"), this);
        m_menu = new QMenu;
        m_subMenuAction->setMenu(m_menu);
        m_mapper = new QSignalMapper(this);
        connect(m_mapper , SIGNAL(mapped(QString)), this, SLOT(slotMorph(QString)));
    }

    // Add actions
    const QStringList::const_iterator cend = c.constEnd();
    for (QStringList::const_iterator it = c.constBegin(); it != cend; ++it) {
        if (*it != oldClassName) {
            QAction *a = m_menu->addAction(*it);
            m_mapper->setMapping (a, *it);
            connect(a, SIGNAL(triggered()), m_mapper, SLOT(map()));
        }
    }
    m_subMenuAction->setVisible(true);
    return true;
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
