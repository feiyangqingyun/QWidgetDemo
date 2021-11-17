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

#include "qdesigner_actions.h"
#include "designer_enums.h"
#include "qdesigner.h"
#include "qdesigner_workbench.h"
#include "qdesigner_formwindow.h"
#include "newform.h"
#include "versiondialog.h"
#include "saveformastemplate.h"
#include "qdesigner_toolwindow.h"
#include "preferencesdialog.h"
#include "appfontdialog.h"

#include <pluginmanager_p.h>
#include <qdesigner_formbuilder_p.h>
#include <qdesigner_utils_p.h>
#include <iconloader_p.h>
#include <qsimpleresource_p.h>
#include <previewmanager_p.h>
#include <codedialog_p.h>
#include <qdesigner_formwindowmanager_p.h>
#include "qdesigner_integration_p.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormEditorPluginInterface>
#include <QtDesigner/QExtensionManager>

#include <QtDesigner/private/shared_settings_p.h>
#include <QtDesigner/private/formwindowbase_p.h>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QStyleFactory>
#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtGui/QTransform>
#include <QtGui/QCursor>
#include <QtCore/QSizeF>

#include <QtCore/QLibraryInfo>
#include <QtCore/QBuffer>
#include <QtCore/QPluginLoader>
#include <QtCore/qdebug.h>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QFileInfo>
#include <QtGui/QStatusBar>
#include <QtGui/QDesktopWidget>
#include <QtXml/QDomDocument>

QT_BEGIN_NAMESPACE

using namespace qdesigner_internal;

const char *QDesignerActions::defaultToolbarPropertyName = "__qt_defaultToolBarAction";

//#ifdef Q_WS_MAC
#  define NONMODAL_PREVIEW
//#endif

static QAction *createSeparator(QObject *parent) {
    QAction * rc = new QAction(parent);
    rc->setSeparator(true);
    return rc;
}

static QActionGroup *createActionGroup(QObject *parent, bool exclusive = false) {
    QActionGroup * rc = new QActionGroup(parent);
    rc->setExclusive(exclusive);
    return rc;
}

static inline QString savedMessage(const QString &fileName)
{
    return QDesignerActions::tr("Saved %1.").arg(fileName);
}

// Prompt for a file and make sure an extension is added
// unless the user explicitly specifies another one.

static QString getSaveFileNameWithExtension(QWidget *parent, const QString &title, QString dir, const QString &filter, const QString &extension)
{
    const QChar dot = QLatin1Char('.');

    QString saveFile;
    while (true) {
        saveFile = QFileDialog::getSaveFileName(parent, title, dir, filter, 0, QFileDialog::DontConfirmOverwrite);
        if (saveFile.isEmpty())
            return saveFile;

        const QFileInfo fInfo(saveFile);
        if (fInfo.suffix().isEmpty() && !fInfo.fileName().endsWith(dot)) {
            saveFile += dot;
            saveFile += extension;
        }

        const QFileInfo fi(saveFile);
        if (!fi.exists())
            break;

        const QString prompt = QDesignerActions::tr("%1 already exists.\nDo you want to replace it?").arg(fi.fileName());
        if (QMessageBox::warning(parent, title, prompt, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            break;

        dir = saveFile;
    }
    return saveFile;
}

QDesignerActions::QDesignerActions(QDesignerWorkbench *workbench)
    : QObject(workbench),
      m_workbench(workbench),
      m_core(workbench->core()),
      m_settings(workbench->core()),
      m_backupTimer(new QTimer(this)),
      m_fileActions(createActionGroup(this)),
      m_recentFilesActions(createActionGroup(this)),
      m_editActions(createActionGroup(this)),
      m_formActions(createActionGroup(this)),
      m_settingsActions(createActionGroup(this)),
      m_windowActions(createActionGroup(this)),
      m_toolActions(createActionGroup(this, true)),
      m_helpActions(0),
      m_styleActions(0),
      m_editWidgetsAction(new QAction(tr("Edit Widgets"), this)),
      m_newFormAction(new QAction(qdesigner_internal::createIconSet(QLatin1String("filenew.png")), tr("&New..."), this)),
      m_openFormAction(new QAction(qdesigner_internal::createIconSet(QLatin1String("fileopen.png")), tr("&Open..."), this)),
      m_saveFormAction(new QAction(qdesigner_internal::createIconSet(QLatin1String("filesave.png")), tr("&Save"), this)),
      m_saveFormAsAction(new QAction(tr("Save &As..."), this)),
      m_saveAllFormsAction(new QAction(tr("Save A&ll"), this)),
      m_saveFormAsTemplateAction(new QAction(tr("Save As &Template..."), this)),
      m_closeFormAction(new QAction(tr("&Close"), this)),
      m_savePreviewImageAction(new QAction(tr("Save &Image..."), this)),
      m_printPreviewAction(new QAction(tr("&Print..."), this)),
      m_quitAction(new QAction(tr("&Quit"), this)),
      m_previewFormAction(0),
      m_viewCodeAction(new QAction(tr("View &Code..."), this)),
      m_minimizeAction(new QAction(tr("&Minimize"), this)),
      m_bringAllToFrontSeparator(createSeparator(this)),
      m_bringAllToFrontAction(new QAction(tr("Bring All to Front"), this)),
      m_windowListSeparatorAction(createSeparator(this)),
      m_preferencesAction(new QAction(tr("Preferences..."), this)),
      m_appFontAction(new QAction(tr("Additional Fonts..."), this)),
      m_appFontDialog(0),
#ifndef QT_NO_PRINTER
      m_printer(0),
#endif
      m_previewManager(0)
{
#ifdef Q_WS_X11
    m_newFormAction->setIcon(QIcon::fromTheme("document-new", m_newFormAction->icon()));
    m_openFormAction->setIcon(QIcon::fromTheme("document-open", m_openFormAction->icon()));
    m_saveFormAction->setIcon(QIcon::fromTheme("document-save", m_saveFormAction->icon()));
    m_saveFormAsAction->setIcon(QIcon::fromTheme("document-save-as", m_saveFormAsAction->icon()));
    m_printPreviewAction->setIcon(QIcon::fromTheme("document-print", m_printPreviewAction->icon()));
    m_closeFormAction->setIcon(QIcon::fromTheme("window-close", m_closeFormAction->icon()));
    m_quitAction->setIcon(QIcon::fromTheme("application-exit", m_quitAction->icon()));
#endif

    Q_ASSERT(m_core != 0);
    qdesigner_internal::QDesignerFormWindowManager *ifwm = qobject_cast<qdesigner_internal::QDesignerFormWindowManager *>(m_core->formWindowManager());
    Q_ASSERT(ifwm);
    m_previewManager = ifwm->previewManager();
    m_previewFormAction = ifwm->actionDefaultPreview();
    m_styleActions = ifwm->actionGroupPreviewInStyle();
    connect(ifwm, SIGNAL(formWindowSettingsChanged(QDesignerFormWindowInterface*)),
            this, SLOT(formWindowSettingsChanged(QDesignerFormWindowInterface*)));

    m_editWidgetsAction->setObjectName(QLatin1String("__qt_edit_widgets_action"));
    m_newFormAction->setObjectName(QLatin1String("__qt_new_form_action"));
    m_openFormAction->setObjectName(QLatin1String("__qt_open_form_action"));
    m_saveFormAction->setObjectName(QLatin1String("__qt_save_form_action"));
    m_saveFormAsAction->setObjectName(QLatin1String("__qt_save_form_as_action"));
    m_saveAllFormsAction->setObjectName(QLatin1String("__qt_save_all_forms_action"));
    m_saveFormAsTemplateAction->setObjectName(QLatin1String("__qt_save_form_as_template_action"));
    m_closeFormAction->setObjectName(QLatin1String("__qt_close_form_action"));
    m_quitAction->setObjectName(QLatin1String("__qt_quit_action"));
    m_previewFormAction->setObjectName(QLatin1String("__qt_preview_form_action"));
    m_viewCodeAction->setObjectName(QLatin1String("__qt_preview_code_action"));
    m_minimizeAction->setObjectName(QLatin1String("__qt_minimize_action"));
    m_bringAllToFrontAction->setObjectName(QLatin1String("__qt_bring_all_to_front_action"));
    m_preferencesAction->setObjectName(QLatin1String("__qt_preferences_action"));

    m_helpActions = createHelpActions();

    m_newFormAction->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    m_openFormAction->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    m_saveFormAction->setProperty(QDesignerActions::defaultToolbarPropertyName, true);

    QDesignerFormWindowManagerInterface *formWindowManager = m_core->formWindowManager();
    Q_ASSERT(formWindowManager != 0);

//
// file actions
//
    m_newFormAction->setShortcut(QKeySequence::New);
    connect(m_newFormAction, SIGNAL(triggered()), this, SLOT(createForm()));
    m_fileActions->addAction(m_newFormAction);

    m_openFormAction->setShortcut(QKeySequence::Open);
    connect(m_openFormAction, SIGNAL(triggered()), this, SLOT(slotOpenForm()));
    m_fileActions->addAction(m_openFormAction);

    m_fileActions->addAction(createRecentFilesMenu());
    m_fileActions->addAction(createSeparator(this));

    m_saveFormAction->setShortcut(QKeySequence::Save);
    connect(m_saveFormAction, SIGNAL(triggered()), this, SLOT(saveForm()));
    m_fileActions->addAction(m_saveFormAction);

    connect(m_saveFormAsAction, SIGNAL(triggered()), this, SLOT(saveFormAs()));
    m_fileActions->addAction(m_saveFormAsAction);

#ifdef Q_OS_MAC
    m_saveAllFormsAction->setShortcut(tr("ALT+CTRL+S"));
#else
    m_saveAllFormsAction->setShortcut(tr("CTRL+SHIFT+S")); // Commonly "Save As" on Mac
#endif
    connect(m_saveAllFormsAction, SIGNAL(triggered()), this, SLOT(saveAllForms()));
    m_fileActions->addAction(m_saveAllFormsAction);

    connect(m_saveFormAsTemplateAction, SIGNAL(triggered()), this, SLOT(saveFormAsTemplate()));
    m_fileActions->addAction(m_saveFormAsTemplateAction);

    m_fileActions->addAction(createSeparator(this));

    m_printPreviewAction->setShortcut(QKeySequence::Print);
    connect(m_printPreviewAction,  SIGNAL(triggered()), this, SLOT(printPreviewImage()));
    m_fileActions->addAction(m_printPreviewAction);
    m_printPreviewAction->setObjectName(QLatin1String("__qt_print_action"));

    connect(m_savePreviewImageAction,  SIGNAL(triggered()), this, SLOT(savePreviewImage()));
    m_savePreviewImageAction->setObjectName(QLatin1String("__qt_saveimage_action"));
    m_fileActions->addAction(m_savePreviewImageAction);
    m_fileActions->addAction(createSeparator(this));

    m_closeFormAction->setShortcut(QKeySequence::Close);
    connect(m_closeFormAction, SIGNAL(triggered()), this, SLOT(closeForm()));
    m_fileActions->addAction(m_closeFormAction);
    updateCloseAction();

    m_fileActions->addAction(createSeparator(this));

    m_quitAction->setShortcuts(QKeySequence::Quit);
    m_quitAction->setMenuRole(QAction::QuitRole);
    connect(m_quitAction, SIGNAL(triggered()), this, SLOT(shutdown()));
    m_fileActions->addAction(m_quitAction);

//
// edit actions
//
    QAction *undoAction = formWindowManager->actionUndo();
    undoAction->setObjectName(QLatin1String("__qt_undo_action"));
    undoAction->setShortcut(QKeySequence::Undo);
    m_editActions->addAction(undoAction);

    QAction *redoAction = formWindowManager->actionRedo();
    redoAction->setObjectName(QLatin1String("__qt_redo_action"));
    redoAction->setShortcut(QKeySequence::Redo);
    m_editActions->addAction(redoAction);

    m_editActions->addAction(createSeparator(this));

    m_editActions->addAction(formWindowManager->actionCut());
    m_editActions->addAction(formWindowManager->actionCopy());
    m_editActions->addAction(formWindowManager->actionPaste());
    m_editActions->addAction(formWindowManager->actionDelete());

    m_editActions->addAction(formWindowManager->actionSelectAll());

    m_editActions->addAction(createSeparator(this));

    m_editActions->addAction(formWindowManager->actionLower());
    m_editActions->addAction(formWindowManager->actionRaise());

    formWindowManager->actionLower()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionRaise()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);

//
// edit mode actions
//

    m_editWidgetsAction->setCheckable(true);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::Key_F3));
#if QT_VERSION >= 0x040900 // "ESC" switching to edit mode: Activate once item delegates handle shortcut overrides for ESC.
    shortcuts.append(QKeySequence(Qt::Key_Escape));
#endif
    m_editWidgetsAction->setShortcuts(shortcuts);
    QIcon fallback(m_core->resourceLocation() + QLatin1String("/widgettool.png"));
    m_editWidgetsAction->setIcon(QIcon::fromTheme("designer-edit-widget", fallback));
    connect(m_editWidgetsAction, SIGNAL(triggered()), this, SLOT(editWidgetsSlot()));
    m_editWidgetsAction->setChecked(true);
    m_editWidgetsAction->setEnabled(false);
    m_editWidgetsAction->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    m_toolActions->addAction(m_editWidgetsAction);

    connect(formWindowManager, SIGNAL(activeFormWindowChanged(QDesignerFormWindowInterface*)),
                this, SLOT(activeFormWindowChanged(QDesignerFormWindowInterface*)));

    QList<QObject*> builtinPlugins = QPluginLoader::staticInstances();
    builtinPlugins += m_core->pluginManager()->instances();
    foreach (QObject *plugin, builtinPlugins) {
        if (QDesignerFormEditorPluginInterface *formEditorPlugin = qobject_cast<QDesignerFormEditorPluginInterface*>(plugin)) {
            if (QAction *action = formEditorPlugin->action()) {
                m_toolActions->addAction(action);
                action->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
                action->setCheckable(true);
            }
        }
    }

    connect(m_preferencesAction, SIGNAL(triggered()),  this, SLOT(showPreferencesDialog()));
    m_preferencesAction->setMenuRole(QAction::PreferencesRole);
    m_settingsActions->addAction(m_preferencesAction);

    connect(m_appFontAction, SIGNAL(triggered()),  this, SLOT(showAppFontDialog()));
    m_appFontAction->setMenuRole(QAction::PreferencesRole);
    m_settingsActions->addAction(m_appFontAction);
//
// form actions
//

    m_formActions->addAction(formWindowManager->actionHorizontalLayout());
    m_formActions->addAction(formWindowManager->actionVerticalLayout());
    m_formActions->addAction(formWindowManager->actionSplitHorizontal());
    m_formActions->addAction(formWindowManager->actionSplitVertical());
    m_formActions->addAction(formWindowManager->actionGridLayout());
    m_formActions->addAction(formWindowManager->actionFormLayout());
    m_formActions->addAction(formWindowManager->actionBreakLayout());
    m_formActions->addAction(formWindowManager->actionAdjustSize());
    m_formActions->addAction(formWindowManager->actionSimplifyLayout());
    m_formActions->addAction(createSeparator(this));

    formWindowManager->actionHorizontalLayout()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionVerticalLayout()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionSplitHorizontal()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionSplitVertical()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionGridLayout()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionFormLayout()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionBreakLayout()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);
    formWindowManager->actionAdjustSize()->setProperty(QDesignerActions::defaultToolbarPropertyName, true);

    m_previewFormAction->setShortcut(tr("CTRL+R"));
    m_formActions->addAction(m_previewFormAction);
    connect(m_previewManager, SIGNAL(firstPreviewOpened()), this, SLOT(updateCloseAction()));
    connect(m_previewManager, SIGNAL(lastPreviewClosed()), this, SLOT(updateCloseAction()));

    connect(m_viewCodeAction, SIGNAL(triggered()), this, SLOT(viewCode()));
    // Preview code only in Cpp
    if (qt_extension<QDesignerLanguageExtension *>(m_core->extensionManager(), m_core) == 0)
        m_formActions->addAction(m_viewCodeAction);

    m_formActions->addAction(createSeparator(this));

    m_formActions->addAction(ifwm->actionShowFormWindowSettingsDialog());
//
// window actions
//
    m_minimizeAction->setEnabled(false);
    m_minimizeAction->setCheckable(true);
    m_minimizeAction->setShortcut(tr("CTRL+M"));
    connect(m_minimizeAction, SIGNAL(triggered()), m_workbench, SLOT(toggleFormMinimizationState()));
    m_windowActions->addAction(m_minimizeAction);

    m_windowActions->addAction(m_bringAllToFrontSeparator);
    connect(m_bringAllToFrontAction, SIGNAL(triggered()), m_workbench, SLOT(bringAllToFront()));
    m_windowActions->addAction(m_bringAllToFrontAction);
    m_windowActions->addAction(m_windowListSeparatorAction);

    setWindowListSeparatorVisible(false);

//
// connections
//
    fixActionContext();
    activeFormWindowChanged(core()->formWindowManager()->activeFormWindow());

    m_backupTimer->start(180000); // 3min
    connect(m_backupTimer, SIGNAL(timeout()), this, SLOT(backupForms()));

    // Enable application font action
    connect(formWindowManager, SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), this, SLOT(formWindowCountChanged()));
    connect(formWindowManager, SIGNAL(formWindowRemoved(QDesignerFormWindowInterface*)), this, SLOT(formWindowCountChanged()));
    formWindowCountChanged();
}

QActionGroup *QDesignerActions::createHelpActions()
{
    QActionGroup *helpActions = createActionGroup(this);

#ifndef QT_JAMBI_BUILD
    QAction *mainHelpAction = new QAction(tr("Qt Designer &Help"), this);
    mainHelpAction->setObjectName(QLatin1String("__qt_designer_help_action"));
    connect(mainHelpAction, SIGNAL(triggered()), this, SLOT(showDesignerHelp()));
    mainHelpAction->setShortcut(Qt::CTRL + Qt::Key_Question);
    helpActions->addAction(mainHelpAction);

    helpActions->addAction(createSeparator(this));
    QAction *widgetHelp = new QAction(tr("Current Widget Help"), this);
    widgetHelp->setObjectName(QLatin1String("__qt_current_widget_help_action"));
    widgetHelp->setShortcut(Qt::Key_F1);
    connect(widgetHelp, SIGNAL(triggered()), this, SLOT(showWidgetSpecificHelp()));
    helpActions->addAction(widgetHelp);

    helpActions->addAction(createSeparator(this));
    QAction *whatsNewAction = new QAction(tr("What's New in Qt Designer?"), this);
    whatsNewAction->setObjectName(QLatin1String("__qt_whats_new_in_qt_designer_action"));
    connect(whatsNewAction, SIGNAL(triggered()), this, SLOT(showWhatsNew()));
    helpActions->addAction(whatsNewAction);
#endif

    helpActions->addAction(createSeparator(this));
    QAction *aboutPluginsAction = new QAction(tr("About Plugins"), this);
    aboutPluginsAction->setObjectName(QLatin1String("__qt_about_plugins_action"));
    aboutPluginsAction->setMenuRole(QAction::ApplicationSpecificRole);
    connect(aboutPluginsAction, SIGNAL(triggered()), m_core->formWindowManager(), SLOT(aboutPlugins()));
    helpActions->addAction(aboutPluginsAction);

    QAction *aboutDesignerAction = new QAction(tr("About Qt Designer"), this);
    aboutDesignerAction->setMenuRole(QAction::AboutRole);
    aboutDesignerAction->setObjectName(QLatin1String("__qt_about_designer_action"));
    connect(aboutDesignerAction, SIGNAL(triggered()), this, SLOT(aboutDesigner()));
    helpActions->addAction(aboutDesignerAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"), this);
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    aboutQtAction->setObjectName(QLatin1String("__qt_about_qt_action"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    helpActions->addAction(aboutQtAction);
    return helpActions;
}

QDesignerActions::~QDesignerActions()
{
#ifndef QT_NO_PRINTER
    delete m_printer;
#endif
}

QString QDesignerActions::uiExtension() const
{
    QDesignerLanguageExtension *lang
        = qt_extension<QDesignerLanguageExtension *>(m_core->extensionManager(), m_core);
    if (lang)
        return lang->uiExtension();
    return QLatin1String("ui");
}

QAction *QDesignerActions::createRecentFilesMenu()
{
    QMenu *menu = new QMenu;
    QAction *act;
    // Need to insert this into the QAction.
    for (int i = 0; i < MaxRecentFiles; ++i) {
        act = new QAction(this);
        act->setVisible(false);
        connect(act, SIGNAL(triggered()), this, SLOT(openRecentForm()));
        m_recentFilesActions->addAction(act);
        menu->addAction(act);
    }
    updateRecentFileActions();
    menu->addSeparator();
    act = new QAction(QIcon::fromTheme("edit-clear"), tr("Clear &Menu"), this);
    act->setObjectName(QLatin1String("__qt_action_clear_menu_"));
    connect(act, SIGNAL(triggered()), this, SLOT(clearRecentFiles()));
    m_recentFilesActions->addAction(act);
    menu->addAction(act);

    act = new QAction(QIcon::fromTheme("document-open-recent"), tr("&Recent Forms"), this);
    act->setMenu(menu);
    return act;
}

QActionGroup *QDesignerActions::toolActions() const
{ return m_toolActions; }

QDesignerWorkbench *QDesignerActions::workbench() const
{ return m_workbench; }

QDesignerFormEditorInterface *QDesignerActions::core() const
{ return m_core; }

QActionGroup *QDesignerActions::fileActions() const
{ return m_fileActions; }

QActionGroup *QDesignerActions::editActions() const
{ return m_editActions; }

QActionGroup *QDesignerActions::formActions() const
{ return m_formActions; }

QActionGroup *QDesignerActions::settingsActions() const
{  return m_settingsActions; }

QActionGroup *QDesignerActions::windowActions() const
{ return m_windowActions; }

QActionGroup *QDesignerActions::helpActions() const
{ return m_helpActions; }

QActionGroup *QDesignerActions::styleActions() const
{ return m_styleActions; }

QAction *QDesignerActions::previewFormAction() const
{ return m_previewFormAction; }

QAction *QDesignerActions::viewCodeAction() const
{ return m_viewCodeAction; }


void QDesignerActions::editWidgetsSlot()
{
    QDesignerFormWindowManagerInterface *formWindowManager = core()->formWindowManager();
    for (int i=0; i<formWindowManager->formWindowCount(); ++i) {
        QDesignerFormWindowInterface *formWindow = formWindowManager->formWindow(i);
        formWindow->editWidgets();
    }
}

void QDesignerActions::createForm()
{    
    showNewFormDialog(QString());
}

void QDesignerActions::showNewFormDialog(const QString &fileName)
{
    closePreview();
    NewForm *dlg = new NewForm(workbench(), workbench()->core()->topLevel(), fileName);

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setAttribute(Qt::WA_ShowModal);

    dlg->setGeometry(fixDialogRect(dlg->rect()));
    dlg->exec();
}

void QDesignerActions::slotOpenForm()
{
    openForm(core()->topLevel());
}

bool QDesignerActions::openForm(QWidget *parent)
{
    closePreview();
    const QString extension = uiExtension();
    const QStringList fileNames = QFileDialog::getOpenFileNames(parent, tr("Open Form"),
        m_openDirectory, tr("Designer UI files (*.%1);;All Files (*)").arg(extension), 0, QFileDialog::DontUseSheet);

    if (fileNames.isEmpty())
        return false;

    bool atLeastOne = false;
    foreach (const QString &fileName, fileNames) {
        if (readInForm(fileName) && !atLeastOne)
            atLeastOne = true;
    }

    return atLeastOne;
}

bool QDesignerActions::saveFormAs(QDesignerFormWindowInterface *fw)
{
    const QString extension = uiExtension();

    QString dir = fw->fileName();
    if (dir.isEmpty()) {
        do {
            // Build untitled name
            if (!m_saveDirectory.isEmpty()) {
                dir = m_saveDirectory;
                break;
            }
            if (!m_openDirectory.isEmpty()) {
                dir = m_openDirectory;
                break;
            }
            dir = QDir::current().absolutePath();
        } while (false);
        dir += QDir::separator();
        dir += QLatin1String("untitled.");
        dir += extension;
    }

    const  QString saveFile = getSaveFileNameWithExtension(fw, tr("Save Form As"), dir, tr("Designer UI files (*.%1);;All Files (*)").arg(extension), extension);
    if (saveFile.isEmpty())
        return false;

    fw->setFileName(saveFile);
    return writeOutForm(fw, saveFile);
}

void QDesignerActions::saveForm()
{
    if (QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow()) {
        if (saveForm(fw))
            showStatusBarMessage(savedMessage(QFileInfo(fw->fileName()).fileName()));
    }
}

void QDesignerActions::saveAllForms()
{
    QString fileNames;
    QDesignerFormWindowManagerInterface *formWindowManager = core()->formWindowManager();
    if (const int totalWindows = formWindowManager->formWindowCount()) {
        const QString separator = QLatin1String(", ");
        for (int i = 0; i < totalWindows; ++i) {
            QDesignerFormWindowInterface *fw = formWindowManager->formWindow(i);
            if (fw && fw->isDirty()) {
                formWindowManager->setActiveFormWindow(fw);
                if (saveForm(fw)) {
                    if (!fileNames.isEmpty())
                        fileNames += separator;
                    fileNames += QFileInfo(fw->fileName()).fileName();
                } else {
                    break;
                }
            }
        }
    }

    if (!fileNames.isEmpty()) {
        showStatusBarMessage(savedMessage(fileNames));
    }
}

bool QDesignerActions::saveForm(QDesignerFormWindowInterface *fw)
{
    bool ret;
    if (fw->fileName().isEmpty())
        ret = saveFormAs(fw);
    else
        ret =  writeOutForm(fw, fw->fileName());
    return ret;
}

void QDesignerActions::closeForm()
{
    if (m_previewManager->previewCount()) {
        closePreview();
        return;
    }

    if (QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow())
        if (QWidget *parent = fw->parentWidget()) {
            if (QMdiSubWindow *mdiSubWindow = qobject_cast<QMdiSubWindow *>(parent->parentWidget())) {
                mdiSubWindow->close();
            } else {
                parent->close();
            }
        }
}

void QDesignerActions::saveFormAs()
{
    if (QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow()) {
        if (saveFormAs(fw))
            showStatusBarMessage(savedMessage(fw->fileName()));
    }
}

void QDesignerActions::saveFormAsTemplate()
{
    if (QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow()) {
        SaveFormAsTemplate dlg(core(), fw, fw->window());
        dlg.exec();
    }
}

void QDesignerActions::notImplementedYet()
{
    QMessageBox::information(core()->topLevel(), tr("Designer"), tr("Feature not implemented yet!"));
}

void QDesignerActions::closePreview()
{
    m_previewManager->closeAllPreviews();
}

void  QDesignerActions::viewCode()
{
    QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow();
    if (!fw)
        return;
    QString errorMessage;
    if (!qdesigner_internal::CodeDialog::showCodeDialog(fw, fw, &errorMessage))
        QMessageBox::warning(fw, tr("Code generation failed"), errorMessage);
}

void QDesignerActions::fixActionContext()
{
    QList<QAction*> actions;
    actions += m_fileActions->actions();
    actions += m_editActions->actions();
    actions += m_toolActions->actions();
    actions += m_formActions->actions();
    actions += m_windowActions->actions();
    actions += m_helpActions->actions();

    foreach (QAction *a, actions) {
        a->setShortcutContext(Qt::ApplicationShortcut);
    }
}

bool QDesignerActions::readInForm(const QString &fileName)
{
    QString fn = fileName;

    // First make sure that we don't have this one open already.
    QDesignerFormWindowManagerInterface *formWindowManager = core()->formWindowManager();
    const int totalWindows = formWindowManager->formWindowCount();
    for (int i = 0; i < totalWindows; ++i) {
        QDesignerFormWindowInterface *w = formWindowManager->formWindow(i);
        if (w->fileName() == fn) {
            w->raise();
            formWindowManager->setActiveFormWindow(w);
            addRecentFile(fn);
            return true;
        }
    }

    // Otherwise load it.
    do {
        QString errorMessage;
        if (workbench()->openForm(fn, &errorMessage)) {
            addRecentFile(fn);
            m_openDirectory = QFileInfo(fn).absolutePath();
            return true;
        } else {
            // prompt to reload
            QMessageBox box(QMessageBox::Warning, tr("Read error"),
                            tr("%1\nDo you want to update the file location or generate a new form?").arg(errorMessage),
                            QMessageBox::Cancel, core()->topLevel());

            QPushButton *updateButton = box.addButton(tr("&Update"), QMessageBox::ActionRole);
            QPushButton *newButton    = box.addButton(tr("&New Form"), QMessageBox::ActionRole);
            box.exec();
            if (box.clickedButton() == box.button(QMessageBox::Cancel))
                return false;

            if (box.clickedButton() == updateButton) {
                const QString extension = uiExtension();
                fn = QFileDialog::getOpenFileName(core()->topLevel(),
                                                  tr("Open Form"), m_openDirectory,
                                                  tr("Designer UI files (*.%1);;All Files (*)").arg(extension), 0, QFileDialog::DontUseSheet);

                if (fn.isEmpty())
                    return false;
            } else if (box.clickedButton() == newButton) {
                // If the file does not exist, but its directory, is valid, open the template with the editor file name set to it.
                // (called from command line).
                QString newFormFileName;
                const  QFileInfo fInfo(fn);
                if (!fInfo.exists()) {
                    // Normalize file name
                    const QString directory = fInfo.absolutePath();
                    if (QDir(directory).exists()) {
                        newFormFileName = directory;
                        newFormFileName  += QLatin1Char('/');
                        newFormFileName  += fInfo.fileName();
                    }
                }
                showNewFormDialog(newFormFileName);
                return false;
            }
        }
    } while (true);
    return true;
}

static QString createBackup(const QString &fileName)
{
    const QString suffix = QLatin1String(".bak");
    QString backupFile = fileName + suffix;
    QFileInfo fi(backupFile);
    int i = 0;
    while (fi.exists()) {
        backupFile = fileName + suffix + QString::number(++i);
        fi.setFile(backupFile);
    }

    if (QFile::copy(fileName, backupFile))
        return backupFile;
    return QString();
}

static void removeBackup(const QString &backupFile)
{
    if (!backupFile.isEmpty())
        QFile::remove(backupFile);
}

bool QDesignerActions::writeOutForm(QDesignerFormWindowInterface *fw, const QString &saveFile)
{
    Q_ASSERT(fw && !saveFile.isEmpty());

    QString backupFile;
    QFileInfo fi(saveFile);
    if (fi.exists())
        backupFile = createBackup(saveFile);

    QString contents = fw->contents();
    if (qdesigner_internal::FormWindowBase *fwb = qobject_cast<qdesigner_internal::FormWindowBase *>(fw)) {
        if (fwb->lineTerminatorMode() == qdesigner_internal::FormWindowBase::CRLFLineTerminator)
            contents.replace(QLatin1Char('\n'), QLatin1String("\r\n"));
    }
    const QByteArray utf8Array = contents.toUtf8();
    m_workbench->updateBackup(fw);

    QFile f(saveFile);
    while (!f.open(QFile::WriteOnly)) {
        QMessageBox box(QMessageBox::Warning,
                        tr("Save Form?"),
                        tr("Could not open file"),
                        QMessageBox::NoButton, fw);

        box.setWindowModality(Qt::WindowModal);
        box.setInformativeText(tr("The file %1 could not be opened."
                               "\nReason: %2"
                               "\nWould you like to retry or select a different file?")
                                .arg(f.fileName()).arg(f.errorString()));
        QPushButton *retryButton = box.addButton(QMessageBox::Retry);
        retryButton->setDefault(true);
        QPushButton *switchButton = box.addButton(tr("Select New File"), QMessageBox::AcceptRole);
        QPushButton *cancelButton = box.addButton(QMessageBox::Cancel);
        box.exec();

        if (box.clickedButton() == cancelButton) {
            removeBackup(backupFile);
            return false;
        } else if (box.clickedButton() == switchButton) {
            QString extension = uiExtension();
            const QString fileName = QFileDialog::getSaveFileName(fw, tr("Save Form As"),
                                                                  QDir::current().absolutePath(),
                                                                  QLatin1String("*.") + extension);
            if (fileName.isEmpty()) {
                removeBackup(backupFile);
                return false;
            }
            if (f.fileName() != fileName) {
                removeBackup(backupFile);
                fi.setFile(fileName);
                backupFile.clear();
                if (fi.exists())
                    backupFile = createBackup(fileName);
            }
            f.setFileName(fileName);
            fw->setFileName(fileName);
        }
        // loop back around...
    }
    while (f.write(utf8Array, utf8Array.size()) != utf8Array.size()) {
        QMessageBox box(QMessageBox::Warning, tr("Save Form?"),
                        tr("Could not write file"),
                        QMessageBox::Retry|QMessageBox::Cancel, fw);
        box.setWindowModality(Qt::WindowModal);
        box.setInformativeText(tr("It was not possible to write the entire file %1 to disk."
                                "\nReason:%2\nWould you like to retry?")
                                .arg(f.fileName()).arg(f.errorString()));
        box.setDefaultButton(QMessageBox::Retry);
        switch (box.exec()) {
        case QMessageBox::Retry:
            f.resize(0);
            break;
        default:
            return false;
        }
    }
    f.close();
    removeBackup(backupFile);
    addRecentFile(saveFile);
    m_saveDirectory = QFileInfo(f).absolutePath();

    fw->setDirty(false);
    fw->parentWidget()->setWindowModified(false);
    return true;
}

void QDesignerActions::shutdown()
{
    // Follow the idea from the Mac, i.e. send the Application a close event
    // and if it's accepted, quit.
    QCloseEvent ev;
    QApplication::sendEvent(qDesigner, &ev);
    if (ev.isAccepted())
        qDesigner->quit();
}

void QDesignerActions::activeFormWindowChanged(QDesignerFormWindowInterface *formWindow)
{
    const bool enable = formWindow != 0;
    m_saveFormAction->setEnabled(enable);
    m_saveFormAsAction->setEnabled(enable);
    m_saveAllFormsAction->setEnabled(enable);
    m_saveFormAsTemplateAction->setEnabled(enable);
    m_closeFormAction->setEnabled(enable);
    m_savePreviewImageAction->setEnabled(enable);
    m_printPreviewAction->setEnabled(enable);

    m_editWidgetsAction->setEnabled(enable);

    m_previewFormAction->setEnabled(enable);
    m_viewCodeAction->setEnabled(enable);
    m_styleActions->setEnabled(enable);
}

void QDesignerActions::formWindowSettingsChanged(QDesignerFormWindowInterface *fw)
{
    if (QDesignerFormWindow *window = m_workbench->findFormWindow(fw))
        window->updateChanged();
}

void QDesignerActions::updateRecentFileActions()
{
    QStringList files = m_settings.recentFilesList();
    const int originalSize = files.size();
    int numRecentFiles = qMin(files.size(), int(MaxRecentFiles));
    const QList<QAction *> recentFilesActs = m_recentFilesActions->actions();

    for (int i = 0; i < numRecentFiles; ++i) {
        const QFileInfo fi(files[i]);
        // If the file doesn't exist anymore, just remove it from the list so
        // people don't get confused.
        if (!fi.exists()) {
            files.removeAt(i);
            --i;
            numRecentFiles = qMin(files.size(), int(MaxRecentFiles));
            continue;
        }
        const QString text = fi.fileName();
        recentFilesActs[i]->setText(text);
        recentFilesActs[i]->setIconText(files[i]);
        recentFilesActs[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFilesActs[j]->setVisible(false);

    // If there's been a change, right it back
    if (originalSize != files.size())
        m_settings.setRecentFilesList(files);
}

void QDesignerActions::openRecentForm()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender())) {
        if (!readInForm(action->iconText()))
            updateRecentFileActions(); // File doesn't exist, remove it from settings
    }
}

void QDesignerActions::clearRecentFiles()
{
    m_settings.setRecentFilesList(QStringList());
    updateRecentFileActions();
}

QActionGroup *QDesignerActions::recentFilesActions() const
{
    return m_recentFilesActions;
}

void QDesignerActions::addRecentFile(const QString &fileName)
{
    QStringList files = m_settings.recentFilesList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    m_settings.setRecentFilesList(files);
    updateRecentFileActions();
}

QAction *QDesignerActions::openFormAction() const
{
    return  m_openFormAction;
}

QAction *QDesignerActions::closeFormAction() const
{
    return m_closeFormAction;
}

QAction *QDesignerActions::minimizeAction() const
{
    return m_minimizeAction;
}

void QDesignerActions::showDesignerHelp()
{
    QString url = AssistantClient::designerManualUrl();
    url += QLatin1String("designer-manual.html");
    showHelp(url);
}

void QDesignerActions::showWhatsNew()
{
    QString url = AssistantClient::qtReferenceManualUrl();
    url += QLatin1String("qt4-designer.html");
    showHelp(url);
}

void QDesignerActions::helpRequested(const QString &manual, const QString &document)
{
    QString url = AssistantClient::documentUrl(manual);
    url += document;
    showHelp(url);
}

void QDesignerActions::showHelp(const QString &url)
{
    QString errorMessage;
    if (!m_assistantClient.showPage(url, &errorMessage))
        QMessageBox::warning(core()->topLevel(), tr("Assistant"), errorMessage);
}

void QDesignerActions::aboutDesigner()
{
    VersionDialog mb(core()->topLevel());
    mb.setWindowTitle(tr("About Qt Designer"));
    if (mb.exec()) {
        QMessageBox messageBox(QMessageBox::Information, QLatin1String("Easter Egg"),
                               QLatin1String("Easter Egg"), QMessageBox::Ok, core()->topLevel());
        messageBox.setInformativeText(QLatin1String("The Easter Egg has been removed."));
        messageBox.exec();
    }
}

QAction *QDesignerActions::editWidgets() const
{
    return m_editWidgetsAction;
}

void QDesignerActions::showWidgetSpecificHelp()
{
    QString helpId;
    if (const qdesigner_internal::QDesignerIntegration *integration = qobject_cast<qdesigner_internal::QDesignerIntegration *>(core()->integration()))
        helpId = integration->contextHelpId();

    if (helpId.isEmpty()) {
        showDesignerHelp();
        return;
    }

    QString errorMessage;
    const bool rc = m_assistantClient.activateIdentifier(helpId, &errorMessage);
    if (!rc)
        QMessageBox::warning(core()->topLevel(), tr("Assistant"), errorMessage);
}

void QDesignerActions::updateCloseAction()
{
    if (m_previewManager->previewCount()) {
        m_closeFormAction->setText(tr("&Close Preview"));
    } else {
        m_closeFormAction->setText(tr("&Close"));
    }
}

void QDesignerActions::backupForms()
{
    const int count = m_workbench->formWindowCount();
    if (!count || !ensureBackupDirectories())
        return;


    QStringList tmpFiles;
    QMap<QString, QString> backupMap;
    QDir backupDir(m_backupPath);
    const bool warningsEnabled = qdesigner_internal::QSimpleResource::setWarningsEnabled(false);
    for (int i = 0; i < count; ++i) {
        QDesignerFormWindow *fw = m_workbench->formWindow(i);
        QDesignerFormWindowInterface *fwi = fw->editor();

        QString formBackupName;
        QTextStream(&formBackupName) << m_backupPath << QDir::separator()
                                     << QLatin1String("backup") << i << QLatin1String(".bak");

        QString fwn = QDir::convertSeparators(fwi->fileName());
        if (fwn.isEmpty())
            fwn = fw->windowTitle();

        backupMap.insert(fwn, formBackupName);

        QFile file(formBackupName.replace(m_backupPath, m_backupTmpPath));
        if (file.open(QFile::WriteOnly)){
            QString contents = fixResourceFileBackupPath(fwi, backupDir);
            if (qdesigner_internal::FormWindowBase *fwb = qobject_cast<qdesigner_internal::FormWindowBase *>(fwi)) {
                if (fwb->lineTerminatorMode() == qdesigner_internal::FormWindowBase::CRLFLineTerminator)
                    contents.replace(QLatin1Char('\n'), QLatin1String("\r\n"));
            }
            const QByteArray utf8Array = contents.toUtf8();
            if (file.write(utf8Array, utf8Array.size()) != utf8Array.size()) {
                backupMap.remove(fwn);
                qdesigner_internal::designerWarning(tr("The backup file %1 could not be written.").arg(file.fileName()));
            } else
                tmpFiles.append(formBackupName);

            file.close();
        }
    }
    qdesigner_internal::QSimpleResource::setWarningsEnabled(warningsEnabled);
    if(!tmpFiles.isEmpty()) {
        const QStringList backupFiles = backupDir.entryList(QDir::Files);
        if(!backupFiles.isEmpty()) {
            QStringListIterator it(backupFiles);
            while (it.hasNext())
                backupDir.remove(it.next());
        }

        QStringListIterator it(tmpFiles);
        while (it.hasNext()) {
            const QString tmpName = it.next();
            QString name(tmpName);
            name.replace(m_backupTmpPath, m_backupPath);
            QFile tmpFile(tmpName);
            if (!tmpFile.copy(name))
                qdesigner_internal::designerWarning(tr("The backup file %1 could not be written.").arg(name));
            tmpFile.remove();
        }

        m_settings.setBackup(backupMap);
    }
}

QString QDesignerActions::fixResourceFileBackupPath(QDesignerFormWindowInterface *fwi, const QDir& backupDir)
{
    const QString content = fwi->contents();
    QDomDocument domDoc(QLatin1String("backup"));
    if(!domDoc.setContent(content))
        return content;

    const QDomNodeList list = domDoc.elementsByTagName(QLatin1String("resources"));
    if (list.isEmpty())
        return content;

    for (int i = 0; i < list.count(); i++) {
        const QDomNode node = list.at(i);
        if (!node.isNull()) {
            const QDomElement element = node.toElement();
            if(!element.isNull() && element.tagName() == QLatin1String("resources")) {
                QDomNode childNode = element.firstChild();
                while (!childNode.isNull()) {
                    QDomElement childElement = childNode.toElement();
                    if(!childElement.isNull() && childElement.tagName() == QLatin1String("include")) {
                        const QString attr = childElement.attribute(QLatin1String("location"));
                        const QString path = fwi->absoluteDir().absoluteFilePath(attr);
                        childElement.setAttribute(QLatin1String("location"), backupDir.relativeFilePath(path));
                    }
                    childNode = childNode.nextSibling();
                }
            }
        }
    }


    return domDoc.toString();
}

QRect QDesignerActions::fixDialogRect(const QRect &rect) const
{
    QRect frameGeometry;
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(core()->topLevel());

    if (workbench()->mode() == DockedMode) {
        frameGeometry = core()->topLevel()->frameGeometry();
    } else
        frameGeometry = availableGeometry;

    QRect dlgRect = rect;
    dlgRect.moveCenter(frameGeometry.center());

    // make sure that parts of the dialog are not outside of screen
    dlgRect.moveBottom(qMin(dlgRect.bottom(), availableGeometry.bottom()));
    dlgRect.moveRight(qMin(dlgRect.right(), availableGeometry.right()));
    dlgRect.moveLeft(qMax(dlgRect.left(), availableGeometry.left()));
    dlgRect.moveTop(qMax(dlgRect.top(), availableGeometry.top()));

    return dlgRect;
}

void QDesignerActions::showStatusBarMessage(const QString &message) const
{
    if (workbench()->mode() == DockedMode) {
        QStatusBar *bar = qDesigner->mainWindow()->statusBar();
        if (bar && !bar->isHidden())
            bar->showMessage(message, 3000);
    }
}

void QDesignerActions::setBringAllToFrontVisible(bool visible)
{
      m_bringAllToFrontSeparator->setVisible(visible);
      m_bringAllToFrontAction->setVisible(visible);
}

void QDesignerActions::setWindowListSeparatorVisible(bool visible)
{
    m_windowListSeparatorAction->setVisible(visible);
}

bool QDesignerActions::ensureBackupDirectories() {

    if (m_backupPath.isEmpty()) {
        // create names
        m_backupPath = QDir::homePath();
        m_backupPath += QDir::separator();
        m_backupPath += QLatin1String(".designer");
        m_backupPath += QDir::separator();
        m_backupPath += QLatin1String("backup");
        m_backupPath = QDir::convertSeparators(m_backupPath );

        m_backupTmpPath = m_backupPath;
        m_backupTmpPath += QDir::separator();
        m_backupTmpPath += QLatin1String("tmp");
        m_backupTmpPath = QDir::convertSeparators(m_backupTmpPath);
    }

    // ensure directories
    const QDir backupDir(m_backupPath);
    const QDir backupTmpDir(m_backupTmpPath);

    if (!backupDir.exists()) {
        if (!backupDir.mkpath(m_backupPath)) {
            qdesigner_internal::designerWarning(tr("The backup directory %1 could not be created.").arg(m_backupPath));
            return false;
        }
    }
    if (!backupTmpDir.exists()) {
        if (!backupTmpDir.mkpath(m_backupTmpPath)) {
            qdesigner_internal::designerWarning(tr("The temporary backup directory %1 could not be created.").arg(m_backupTmpPath));
            return false;
        }
    }
    return true;
}

void QDesignerActions::showPreferencesDialog()
{
    PreferencesDialog preferencesDialog(workbench()->core(), m_core->topLevel());
    preferencesDialog.exec();
}

void QDesignerActions::showAppFontDialog()
{
    if (!m_appFontDialog) // Might get deleted when switching ui modes
        m_appFontDialog = new AppFontDialog(core()->topLevel());
    m_appFontDialog->show();
    m_appFontDialog->raise();
}

QPixmap QDesignerActions::createPreviewPixmap(QDesignerFormWindowInterface *fw)
{
    const QCursor oldCursor = core()->topLevel()->cursor();
    core()->topLevel()->setCursor(Qt::WaitCursor);

    QString errorMessage;
    const QPixmap pixmap = m_previewManager->createPreviewPixmap(fw, QString(), &errorMessage);
    core()->topLevel()->setCursor(oldCursor);
    if (pixmap.isNull()) {
        QMessageBox::warning(fw, tr("Preview failed"), errorMessage);
    }
    return pixmap;
}

qdesigner_internal::PreviewConfiguration QDesignerActions::previewConfiguration()
{
    qdesigner_internal::PreviewConfiguration pc;
    QDesignerSharedSettings settings(core());
    if (settings.isCustomPreviewConfigurationEnabled())
        pc = settings.customPreviewConfiguration();
    return pc;
}

void QDesignerActions::savePreviewImage()
{
    const char *format = "png";

    QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow();
    if (!fw)
        return;

    QImage image;
    const QString extension = QString::fromAscii(format);
    const QString filter = tr("Image files (*.%1)").arg(extension);

    QString suggestion = fw->fileName();
    if (!suggestion.isEmpty()) {
        suggestion = QFileInfo(suggestion).baseName();
        suggestion += QLatin1Char('.');
        suggestion += extension;
    }
    do {
        const QString fileName  = getSaveFileNameWithExtension(fw, tr("Save Image"), suggestion, filter, extension);
        if (fileName.isEmpty())
            break;

        if (image.isNull()) {
            const QPixmap pixmap = createPreviewPixmap(fw);
            if (pixmap.isNull())
                break;

            image = pixmap.toImage();
        }

        if (image.save(fileName, format)) {
            showStatusBarMessage(tr("Saved image %1.").arg(QFileInfo(fileName).fileName()));
            break;
        }

        QMessageBox box(QMessageBox::Warning, tr("Save Image"),
                        tr("The file %1 could not be written.").arg( fileName),
                        QMessageBox::Retry|QMessageBox::Cancel, fw);
        if (box.exec() == QMessageBox::Cancel)
            break;
    } while (true);
}

void QDesignerActions::formWindowCountChanged()
{
    const bool enabled = m_core->formWindowManager()->formWindowCount() == 0;
    /* Disable the application font action if there are form windows open
     * as the reordering of the fonts sets font properties to 'changed'
     * and overloaded fonts are not updated. */
    static const QString disabledTip = tr("Please close all forms to enable the loading of additional fonts.");
    m_appFontAction->setEnabled(enabled);
    m_appFontAction->setStatusTip(enabled ? QString() : disabledTip);
}

void QDesignerActions::printPreviewImage()
{
#ifndef QT_NO_PRINTER
    QDesignerFormWindowInterface *fw = core()->formWindowManager()->activeFormWindow();
    if (!fw)
        return;

    if (!m_printer)
        m_printer = new QPrinter(QPrinter::HighResolution);

    m_printer->setFullPage(false);

    // Grab the image to be able to a suggest suitable orientation
    const QPixmap pixmap = createPreviewPixmap(fw);
    if (pixmap.isNull())
        return;

    const QSizeF pixmapSize = pixmap.size();
    m_printer->setOrientation( pixmapSize.width() > pixmapSize.height() ?  QPrinter::Landscape :  QPrinter::Portrait);

    // Printer parameters
    QPrintDialog dialog(m_printer, fw);
    if (!dialog.exec())
        return;

    const QCursor oldCursor = core()->topLevel()->cursor();
    core()->topLevel()->setCursor(Qt::WaitCursor);
    // Estimate of required scaling to make form look the same on screen and printer.
    const double suggestedScaling = static_cast<double>(m_printer->physicalDpiX()) /  static_cast<double>(fw->physicalDpiX());

    QPainter painter(m_printer);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Clamp to page
    const QRectF page =  painter.viewport();
    const double maxScaling = qMin(page.size().width() / pixmapSize.width(), page.size().height() / pixmapSize.height());
    const double scaling = qMin(suggestedScaling, maxScaling);

    const double xOffset = page.left() + qMax(0.0, (page.size().width()  - scaling * pixmapSize.width())  / 2.0);
    const double yOffset = page.top()  + qMax(0.0, (page.size().height() - scaling * pixmapSize.height()) / 2.0);

    // Draw.
    painter.translate(xOffset, yOffset);
    painter.scale(scaling, scaling);
    painter.drawPixmap(0, 0, pixmap);
    core()->topLevel()->setCursor(oldCursor);

    showStatusBarMessage(tr("Printed %1.").arg(QFileInfo(fw->fileName()).fileName()));
#endif
}

QT_END_NAMESPACE
