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

#include "formwindowbase_p.h"
#include "connectionedit_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertysheet_p.h"
#include "qdesigner_propertyeditor_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_menubar_p.h"
#include "shared_settings_p.h"
#include "grid_p.h" 
#include "deviceprofile_p.h"
#include "qdesigner_utils_p.h"

#include "qsimpleresource_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerTaskMenuExtension>

#include <QtCore/qdebug.h>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QListWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QComboBox>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBox>
#include <QtGui/QToolBar>
#include <QtGui/QStatusBar>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class FormWindowBasePrivate {
public:
    explicit FormWindowBasePrivate(QDesignerFormEditorInterface *core);

    static Grid m_defaultGrid;

    QDesignerFormWindowInterface::Feature m_feature;
    Grid m_grid;
    bool m_hasFormGrid;
    DesignerPixmapCache *m_pixmapCache;
    DesignerIconCache *m_iconCache;
    QtResourceSet *m_resourceSet;
    QMap<QDesignerPropertySheet *, QMap<int, bool> > m_reloadableResources; // bool is dummy, QMap used as QSet
    QMap<QDesignerPropertySheet *, QObject *> m_reloadablePropertySheets;
    const DeviceProfile m_deviceProfile;
    FormWindowBase::LineTerminatorMode m_lineTerminatorMode;
    FormWindowBase::SaveResourcesBehaviour m_saveResourcesBehaviour;
};

FormWindowBasePrivate::FormWindowBasePrivate(QDesignerFormEditorInterface *core) :
    m_feature(QDesignerFormWindowInterface::DefaultFeature),
    m_grid(m_defaultGrid),
    m_hasFormGrid(false),
    m_pixmapCache(0),
    m_iconCache(0),
    m_resourceSet(0),
    m_deviceProfile(QDesignerSharedSettings(core).currentDeviceProfile()),
    m_lineTerminatorMode(FormWindowBase::NativeLineTerminator),
    m_saveResourcesBehaviour(FormWindowBase::SaveAll)
{
}

Grid FormWindowBasePrivate::m_defaultGrid;

FormWindowBase::FormWindowBase(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags) :
    QDesignerFormWindowInterface(parent, flags),
    m_d(new FormWindowBasePrivate(core))
{
    syncGridFeature();
    m_d->m_pixmapCache = new DesignerPixmapCache(this);
    m_d->m_iconCache = new DesignerIconCache(m_d->m_pixmapCache, this);
}

FormWindowBase::~FormWindowBase()
{
    delete m_d;
}

DesignerPixmapCache *FormWindowBase::pixmapCache() const
{
    return m_d->m_pixmapCache;
}

DesignerIconCache *FormWindowBase::iconCache() const
{
    return m_d->m_iconCache;
}

QtResourceSet *FormWindowBase::resourceSet() const
{
    return m_d->m_resourceSet;
}

void FormWindowBase::setResourceSet(QtResourceSet *resourceSet)
{
    m_d->m_resourceSet = resourceSet;
}

void FormWindowBase::addReloadableProperty(QDesignerPropertySheet *sheet, int index)
{
    m_d->m_reloadableResources[sheet][index] = true;
}

void FormWindowBase::removeReloadableProperty(QDesignerPropertySheet *sheet, int index)
{
    m_d->m_reloadableResources[sheet].remove(index);
    if (m_d->m_reloadableResources[sheet].count() == 0)
        m_d->m_reloadableResources.remove(sheet);
}

void FormWindowBase::addReloadablePropertySheet(QDesignerPropertySheet *sheet, QObject *object)
{
    if (qobject_cast<QTreeWidget *>(object) ||
            qobject_cast<QTableWidget *>(object) ||
            qobject_cast<QListWidget *>(object) ||
            qobject_cast<QComboBox *>(object))
        m_d->m_reloadablePropertySheets[sheet] = object;
}

void FormWindowBase::removeReloadablePropertySheet(QDesignerPropertySheet *sheet)
{
    m_d->m_reloadablePropertySheets.remove(sheet);
}

void FormWindowBase::reloadProperties()
{
    pixmapCache()->clear();
    iconCache()->clear();
    QMapIterator<QDesignerPropertySheet *, QMap<int, bool> > itSheet(m_d->m_reloadableResources);
    while (itSheet.hasNext()) {
        QDesignerPropertySheet *sheet = itSheet.next().key();
        QMapIterator<int, bool> itIndex(itSheet.value());
        while (itIndex.hasNext()) {
            const int index = itIndex.next().key();
            const QVariant newValue = sheet->property(index);
            if (qobject_cast<QLabel *>(sheet->object()) && sheet->propertyName(index) == QLatin1String("text")) {
                const PropertySheetStringValue newString = qVariantValue<PropertySheetStringValue>(newValue);
                // optimize a bit, reset only if the text value might contain a reference to qt resources
                // (however reloading of icons other than taken from resources might not work here)
                if (newString.value().contains(QLatin1String(":/"))) {
                    const QVariant resetValue = qVariantFromValue(PropertySheetStringValue());
                    sheet->setProperty(index, resetValue);
                }
            }
            sheet->setProperty(index, newValue);
        }
        if (QTabWidget *tabWidget = qobject_cast<QTabWidget *>(sheet->object())) {
            const int count = tabWidget->count();
            const int current = tabWidget->currentIndex();
            const QString currentTabIcon = QLatin1String("currentTabIcon");
            for (int i = 0; i < count; i++) {
                tabWidget->setCurrentIndex(i);
                const int index = sheet->indexOf(currentTabIcon);
                sheet->setProperty(index, sheet->property(index));
            }
            tabWidget->setCurrentIndex(current);
        } else if (QToolBox *toolBox = qobject_cast<QToolBox *>(sheet->object())) {
            const int count = toolBox->count();
            const int current = toolBox->currentIndex();
            const QString currentItemIcon = QLatin1String("currentItemIcon");
            for (int i = 0; i < count; i++) {
                toolBox->setCurrentIndex(i);
                const int index = sheet->indexOf(currentItemIcon);
                sheet->setProperty(index, sheet->property(index));
            }
            toolBox->setCurrentIndex(current);
        }
    }
    QMapIterator<QDesignerPropertySheet *, QObject *> itSh(m_d->m_reloadablePropertySheets);
    while (itSh.hasNext()) {
        QObject *object = itSh.next().value();
        reloadIconResources(iconCache(), object);
    }
}

void FormWindowBase::resourceSetActivated(QtResourceSet *resource, bool resourceSetChanged)
{
    if (resource == resourceSet() && resourceSetChanged) {
        reloadProperties();
        emit pixmapCache()->reloaded();
        emit iconCache()->reloaded();
        if (QDesignerPropertyEditor *propertyEditor = qobject_cast<QDesignerPropertyEditor *>(core()->propertyEditor()))
            propertyEditor->reloadResourceProperties();
    }
}

QVariantMap FormWindowBase::formData()
{
    QVariantMap rc;
    if (m_d->m_hasFormGrid)
        m_d->m_grid.addToVariantMap(rc, true);
    return rc;
}

void FormWindowBase::setFormData(const QVariantMap &vm)
{
    Grid formGrid;
    m_d->m_hasFormGrid = formGrid.fromVariantMap(vm);
    if (m_d->m_hasFormGrid)
         m_d->m_grid = formGrid;
}

QPoint FormWindowBase::grid() const
{
    return QPoint(m_d->m_grid.deltaX(), m_d->m_grid.deltaY());
}

void FormWindowBase::setGrid(const QPoint &grid)
{
    m_d->m_grid.setDeltaX(grid.x());
    m_d->m_grid.setDeltaY(grid.y());
}

bool FormWindowBase::hasFeature(Feature f) const
{
    return f & m_d->m_feature;
}

static void recursiveUpdate(QWidget *w)
{
    w->update();

    const QObjectList &l = w->children();
    const QObjectList::const_iterator cend = l.constEnd();
    for (QObjectList::const_iterator it = l.constBegin(); it != cend; ++it) {
        if (QWidget *w = qobject_cast<QWidget*>(*it))
            recursiveUpdate(w);
    }
}

void FormWindowBase::setFeatures(Feature f)
{
    m_d->m_feature = f;
    const bool enableGrid = f & GridFeature;
    m_d->m_grid.setVisible(enableGrid);
    m_d->m_grid.setSnapX(enableGrid);
    m_d->m_grid.setSnapY(enableGrid);
    emit featureChanged(f);
    recursiveUpdate(this);
}

FormWindowBase::Feature FormWindowBase::features() const
{
    return m_d->m_feature;
}

bool FormWindowBase::gridVisible() const
{
    return m_d->m_grid.visible() && currentTool() == 0;
}

FormWindowBase::SaveResourcesBehaviour FormWindowBase::saveResourcesBehaviour() const
{
    return m_d->m_saveResourcesBehaviour;
}

void FormWindowBase::setSaveResourcesBehaviour(SaveResourcesBehaviour behaviour)
{
    m_d->m_saveResourcesBehaviour = behaviour;
}

void FormWindowBase::syncGridFeature()
{
    if (m_d->m_grid.snapX() || m_d->m_grid.snapY())
        m_d->m_feature |= GridFeature;
    else
        m_d->m_feature &= ~GridFeature;
}

void FormWindowBase::setDesignerGrid(const  Grid& grid)
{
    m_d->m_grid = grid;
    syncGridFeature();
    recursiveUpdate(this);
}

const Grid &FormWindowBase::designerGrid() const
{
    return m_d->m_grid;
}

bool FormWindowBase::hasFormGrid() const
{
    return m_d->m_hasFormGrid;
}

void FormWindowBase::setHasFormGrid(bool b)
{
    m_d->m_hasFormGrid = b;
}

void FormWindowBase::setDefaultDesignerGrid(const Grid& grid)
{
    FormWindowBasePrivate::m_defaultGrid = grid;
}

const Grid &FormWindowBase::defaultDesignerGrid()
{
    return FormWindowBasePrivate::m_defaultGrid;
}

QMenu *FormWindowBase::initializePopupMenu(QWidget * /*managedWidget*/)
{
    return 0;
}

// Widget under mouse for finding the Widget to highlight
// when doing DnD. Restricts to pages by geometry if a container with
// a container extension (or one of its helper widgets) is hit; otherwise
// returns the widget as such (be it managed/unmanaged)

QWidget *FormWindowBase::widgetUnderMouse(const QPoint &formPos, WidgetUnderMouseMode /* wum */)
{
    // widget_under_mouse might be some temporary thing like the dropLine. We need
    // the actual widget that's part of the edited GUI.
    QWidget *rc = widgetAt(formPos);
    if (!rc || qobject_cast<ConnectionEdit*>(rc))
        return 0;

    if (rc == mainContainer()) {
        // Refuse main container areas if the main container has a container extension,
        // for example when hitting QToolBox/QTabWidget empty areas.
        if (qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), rc))
            return 0;
        return rc;
    }

    // If we hit on container extension type container, make sure
    // we use the top-most current page
    if (QWidget *container = findContainer(rc, false))
        if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), container)) {
            // For container that do not have a "stacked" nature (QToolBox, QMdiArea),
            // make sure the position is within the current page
            const int ci = c->currentIndex();
            if (ci < 0)
                return 0;
            QWidget *page = c->widget(ci);
            QRect pageGeometry = page->geometry();
            pageGeometry.moveTo(page->mapTo(this, pageGeometry.topLeft()));
            if (!pageGeometry.contains(formPos))
                return 0;
            return page;
        }

    return rc;
}

void FormWindowBase::deleteWidgetList(const QWidgetList &widget_list)
{
    // We need a macro here even for single widgets because the some components (for example,
    // the signal slot editor are connected to widgetRemoved() and add their
    // own commands (for example, to delete w's connections)
    const QString description = widget_list.size() == 1 ?
        tr("Delete '%1'").arg(widget_list.front()->objectName()) : tr("Delete");

    commandHistory()->beginMacro(description);
    foreach (QWidget *w, widget_list) {
        emit widgetRemoved(w);
        DeleteWidgetCommand *cmd = new DeleteWidgetCommand(this);
        cmd->init(w);
        commandHistory()->push(cmd);
    }
    commandHistory()->endMacro();
}

QMenu *FormWindowBase::createExtensionTaskMenu(QDesignerFormWindowInterface *fw, QObject *o, bool trailingSeparator)
{
    typedef QList<QAction *> ActionList;
    ActionList actions;
    // 1) Standard public extension
    QExtensionManager *em = fw->core()->extensionManager();
    if (const QDesignerTaskMenuExtension *extTaskMenu = qt_extension<QDesignerTaskMenuExtension*>(em, o))
        actions += extTaskMenu->taskActions();
    if (const QDesignerTaskMenuExtension *intTaskMenu = qobject_cast<QDesignerTaskMenuExtension *>(em->extension(o, QLatin1String("QDesignerInternalTaskMenuExtension")))) {
        if (!actions.empty()) {
            QAction *a = new QAction(fw);
            a->setSeparator(true);
            actions.push_back(a);
        }
        actions += intTaskMenu->taskActions();
    }
    if (actions.empty())
        return 0;
    if (trailingSeparator && !actions.back()->isSeparator()) {
        QAction *a  = new QAction(fw);
        a->setSeparator(true);
        actions.push_back(a);
    }
    QMenu *rc = new QMenu;
    const ActionList::const_iterator cend = actions.constEnd();
    for (ActionList::const_iterator it = actions.constBegin(); it != cend; ++it)
        rc->addAction(*it);
    return rc;
}

void FormWindowBase::emitObjectRemoved(QObject *o)
{
    emit objectRemoved(o);
}

DeviceProfile FormWindowBase::deviceProfile() const
{
    return m_d->m_deviceProfile;
}

QString FormWindowBase::styleName() const
{
    return m_d->m_deviceProfile.isEmpty() ? QString() : m_d->m_deviceProfile.style();
}

void FormWindowBase::emitWidgetRemoved(QWidget *w)
{
    emit widgetRemoved(w);
}

QString FormWindowBase::deviceProfileName() const
{
    return m_d->m_deviceProfile.isEmpty() ? QString() : m_d->m_deviceProfile.name();
}

void FormWindowBase::setLineTerminatorMode(FormWindowBase::LineTerminatorMode mode)
{
    m_d->m_lineTerminatorMode = mode;
}

FormWindowBase::LineTerminatorMode FormWindowBase::lineTerminatorMode() const
{
    return m_d->m_lineTerminatorMode;
}

void FormWindowBase::triggerDefaultAction(QWidget *widget)
{
    if (QAction *action = qdesigner_internal::preferredEditAction(core(), widget))
        QTimer::singleShot(0, action, SIGNAL(triggered()));
}

void FormWindowBase::setupDefaultAction(QDesignerFormWindowInterface *fw)
{
    QObject::connect(fw, SIGNAL(activated(QWidget*)), fw, SLOT(triggerDefaultAction(QWidget*)));
}

QString FormWindowBase::fileContents() const
{
    const bool oldValue = QSimpleResource::setWarningsEnabled(false);
    const QString rc = contents();
    QSimpleResource::setWarningsEnabled(oldValue);
    return rc;
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
