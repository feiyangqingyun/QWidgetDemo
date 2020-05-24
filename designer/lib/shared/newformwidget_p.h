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

#ifndef NEWFORMWIDGET_H
#define NEWFORMWIDGET_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "shared_global_p.h"
#include "deviceprofile_p.h"

#include <abstractnewformwidget_p.h>

#include <QtGui/QWidget>
#include <QtGui/QPixmap>

#include <QtCore/QStringList>
#include <QtCore/QPair>
#include <QtCore/QMap>

QT_BEGIN_NAMESPACE

class QIODevice;
class QTreeWidgetItem;

namespace qdesigner_internal {

namespace Ui {
    class NewFormWidget;
}

class QDesignerWorkbench;

class QDESIGNER_SHARED_EXPORT NewFormWidget : public QDesignerNewFormWidgetInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(NewFormWidget)

public:
    typedef QList<qdesigner_internal::DeviceProfile> DeviceProfileList;

    explicit NewFormWidget(QDesignerFormEditorInterface *core, QWidget *parentWidget);
    virtual ~NewFormWidget();

    virtual bool hasCurrentTemplate() const;
    virtual QString currentTemplate(QString *errorMessage = 0);

    // Convenience for implementing file dialogs with preview
    static QImage grabForm(QDesignerFormEditorInterface *core,
                           QIODevice &file,
                           const QString &workingDir,
                           const qdesigner_internal::DeviceProfile &dp);

private slots:
    void on_treeWidget_itemActivated(QTreeWidgetItem *item);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
    void on_treeWidget_itemPressed(QTreeWidgetItem *item);
    void slotDeviceProfileIndexChanged(int idx);

private:
    QPixmap formPreviewPixmap(const QString &fileName) const;
    QPixmap formPreviewPixmap(QIODevice &file, const QString &workingDir = QString()) const;
    QPixmap formPreviewPixmap(const QTreeWidgetItem *item);

    void loadFrom(const QString &path, bool resourceFile, const QString &uiExtension,
                  const QString &selectedItem, QTreeWidgetItem *&selectedItemFound);
    void loadFrom(const QString &title, const QStringList &nameList,
                  const QString &selectedItem, QTreeWidgetItem *&selectedItemFound);

private:
    QString itemToTemplate(const QTreeWidgetItem *item, QString *errorMessage) const;
    QString currentTemplateI(QString *ptrToErrorMessage);

    QSize templateSize() const;
    void setTemplateSize(const QSize &s);
    int profileComboIndex() const;
    qdesigner_internal::DeviceProfile currentDeviceProfile() const;
    bool showCurrentItemPixmap();

    // Pixmap cache (item, profile combo index)
    typedef QPair<const QTreeWidgetItem *, int> ItemPixmapCacheKey;
    typedef QMap<ItemPixmapCacheKey, QPixmap> ItemPixmapCache;
    ItemPixmapCache m_itemPixmapCache;

    QDesignerFormEditorInterface *m_core;
    Ui::NewFormWidget *m_ui;
    QTreeWidgetItem *m_currentItem;
    QTreeWidgetItem *m_acceptedItem;
    DeviceProfileList m_deviceProfiles;
};

}

QT_END_NAMESPACE

#endif // NEWFORMWIDGET_H
