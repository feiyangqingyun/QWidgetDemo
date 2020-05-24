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

#include "qsimpleresource_p.h"
#include "widgetfactory_p.h"
#include "widgetdatabase_p.h"

#include <formscriptrunner_p.h>
#include <properties_p.h>
#include <ui4_p.h>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <script_p.h>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerCustomWidgetInterface>
#include <QtDesigner/extrainfo.h>

#include <QtGui/QIcon>
#include <QtGui/QWidget>
#include <QtGui/QAction>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>


QT_BEGIN_NAMESPACE

namespace {
    typedef QList<DomWidgetData*> DomWidgetDataList;
    typedef QList<DomProperty*> DomPropertyList;
    typedef QList<QDesignerCustomWidgetInterface *> CustomWidgetInterfaces;
}

namespace qdesigner_internal {

bool QSimpleResource::m_warningsEnabled = true;

QSimpleResource::QSimpleResource(QDesignerFormEditorInterface *core) :
    QAbstractFormBuilder(),
    m_core(core)
{
    QString workingDirectory = QDir::homePath();
    workingDirectory +=  QDir::separator();
    workingDirectory +=  QLatin1String(".designer");
    setWorkingDirectory(QDir(workingDirectory));
#ifndef QT_FORMBUILDER_NO_SCRIPT
    // Disable scripting in the editors.
    formScriptRunner()-> setOptions(QFormScriptRunner::DisableScripts);
#endif
}

QSimpleResource::~QSimpleResource()
{

}

QBrush QSimpleResource::setupBrush(DomBrush *brush)
{
    return QAbstractFormBuilder::setupBrush(brush);
}

DomBrush *QSimpleResource::saveBrush(const QBrush &brush)
{
    return QAbstractFormBuilder::saveBrush(brush);
}

QIcon QSimpleResource::nameToIcon(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QSimpleResource::nameToIcon() is obsoleted";
    return QIcon();
}

QString QSimpleResource::iconToFilePath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::iconToFilePath() is obsoleted";
    return QString();
}

QString QSimpleResource::iconToQrcPath(const QIcon &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::iconToQrcPath() is obsoleted";
    return QString();
}

QPixmap QSimpleResource::nameToPixmap(const QString &filePath, const QString &qrcPath)
{
    Q_UNUSED(filePath)
    Q_UNUSED(qrcPath)
    qWarning() << "QSimpleResource::nameToPixmap() is obsoleted";
    return QPixmap();
}

QString QSimpleResource::pixmapToFilePath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::pixmapToFilePath() is obsoleted";
    return QString();
}

QString QSimpleResource::pixmapToQrcPath(const QPixmap &pm) const
{
    Q_UNUSED(pm)
    qWarning() << "QSimpleResource::pixmapToQrcPath() is obsoleted";
    return QString();
}

DomScript *QSimpleResource::createScript(const QString &script, ScriptSource source)
{
    if (script.isEmpty())
        return 0;
    DomScript *domScript = new DomScript();
    switch (source) {
    case ScriptExtension:
        domScript->setAttributeSource(QLatin1String("extension"));
        break;
    case ScriptDesigner:
        domScript->setAttributeSource(QLatin1String("designer"));
        break;
    case ScriptCustomWidgetPlugin:
        domScript->setAttributeSource(QLatin1String("customwidgetplugin"));
        break;
    }
    domScript->setAttributeLanguage(QLatin1String("Qt Script"));
    domScript->setText(script);
    return domScript;
}

// Add a script to a list of DomScripts unless empty
void QSimpleResource::addScript(const QString &script, ScriptSource source, DomScripts &domScripts)
{
    if (DomScript *domScript = createScript(script, source)) {
        domScripts += domScript;
    }
}

void QSimpleResource::addExtensionDataToDOM(QAbstractFormBuilder *afb,
                                            QDesignerFormEditorInterface *core,
                                            DomWidget *ui_widget, QWidget *widget)
{
    QExtensionManager *emgr = core->extensionManager();
    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(emgr, widget)) {
        extra->saveWidgetExtraInfo(ui_widget);
    }
    if (QDesignerScriptExtension *scriptExt = qt_extension<QDesignerScriptExtension*>(emgr, widget)) {
        // Add internal state
        const QVariantMap data = scriptExt->data();
        if (!data.empty()) {
            // Convert the map to a DomState.
            // We pass on the widget for property introspection. Thus, non-designable properties
            // that have to be converted using QMetaObject (enums and the like) will work.
            DomPropertyList properties;
            const QVariantMap::const_iterator vcend = data.constEnd();
            for (QVariantMap::const_iterator it = data.constBegin(); it != vcend; ++it) {
                if (DomProperty *prop = variantToDomProperty(afb, widget->metaObject(), it.key(), it.value()))
                    properties += prop;
            }
            if (!properties.empty()) {
                DomWidgetData *domData = new DomWidgetData;
                domData->setElementProperty(properties);
                DomWidgetDataList domDataList;
                 domDataList += domData;
                ui_widget->setElementWidgetData(domDataList);
            }

        }
        // Add script
        const QString script = scriptExt->script();
        if (!script.isEmpty()) {
            DomScripts domScripts = ui_widget->elementScript();
            addScript(script, ScriptExtension, domScripts);
            ui_widget->setElementScript(domScripts);
        }
    }
}

void QSimpleResource::applyExtensionDataFromDOM(QAbstractFormBuilder *afb,
                                                QDesignerFormEditorInterface *core,
                                                DomWidget *ui_widget, QWidget *widget, bool applyState)
{
    QExtensionManager *emgr = core->extensionManager();
    if (QDesignerExtraInfoExtension *extra = qt_extension<QDesignerExtraInfoExtension*>(emgr, widget)) {
        extra->loadWidgetExtraInfo(ui_widget);
    }
    if (applyState) {
        if (QDesignerScriptExtension *scriptExt = qt_extension<QDesignerScriptExtension*>(emgr, widget)) {
            // Apply the state.
            // We pass on the widget for property introspection. Thus, non-designable properties
            // that have to be converted using QMetaObject (enums and the like) will work.
            QVariantMap data;
            DomWidgetDataList domDataList = ui_widget->elementWidgetData();
            if (!domDataList.empty()) {
                foreach (const DomWidgetData *domData, domDataList) {
                    const DomPropertyList properties = domData->elementProperty();
                    foreach(const DomProperty *prop, properties) {
                        const QVariant vprop = domPropertyToVariant(afb, widget->metaObject(), prop);
                        if (vprop.type() != QVariant::Invalid)
                            data.insert(prop->attributeName(), vprop);
                    }
                }
            }
            scriptExt->setData(data);
        }
    }
}

QString QSimpleResource::customWidgetScript(QDesignerFormEditorInterface *core, QObject *object)
{
    return customWidgetScript(core, qdesigner_internal::WidgetFactory::classNameOf(core, object));
}

bool QSimpleResource::hasCustomWidgetScript(QDesignerFormEditorInterface *, QObject *)
{
    return false;
}

QString QSimpleResource::customWidgetScript(QDesignerFormEditorInterface *, const QString &)
{
    return QString();
}

bool QSimpleResource::setWarningsEnabled(bool warningsEnabled)
{
    const bool rc = m_warningsEnabled;
    m_warningsEnabled = warningsEnabled;
    return rc;
}

bool QSimpleResource::warningsEnabled()
{
    return m_warningsEnabled;
}

// Custom widgets handling helpers

// Add unique fake slots and signals to lists
bool QSimpleResource::addFakeMethods(const DomSlots *domSlots, QStringList &fakeSlots, QStringList &fakeSignals)
{
    if (!domSlots)
        return false;

    bool rc = false;
    foreach (const QString &fakeSlot, domSlots->elementSlot())
        if (fakeSlots.indexOf(fakeSlot) == -1) {
            fakeSlots += fakeSlot;
            rc = true;
        }

    foreach (const QString &fakeSignal, domSlots->elementSignal())
        if (fakeSignals.indexOf(fakeSignal) == -1) {
            fakeSignals += fakeSignal;
            rc = true;
        }
    return rc;
}

void QSimpleResource::addFakeMethodsToWidgetDataBase(const DomCustomWidget *domCustomWidget, WidgetDataBaseItem *item)
{
    const DomSlots *domSlots = domCustomWidget->elementSlots();
    if (!domSlots)
        return;

    // Merge in new slots, signals
    QStringList fakeSlots = item->fakeSlots();
    QStringList fakeSignals = item->fakeSignals();
    if (addFakeMethods(domSlots, fakeSlots, fakeSignals)) {
        item->setFakeSlots(fakeSlots);
        item->setFakeSignals(fakeSignals);
    }
}

// Perform one iteration of adding the custom widgets to the database,
// looking up the base class and inheriting its data.
// Remove the succeeded custom widgets from the list.
// Classes whose base class could not be found are left in the list.

void QSimpleResource::addCustomWidgetsToWidgetDatabase(const QDesignerFormEditorInterface *core,
                                                       QList<DomCustomWidget*>& custom_widget_list)
{
    QDesignerWidgetDataBaseInterface *db = core->widgetDataBase();
    for (int i=0; i < custom_widget_list.size(); ) {
        bool classInserted = false;
        DomCustomWidget *custom_widget = custom_widget_list[i];
        const QString customClassName = custom_widget->elementClass();
        const QString base_class = custom_widget->elementExtends();
        QString includeFile;
        IncludeType includeType = IncludeLocal;
        if (const DomHeader *header = custom_widget->elementHeader()) {
            includeFile = header->text();
            if (header->hasAttributeLocation() && header->attributeLocation() == QLatin1String("global"))
                includeType = IncludeGlobal;
        }
        const bool domIsContainer = custom_widget->elementContainer();
        // Append a new item
        if (base_class.isEmpty()) {
            WidgetDataBaseItem *item = new WidgetDataBaseItem(customClassName);
            item->setPromoted(false);
            item->setGroup(QCoreApplication::translate("Designer", "Custom Widgets"));
            item->setIncludeFile(buildIncludeFile(includeFile, includeType));
            item->setContainer(domIsContainer);
            item->setCustom(true);
            addFakeMethodsToWidgetDataBase(custom_widget, item);
            db->append(item);
            custom_widget_list.removeAt(i);
            classInserted = true;
        } else {
            // Create a new entry cloned from base class. Note that this will ignore existing
            // classes, eg, plugin custom widgets.
            QDesignerWidgetDataBaseItemInterface *item =
                appendDerived(db, customClassName, QCoreApplication::translate("Designer", "Promoted Widgets"),
                              base_class,
                              buildIncludeFile(includeFile, includeType),
                              true,true);
            // Ok, base class found.
            if (item) {
                // Hack to accommodate for old UI-files in which "container" is not set properly:
                // Apply "container" from DOM only if true (else, eg classes from QFrame might not accept
                // dropping child widgets on them as container=false). This also allows for
                // QWidget-derived stacked pages.
                if (domIsContainer)
                    item->setContainer(domIsContainer);

                addFakeMethodsToWidgetDataBase(custom_widget, static_cast<WidgetDataBaseItem*>(item));
                custom_widget_list.removeAt(i);
                classInserted = true;
            }
        }
        // Skip failed item.
        if (!classInserted)
            i++;
    }

}

void QSimpleResource::handleDomCustomWidgets(const QDesignerFormEditorInterface *core,
                                             const DomCustomWidgets *dom_custom_widgets)
{
    if (dom_custom_widgets == 0)
        return;
    QList<DomCustomWidget*> custom_widget_list = dom_custom_widgets->elementCustomWidget();
    // Attempt to insert each item derived from its base class.
    // This should at most require two iterations in the event that the classes are out of order
    // (derived first, max depth: promoted custom plugin = 2)
    for (int iteration = 0;  iteration < 2;  iteration++) {
        addCustomWidgetsToWidgetDatabase(core, custom_widget_list);
        if (custom_widget_list.empty())
            return;
    }
    // Oops, there are classes left whose base class could not be found.
    // Default them to QWidget with warnings.
    const QString fallBackBaseClass = QLatin1String("QWidget");
    for (int i=0; i < custom_widget_list.size(); i++ ) {
        DomCustomWidget *custom_widget = custom_widget_list[i];
        const QString customClassName = custom_widget->elementClass();
        const QString base_class = custom_widget->elementExtends();
        qDebug() << "** WARNING The base class " << base_class << " of the custom widget class " << customClassName
            << " could not be found. Defaulting to " << fallBackBaseClass << '.';
        custom_widget->setElementExtends(fallBackBaseClass);
    }
    // One more pass.
    addCustomWidgetsToWidgetDatabase(core, custom_widget_list);
}

// ------------ FormBuilderClipboard

FormBuilderClipboard::FormBuilderClipboard(QWidget *w)
{
    m_widgets += w;
}

bool FormBuilderClipboard::empty() const
{
    return m_widgets.empty() && m_actions.empty();
}
}

QT_END_NAMESPACE
