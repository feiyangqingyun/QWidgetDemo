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

#ifndef QTRESOURCEVIEW_H
#define QTRESOURCEVIEW_H

#include "shared_global_p.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QtResourceModel;
class QtResourceSet;
class QDesignerFormEditorInterface;
class QMimeData;

class QDESIGNER_SHARED_EXPORT QtResourceView : public QWidget
{
    Q_OBJECT
public:
    explicit QtResourceView(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    ~QtResourceView();

    void setDragEnabled(bool dragEnabled);
    bool dragEnabled() const;

    QtResourceModel *model() const;
    void setResourceModel(QtResourceModel *model);

    QString selectedResource() const;
    void selectResource(const QString &resource);

    QString settingsKey() const;
    void setSettingsKey(const QString &key);

    bool isResourceEditingEnabled() const;
    void setResourceEditingEnabled(bool enable);

    // Helpers for handling the drag originating in QtResourceView (Xml/text)
    enum ResourceType { ResourceImage, ResourceStyleSheet, ResourceOther };
    static QString encodeMimeData(ResourceType resourceType, const QString &path);

    static bool decodeMimeData(const QMimeData *md, ResourceType *t = 0, QString *file = 0);
    static bool decodeMimeData(const QString &text, ResourceType *t = 0, QString *file = 0);

signals:
    void resourceSelected(const QString &resource);
    void resourceActivated(const QString &resource);

protected:
    bool event(QEvent *event);

private:

    QScopedPointer<class QtResourceViewPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtResourceView)
    Q_DISABLE_COPY(QtResourceView)
    Q_PRIVATE_SLOT(d_func(), void slotResourceSetActivated(QtResourceSet *))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentPathChanged(QTreeWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentResourceChanged(QListWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void slotResourceActivated(QListWidgetItem *))
    Q_PRIVATE_SLOT(d_func(), void slotEditResources())
    Q_PRIVATE_SLOT(d_func(), void slotReloadResources())
    Q_PRIVATE_SLOT(d_func(), void slotCopyResourcePath())
    Q_PRIVATE_SLOT(d_func(), void slotListWidgetContextMenuRequested(const QPoint &pos))
    Q_PRIVATE_SLOT(d_func(), void slotFilterChanged(const QString &pattern))
};

class QDESIGNER_SHARED_EXPORT  QtResourceViewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QtResourceViewDialog(QDesignerFormEditorInterface *core, QWidget *parent = 0);
    virtual ~QtResourceViewDialog();

    QString selectedResource() const;
    void selectResource(const QString &path);

    bool isResourceEditingEnabled() const;
    void setResourceEditingEnabled(bool enable);

private:
    QScopedPointer<class QtResourceViewDialogPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtResourceViewDialog)
    Q_DISABLE_COPY(QtResourceViewDialog)
    Q_PRIVATE_SLOT(d_func(), void slotResourceSelected(const QString &))
};

QT_END_NAMESPACE

#endif
