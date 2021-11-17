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


#include "plugindialog_p.h"
#include "pluginmanager_p.h"
#include "qdesigner_integration_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerCustomWidgetCollectionInterface>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>

#include <QtGui/QStyle>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtCore/QFileInfo>
#include <QtCore/QPluginLoader>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

PluginDialog::PluginDialog(QDesignerFormEditorInterface *core, QWidget *parent)
    : QDialog(parent
#ifdef Q_WS_MAC
            , Qt::Tool
#endif
            ), m_core(core)
{
    ui.setupUi(this);

    ui.message->hide();

    const QStringList headerLabels(tr("Components"));

    ui.treeWidget->setAlternatingRowColors(false);
    ui.treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui.treeWidget->setHeaderLabels(headerLabels);
    ui.treeWidget->header()->hide();

    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                            QIcon::Normal, QIcon::On);
    interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                            QIcon::Normal, QIcon::Off);
    featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    setWindowTitle(tr("Plugin Information"));
    populateTreeWidget();

    if (qobject_cast<qdesigner_internal::QDesignerIntegration *>(m_core->integration())) {
        QPushButton *updateButton = new QPushButton(tr("Refresh"));
        const QString tooltip = tr("Scan for newly installed custom widget plugins.");
        updateButton->setToolTip(tooltip);
        updateButton->setWhatsThis(tooltip);
        connect(updateButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetPlugins()));
        ui.buttonBox->addButton(updateButton, QDialogButtonBox::ActionRole);
    }
}

void PluginDialog::populateTreeWidget()
{
    ui.treeWidget->clear();
    QDesignerPluginManager *pluginManager = m_core->pluginManager();
    const QStringList fileNames = pluginManager->registeredPlugins();

    if (!fileNames.isEmpty()) {
        QTreeWidgetItem *topLevelItem = setTopLevelItem(QLatin1String("Loaded Plugins"));
        QFont boldFont = topLevelItem->font(0);

        foreach (const QString &fileName, fileNames) {
            QPluginLoader loader(fileName);
            const QFileInfo fileInfo(fileName);

            QTreeWidgetItem *pluginItem = setPluginItem(topLevelItem, fileInfo.fileName(), boldFont);

            if (QObject *plugin = loader.instance()) {
                if (const QDesignerCustomWidgetCollectionInterface *c = qobject_cast<QDesignerCustomWidgetCollectionInterface*>(plugin)) {
                    foreach (const QDesignerCustomWidgetInterface *p, c->customWidgets())
                        setItem(pluginItem, p->name(), p->toolTip(), p->whatsThis(), p->icon());
                } else {
                    if (const QDesignerCustomWidgetInterface *p = qobject_cast<QDesignerCustomWidgetInterface*>(plugin))
                        setItem(pluginItem, p->name(), p->toolTip(), p->whatsThis(), p->icon());
                }
            }
        }
    }

    const QStringList notLoadedPlugins = pluginManager->failedPlugins();
    if (!notLoadedPlugins.isEmpty()) {
        QTreeWidgetItem *topLevelItem = setTopLevelItem(QLatin1String("Failed Plugins"));
        const QFont boldFont = topLevelItem->font(0);
        foreach (const QString &plugin, notLoadedPlugins) {
            const QString failureReason = pluginManager->failureReason(plugin);
            QTreeWidgetItem *pluginItem = setPluginItem(topLevelItem, plugin, boldFont);
            setItem(pluginItem, failureReason, failureReason, QString(), QIcon());
        }
    }

    if (ui.treeWidget->topLevelItemCount() == 0) {
        ui.label->setText(tr("Qt Designer couldn't find any plugins"));
        ui.treeWidget->hide();
    } else {
        ui.label->setText(tr("Qt Designer found the following plugins"));
    }
}

QIcon PluginDialog::pluginIcon(const QIcon &icon)
{
    if (icon.isNull())
        return QIcon(QLatin1String(":/trolltech/formeditor/images/qtlogo.png"));

    return icon;
}

QTreeWidgetItem* PluginDialog::setTopLevelItem(const QString &itemName)
{
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(ui.treeWidget);
    topLevelItem->setText(0, itemName);
    ui.treeWidget->setItemExpanded(topLevelItem, true);
    topLevelItem->setIcon(0, style()->standardPixmap(QStyle::SP_DirOpenIcon));

    QFont boldFont = topLevelItem->font(0);
    boldFont.setBold(true);
    topLevelItem->setFont(0, boldFont);

    return topLevelItem;
}

QTreeWidgetItem* PluginDialog::setPluginItem(QTreeWidgetItem *topLevelItem,
                                             const QString &itemName, const QFont &font)
{
    QTreeWidgetItem *pluginItem = new QTreeWidgetItem(topLevelItem);
    pluginItem->setFont(0, font);
    pluginItem->setText(0, itemName);
    ui.treeWidget->setItemExpanded(pluginItem, true);
    pluginItem->setIcon(0, style()->standardPixmap(QStyle::SP_DirOpenIcon));

    return pluginItem;
}

void PluginDialog::setItem(QTreeWidgetItem *pluginItem, const QString &name,
                           const QString &toolTip, const QString &whatsThis, const QIcon &icon)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(pluginItem);
    item->setText(0, name);
    item->setToolTip(0, toolTip);
    item->setWhatsThis(0, whatsThis);
    item->setIcon(0, pluginIcon(icon));
}

void  PluginDialog::updateCustomWidgetPlugins()
{
    if (qdesigner_internal::QDesignerIntegration *integration = qobject_cast<qdesigner_internal::QDesignerIntegration *>(m_core->integration())) {
        const int before = m_core->widgetDataBase()->count();
        integration->updateCustomWidgetPlugins();
        const int after = m_core->widgetDataBase()->count();
        if (after >  before) {
            ui.message->setText(tr("New custom widget plugins have been found."));
            ui.message->show();
        } else {
            ui.message->setText(QString());
        }
        populateTreeWidget();
    }
}

}

QT_END_NAMESPACE
