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

#include "shared_settings_p.h"
#include "grid_p.h"
#include "previewmanager_p.h"
#include "qdesigner_utils_p.h"
#include <QtDesigner/abstractformeditor.h>
#include <QtDesigner/private/abstractsettings_p.h>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QVariantMap>
#include <QtCore/QCoreApplication>
#include <QtCore/QSize>

QT_BEGIN_NAMESPACE

static const char *designerPath = "/.designer";
static const char *defaultGridKey = "defaultGrid";
static const char *previewKey = "Preview";
static const char *enabledKey = "Enabled";
static const char *userDeviceSkinsKey= "UserDeviceSkins";
static const char *zoomKey = "zoom";
static const char *zoomEnabledKey = "zoomEnabled";
static const char *deviceProfileIndexKey = "DeviceProfileIndex";
static const char *deviceProfilesKey = "DeviceProfiles";
static const char *formTemplatePathsKey = "FormTemplatePaths";
static const char *formTemplateKey = "FormTemplate";
static const char *newFormSizeKey = "NewFormSize";

using namespace qdesigner_internal;

static bool checkTemplatePath(const QString &path, bool create)
{
    QDir current(QDir::current());
    if (current.exists(path))
        return true;

    if (!create)
        return false;

    if (current.mkpath(path))
        return true;

    qdesigner_internal::designerWarning(QCoreApplication::translate("QDesignerSharedSettings", "The template path %1 could not be created.").arg(path));
    return false;
}

namespace qdesigner_internal {

QDesignerSharedSettings::QDesignerSharedSettings(QDesignerFormEditorInterface *core)
        : m_settings(core->settingsManager())
{
}

Grid QDesignerSharedSettings::defaultGrid() const
{
    Grid grid;
    const QVariantMap defaultGridMap
            = m_settings->value(QLatin1String(defaultGridKey), QVariantMap()).toMap();
    if (!defaultGridMap.empty())
        grid.fromVariantMap(defaultGridMap);
    return grid;
}

void QDesignerSharedSettings::setDefaultGrid(const Grid &grid)
{
    m_settings->setValue(QLatin1String(defaultGridKey), grid.toVariantMap());
}

const QStringList &QDesignerSharedSettings::defaultFormTemplatePaths()
{
    static QStringList rc;
    if (rc.empty()) {
        // Ensure default form template paths
        const QString templatePath = QLatin1String("/templates");
        // home
        QString path = QDir::homePath();
        path += QLatin1String(designerPath);
        path += templatePath;
        if (checkTemplatePath(path, true))
            rc += path;

        // designer/bin: Might be owned by root in some installations, do not force it.
        path = qApp->applicationDirPath();
        path += templatePath;
        if (checkTemplatePath(path, false))
            rc += path;
    }
    return rc;
}

QStringList QDesignerSharedSettings::formTemplatePaths() const
{
    return m_settings->value(QLatin1String(formTemplatePathsKey),
                            defaultFormTemplatePaths()).toStringList();
}

void QDesignerSharedSettings::setFormTemplatePaths(const QStringList &paths)
{
    m_settings->setValue(QLatin1String(formTemplatePathsKey), paths);
}

QString  QDesignerSharedSettings::formTemplate() const
{
    return m_settings->value(QLatin1String(formTemplateKey)).toString();
}

void QDesignerSharedSettings::setFormTemplate(const QString &t)
{
    m_settings->setValue(QLatin1String(formTemplateKey), t);
}

void QDesignerSharedSettings::setAdditionalFormTemplatePaths(const QStringList &additionalPaths)
{
    // merge template paths
    QStringList templatePaths = defaultFormTemplatePaths();
    templatePaths += additionalPaths;
    setFormTemplatePaths(templatePaths);
}

QStringList QDesignerSharedSettings::additionalFormTemplatePaths() const
{
    // get template paths excluding internal ones
    QStringList rc = formTemplatePaths();
    foreach (const QString &internalTemplatePath, defaultFormTemplatePaths()) {
        const int index = rc.indexOf(internalTemplatePath);
        if (index != -1)
            rc.removeAt(index);
    }
    return rc;
}

QSize QDesignerSharedSettings::newFormSize() const
{
    return m_settings->value(QLatin1String(newFormSizeKey), QSize(0, 0)).toSize();
}

void  QDesignerSharedSettings::setNewFormSize(const QSize &s)
{
    if (s.isNull()) {
        m_settings->remove(QLatin1String(newFormSizeKey));
    } else {
        m_settings->setValue(QLatin1String(newFormSizeKey), s);
    }
}


PreviewConfiguration QDesignerSharedSettings::customPreviewConfiguration() const
{
    PreviewConfiguration configuration;
    configuration.fromSettings(QLatin1String(previewKey), m_settings);
    return configuration;
}

void QDesignerSharedSettings::setCustomPreviewConfiguration(const PreviewConfiguration &configuration)
{
    configuration.toSettings(QLatin1String(previewKey), m_settings);
}

bool QDesignerSharedSettings::isCustomPreviewConfigurationEnabled() const
{
    m_settings->beginGroup(QLatin1String(previewKey));
    bool isEnabled = m_settings->value(QLatin1String(enabledKey), false).toBool();
    m_settings->endGroup();
    return isEnabled;
}

void QDesignerSharedSettings::setCustomPreviewConfigurationEnabled(bool enabled)
{
    m_settings->beginGroup(QLatin1String(previewKey));
    m_settings->setValue(QLatin1String(enabledKey), enabled);
    m_settings->endGroup();
}

QStringList QDesignerSharedSettings::userDeviceSkins() const
{
    m_settings->beginGroup(QLatin1String(previewKey));
    QStringList userDeviceSkins
            = m_settings->value(QLatin1String(userDeviceSkinsKey), QStringList()).toStringList();
    m_settings->endGroup();
    return userDeviceSkins;
}

void QDesignerSharedSettings::setUserDeviceSkins(const QStringList &userDeviceSkins)
{
    m_settings->beginGroup(QLatin1String(previewKey));
    m_settings->setValue(QLatin1String(userDeviceSkinsKey), userDeviceSkins);
    m_settings->endGroup();
}

int QDesignerSharedSettings::zoom() const
{
    return m_settings->value(QLatin1String(zoomKey), 100).toInt();
}

void QDesignerSharedSettings::setZoom(int z)
{
    m_settings->setValue(QLatin1String(zoomKey), QVariant(z));
}

bool QDesignerSharedSettings::zoomEnabled() const
{
    return m_settings->value(QLatin1String(zoomEnabledKey), false).toBool();
}

void QDesignerSharedSettings::setZoomEnabled(bool v)
{
     m_settings->setValue(QLatin1String(zoomEnabledKey), v);
}

DeviceProfile QDesignerSharedSettings::currentDeviceProfile() const
{
    return deviceProfileAt(currentDeviceProfileIndex());
}

void QDesignerSharedSettings::setCurrentDeviceProfileIndex(int i)
{
    m_settings->setValue(QLatin1String(deviceProfileIndexKey), i);
}

int QDesignerSharedSettings::currentDeviceProfileIndex() const
{
     return m_settings->value(QLatin1String(deviceProfileIndexKey), -1).toInt();
}

static inline QString msgWarnDeviceProfileXml(const QString &msg)
{
    return QCoreApplication::translate("QDesignerSharedSettings", "An error has been encountered while parsing device profile XML: %1").arg(msg);
}

DeviceProfile QDesignerSharedSettings::deviceProfileAt(int idx) const
{
    DeviceProfile rc;
    if (idx < 0)
        return rc;
    const QStringList xmls = deviceProfileXml();
    if (idx >= xmls.size())
        return rc;
    QString errorMessage;
    if (!rc.fromXml(xmls.at(idx), &errorMessage)) {
        rc.clear();
        designerWarning(msgWarnDeviceProfileXml(errorMessage));
    }
    return rc;
}

QStringList QDesignerSharedSettings::deviceProfileXml() const
{
    return m_settings->value(QLatin1String(deviceProfilesKey), QStringList()).toStringList();
}

QDesignerSharedSettings::DeviceProfileList QDesignerSharedSettings::deviceProfiles() const
{
    DeviceProfileList rc;
    const QStringList xmls = deviceProfileXml();
    if (xmls.empty())
        return rc;
    // De-serialize
    QString errorMessage;
    DeviceProfile dp;
    const QStringList::const_iterator scend = xmls.constEnd();
    for (QStringList::const_iterator it = xmls.constBegin(); it != scend; ++it) {
        if (dp.fromXml(*it, &errorMessage)) {
            rc.push_back(dp);
        } else {
            designerWarning(msgWarnDeviceProfileXml(errorMessage));
        }
    }
    return rc;
}

void QDesignerSharedSettings::setDeviceProfiles(const DeviceProfileList &dp)
{
    QStringList l;
    const DeviceProfileList::const_iterator dcend = dp.constEnd();
    for (DeviceProfileList::const_iterator it = dp.constBegin(); it != dcend; ++it)
        l.push_back(it->toXml());
    m_settings->setValue(QLatin1String(deviceProfilesKey), l);
}
}

QT_END_NAMESPACE
