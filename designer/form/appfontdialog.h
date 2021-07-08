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

#ifndef QDESIGNER_APPFONTWIDGET_H
#define QDESIGNER_APPFONTWIDGET_H

#include <QtGui/QGroupBox>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class AppFontModel;

class QTreeView;
class QToolButton;
class QItemSelection;
class QDesignerSettingsInterface;

// AppFontWidget: Manages application fonts which the user can load and 
// provides API for saving/restoring them.

class AppFontWidget : public QGroupBox
{
    Q_DISABLE_COPY(AppFontWidget)
    Q_OBJECT
public:
    explicit AppFontWidget(QWidget *parent = 0);

    QStringList fontFiles() const;

    static void save(QDesignerSettingsInterface *s, const QString &prefix);
    static void restore(const QDesignerSettingsInterface *s, const QString &prefix);

private slots:
    void addFiles();
    void slotRemoveFiles();
    void slotRemoveAll();
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

private:
    QTreeView *m_view;
    QToolButton *m_addButton;
    QToolButton *m_removeButton;
    QToolButton *m_removeAllButton;
    AppFontModel *m_model;
};

// AppFontDialog: Non modal dialog for AppFontWidget which has Qt::WA_DeleteOnClose set.

class AppFontDialog : public QDialog
{
    Q_DISABLE_COPY(AppFontDialog)
    Q_OBJECT
public:
    explicit AppFontDialog(QWidget *parent = 0);

private:
    AppFontWidget *m_appFontWidget;
};

QT_END_NAMESPACE

#endif // QDESIGNER_APPFONTWIDGET_H
