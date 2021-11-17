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

#ifndef ABSTRACTFORMEDITOR_H
#define ABSTRACTFORMEDITOR_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/QObject>
#include <QtCore/QPointer>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDesignerWidgetBoxInterface;
class QDesignerPropertyEditorInterface;
class QDesignerFormWindowManagerInterface;
class QDesignerWidgetDataBaseInterface;
class QDesignerMetaDataBaseInterface;
class QDesignerWidgetFactoryInterface;
class QDesignerObjectInspectorInterface;
class QDesignerPromotionInterface;
class QDesignerBrushManagerInterface;
class QDesignerIconCacheInterface;
class QDesignerActionEditorInterface;
class QDesignerIntegrationInterface;
class QDesignerPluginManager;
class QDesignerIntrospectionInterface;
class QDesignerDialogGuiInterface;
class QDesignerSettingsInterface;
class QDesignerOptionsPageInterface;
class QtResourceModel;
class QtGradientManager;

class QWidget;

class QExtensionManager;

class  QDesignerFormEditorInterfacePrivate;

class QDESIGNER_SDK_EXPORT QDesignerFormEditorInterface : public QObject
{
    Q_OBJECT
public:
    QDesignerFormEditorInterface(QObject *parent = 0);
    virtual ~QDesignerFormEditorInterface();

    QExtensionManager *extensionManager() const;

    QWidget *topLevel() const;
    QDesignerWidgetBoxInterface *widgetBox() const;
    QDesignerPropertyEditorInterface *propertyEditor() const;
    QDesignerObjectInspectorInterface *objectInspector() const;
    QDesignerFormWindowManagerInterface *formWindowManager() const;
    QDesignerWidgetDataBaseInterface *widgetDataBase() const;
    QDesignerMetaDataBaseInterface *metaDataBase() const;
    QDesignerPromotionInterface *promotion() const;
    QDesignerWidgetFactoryInterface *widgetFactory() const;
    QDesignerBrushManagerInterface *brushManager() const;
    QDesignerIconCacheInterface *iconCache() const;
    QDesignerActionEditorInterface *actionEditor() const;
    QDesignerIntegrationInterface *integration() const;
    QDesignerPluginManager *pluginManager() const;
    QDesignerIntrospectionInterface *introspection() const;
    QDesignerDialogGuiInterface *dialogGui() const;
    QDesignerSettingsInterface *settingsManager() const;
    QString resourceLocation() const;
    QtResourceModel *resourceModel() const;
    QtGradientManager *gradientManager() const;
    QList<QDesignerOptionsPageInterface*> optionsPages() const;

    void setTopLevel(QWidget *topLevel);
    void setWidgetBox(QDesignerWidgetBoxInterface *widgetBox);
    void setPropertyEditor(QDesignerPropertyEditorInterface *propertyEditor);
    void setObjectInspector(QDesignerObjectInspectorInterface *objectInspector);
    void setPluginManager(QDesignerPluginManager *pluginManager);
    void setActionEditor(QDesignerActionEditorInterface *actionEditor);
    void setIntegration(QDesignerIntegrationInterface *integration);
    void setIntrospection(QDesignerIntrospectionInterface *introspection);
    void setDialogGui(QDesignerDialogGuiInterface *dialogGui);
    void setSettingsManager(QDesignerSettingsInterface *settingsManager);
    void setResourceModel(QtResourceModel *model);
    void setGradientManager(QtGradientManager *manager);
    void setOptionsPages(const QList<QDesignerOptionsPageInterface*> &optionsPages);

protected:
    void setFormManager(QDesignerFormWindowManagerInterface *formWindowManager);
    void setMetaDataBase(QDesignerMetaDataBaseInterface *metaDataBase);
    void setWidgetDataBase(QDesignerWidgetDataBaseInterface *widgetDataBase);
    void setPromotion(QDesignerPromotionInterface *promotion);
    void setWidgetFactory(QDesignerWidgetFactoryInterface *widgetFactory);
    void setExtensionManager(QExtensionManager *extensionManager);
    void setBrushManager(QDesignerBrushManagerInterface *brushManager);
    void setIconCache(QDesignerIconCacheInterface *cache);

private:
    QPointer<QWidget> m_pad1;
    QPointer<QDesignerWidgetBoxInterface> m_pad2;
    QPointer<QDesignerPropertyEditorInterface> m_pad3;
    QPointer<QDesignerFormWindowManagerInterface> m_pad4;
    QPointer<QExtensionManager> m_pad5;
    QPointer<QDesignerMetaDataBaseInterface> m_pad6;
    QPointer<QDesignerWidgetDataBaseInterface> m_pad7;
    QPointer<QDesignerWidgetFactoryInterface> m_pad8;
    QPointer<QDesignerObjectInspectorInterface> m_pad9;
    QPointer<QDesignerBrushManagerInterface> m_pad10;
    QPointer<QDesignerIconCacheInterface> m_pad11;
    QPointer<QDesignerActionEditorInterface> m_pad12;
    QDesignerFormEditorInterfacePrivate *d;

private:
    QDesignerFormEditorInterface(const QDesignerFormEditorInterface &other);
    void operator = (const QDesignerFormEditorInterface &other);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // ABSTRACTFORMEDITOR_H
