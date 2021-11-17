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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_FORMBUILDER_H
#define QDESIGNER_FORMBUILDER_H

#include "shared_global_p.h"
#include "deviceprofile_p.h"

#include <QtDesigner/private/formscriptrunner_p.h>
#include <QtDesigner/formbuilder.h>

#include <QtCore/QMap>
#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

class QPixmap;
class QtResourceSet;

namespace qdesigner_internal {

class DesignerPixmapCache;
class DesignerIconCache;

/* Form builder used for previewing forms and widget box.
 * It applies the system settings to its toplevel window. */

class QDESIGNER_SHARED_EXPORT QDesignerFormBuilder: public QFormBuilder
{
public:
    enum Mode {
        DisableScripts,
        EnableScripts
    };

    QDesignerFormBuilder(QDesignerFormEditorInterface *core,
                         Mode mode,
                         const DeviceProfile &deviceProfile = DeviceProfile());

    QWidget *createWidgetFromContents(const QString &contents, QWidget *parentWidget = 0);

    virtual QWidget *createWidget(DomWidget *ui_widget, QWidget *parentWidget = 0)
    { return QFormBuilder::create(ui_widget, parentWidget); }

    inline QDesignerFormEditorInterface *core() const
    { return m_core; }

    QString systemStyle() const;

    typedef QFormScriptRunner::Errors ScriptErrors;
    // Create a preview widget (for integrations) or return 0. The widget has to be embedded into a main window.
    // Experimental, depending on script support.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName /* ="" */,
                                  const QString &appStyleSheet  /* ="" */,
                                  const DeviceProfile &deviceProfile,
                                  ScriptErrors *scriptErrors, QString *errorMessage);
    // Convenience that pops up message boxes in case of failures.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName = QString());
    //  Create a preview widget (for integrations) or return 0. The widget has to be embedded into a main window.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName, const QString &appStyleSheet, QString *errorMessage);
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName, const QString &appStyleSheet, const DeviceProfile &deviceProfile, QString *errorMessage);
    // Convenience that pops up message boxes in case of failures.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName, const QString &appStyleSheet);

    // Create a preview image
    static QPixmap createPreviewPixmap(const QDesignerFormWindowInterface *fw, const QString &styleName = QString(), const QString &appStyleSheet = QString());

protected:
    using QFormBuilder::createDom;
    using QFormBuilder::create;

    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual DomWidget *createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive = true);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);
    virtual QLayout *create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget);
    virtual void createResources(DomResources *resources);

    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
    virtual bool addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);
    virtual bool addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout);

    virtual QIcon nameToIcon(const QString &filePath, const QString &qrcPath);
    virtual QPixmap nameToPixmap(const QString &filePath, const QString &qrcPath);

    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);

    virtual void loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

    QtResourceSet *internalResourceSet() const { return m_tempResourceSet; }

    DeviceProfile deviceProfile() const { return m_deviceProfile; }

private:
    QDesignerFormEditorInterface *m_core;
    const Mode m_mode;

    typedef QSet<QWidget *> WidgetSet;
    WidgetSet m_customWidgetsWithScript;

    const DeviceProfile m_deviceProfile;

    DesignerPixmapCache *m_pixmapCache;
    DesignerIconCache *m_iconCache;
    bool m_ignoreCreateResources;
    QtResourceSet *m_tempResourceSet;
    bool m_mainWidget;
};

// Form builder for a new form widget (preview). To allow for promoted
// widgets in the template, it implements the handling of custom widgets
// (adding of them to the widget database).

class QDESIGNER_SHARED_EXPORT NewFormWidgetFormBuilder: public QDesignerFormBuilder {
public:
    NewFormWidgetFormBuilder(QDesignerFormEditorInterface *core,
                             Mode mode,
                             const DeviceProfile &deviceProfile = DeviceProfile());

protected:
    virtual void createCustomWidgets(DomCustomWidgets *);
};


} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_FORMBUILDER_H
