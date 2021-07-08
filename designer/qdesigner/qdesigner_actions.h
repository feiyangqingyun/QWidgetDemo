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

#ifndef QDESIGNER_ACTIONS_H
#define QDESIGNER_ACTIONS_H

#include "assistantclient.h"
#include "qdesigner_settings.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QPrinter>

QT_BEGIN_NAMESPACE

class QDesignerWorkbench;

class QDir;
class QTimer;
class QAction;
class QActionGroup;
class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class AppFontDialog;

class QRect;
class QWidget;
class QPixmap;
class QMenu;

namespace qdesigner_internal {
    class PreviewConfiguration;
    class PreviewManager;
}

class QDesignerActions: public QObject
{
    Q_OBJECT
public:
    explicit QDesignerActions(QDesignerWorkbench *mainWindow);
    virtual ~QDesignerActions();

    QDesignerWorkbench *workbench() const;
    QDesignerFormEditorInterface *core() const;

    bool saveForm(QDesignerFormWindowInterface *fw);
    bool readInForm(const QString &fileName);
    bool writeOutForm(QDesignerFormWindowInterface *formWindow, const QString &fileName);

    QActionGroup *fileActions() const;
    QActionGroup *recentFilesActions() const;
    QActionGroup *editActions() const;
    QActionGroup *formActions() const;
    QActionGroup *settingsActions() const;
    QActionGroup *windowActions() const;
    QActionGroup *toolActions() const;
    QActionGroup *helpActions() const;
    QActionGroup *uiMode() const;
    QActionGroup *styleActions() const;
    // file actions
    QAction *openFormAction() const;
    QAction *closeFormAction() const;
    // window actions
    QAction *minimizeAction() const;
    // edit mode actions
    QAction *editWidgets() const;
    // form actions
    QAction *previewFormAction() const;
    QAction *viewCodeAction() const;

    void setBringAllToFrontVisible(bool visible);
    void setWindowListSeparatorVisible(bool visible);

    bool openForm(QWidget *parent);

    QString uiExtension() const;

    // Boolean dynamic property set on actions to
    // show them in the default toolbar layout
    static const char *defaultToolbarPropertyName;

public slots:
    void activeFormWindowChanged(QDesignerFormWindowInterface *formWindow);
    void createForm();
    void slotOpenForm();
    void helpRequested(const QString &manual, const QString &document);

signals:
    void useBigIcons(bool);

private slots:
    void saveForm();
    void saveFormAs();
    void saveAllForms();
    void saveFormAsTemplate();
    void viewCode();
    void notImplementedYet();
    void shutdown();
    void editWidgetsSlot();
    void openRecentForm();
    void clearRecentFiles();
    void closeForm();
    void showDesignerHelp();
    void showWhatsNew();
    void aboutDesigner();
    void showWidgetSpecificHelp();
    void backupForms();
    void showNewFormDialog(const QString &fileName);
    void showPreferencesDialog();
    void showAppFontDialog();
    void savePreviewImage();
    void printPreviewImage();
    void updateCloseAction();
    void formWindowCountChanged();
    void formWindowSettingsChanged(QDesignerFormWindowInterface *fw);

private:
    QAction *createRecentFilesMenu();
    bool saveFormAs(QDesignerFormWindowInterface *fw);
    void fixActionContext();
    void updateRecentFileActions();
    void addRecentFile(const QString &fileName);
    void showHelp(const QString &help);
    void closePreview();
    QRect fixDialogRect(const QRect &rect) const;
    QString fixResourceFileBackupPath(QDesignerFormWindowInterface *fwi, const QDir& backupDir);
    void showStatusBarMessage(const QString &message) const;
    QActionGroup *createHelpActions();
    bool ensureBackupDirectories();
    QPixmap createPreviewPixmap(QDesignerFormWindowInterface *fw);
    qdesigner_internal::PreviewConfiguration previewConfiguration();

    enum { MaxRecentFiles = 10 };
    QDesignerWorkbench *m_workbench;
    QDesignerFormEditorInterface *m_core;
    QDesignerSettings m_settings;
    AssistantClient m_assistantClient;
    QString m_openDirectory;
    QString m_saveDirectory;


    QString m_backupPath;
    QString m_backupTmpPath;

    QTimer* m_backupTimer;

    QActionGroup *m_fileActions;
    QActionGroup *m_recentFilesActions;
    QActionGroup *m_editActions;
    QActionGroup *m_formActions;
    QActionGroup *m_settingsActions;
    QActionGroup *m_windowActions;
    QActionGroup *m_toolActions;
    QActionGroup *m_helpActions;
    QActionGroup *m_styleActions;

    QAction *m_editWidgetsAction;

    QAction *m_newFormAction;
    QAction *m_openFormAction;
    QAction *m_saveFormAction;
    QAction *m_saveFormAsAction;
    QAction *m_saveAllFormsAction;
    QAction *m_saveFormAsTemplateAction;
    QAction *m_closeFormAction;
    QAction *m_savePreviewImageAction;
    QAction *m_printPreviewAction;

    QAction *m_quitAction;

    QAction *m_previewFormAction;
    QAction *m_viewCodeAction;

    QAction *m_minimizeAction;
    QAction *m_bringAllToFrontSeparator;
    QAction *m_bringAllToFrontAction;
    QAction *m_windowListSeparatorAction;

    QAction *m_preferencesAction;
    QAction *m_appFontAction;

    QPointer<AppFontDialog> m_appFontDialog;

#ifndef QT_NO_PRINTER
    QPrinter *m_printer;
#endif

    qdesigner_internal::PreviewManager *m_previewManager;
};

QT_END_NAMESPACE

#endif // QDESIGNER_ACTIONS_H
