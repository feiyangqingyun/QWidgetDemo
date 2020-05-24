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

#include "actioneditor_p.h"
#include "filterwidget_p.h"
#include "actionrepository_p.h"
#include "iconloader_p.h"
#include "newactiondialog_p.h"
#include "qdesigner_menu_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "qdesigner_objectinspector_p.h"
#include "qdesigner_utils_p.h"
#include "qsimpleresource_p.h"
#include "formwindowbase_p.h"
#include "qdesigner_taskmenu_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/private/abstractsettings_p.h>

#include <QtGui/QMenu>
#include <QtGui/QToolBar>
#include <QtGui/QSplitter>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QItemSelection>

#include <QtCore/QRegExp>
#include <QtCore/QDebug>
#include <QtCore/QSignalMapper>
#include <QtCore/QBuffer>

Q_DECLARE_METATYPE(QAction*)

QT_BEGIN_NAMESPACE

static const char *actionEditorViewModeKey = "ActionEditorViewMode";

static const char *iconPropertyC = "icon";
static const char *shortcutPropertyC = "shortcut";
static const char *toolTipPropertyC = "toolTip";
static const char *checkablePropertyC = "checkable";
static const char *objectNamePropertyC = "objectName";
static const char *textPropertyC = "text";

namespace qdesigner_internal {
//--------  ActionGroupDelegate
class ActionGroupDelegate: public QItemDelegate
{
public:
    ActionGroupDelegate(QObject *parent)
        : QItemDelegate(parent) {}

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        QItemDelegate::paint(painter, option, index);
    }

    virtual void drawFocus(QPainter * /*painter*/, const QStyleOptionViewItem &/*option*/, const QRect &/*rect*/) const {}
};

//--------  ActionEditor
ActionEditor::ActionEditor(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags) :
    QDesignerActionEditorInterface(parent, flags),
    m_core(core),
    m_actionGroups(0),
    m_actionView(new ActionView),
    m_actionNew(new QAction(tr("New..."), this)),
    m_actionEdit(new QAction(tr("Edit..."), this)),
    m_actionNavigateToSlot(new QAction(tr("Go to slot..."), this)),
    m_actionCopy(new QAction(tr("Copy"), this)),
    m_actionCut(new QAction(tr("Cut"), this)),
    m_actionPaste(new QAction(tr("Paste"), this)),
    m_actionSelectAll(new QAction(tr("Select all"), this)),
    m_actionDelete(new QAction(tr("Delete"), this)),
    m_viewModeGroup(new  QActionGroup(this)),
    m_iconViewAction(0),
    m_listViewAction(0),
    m_filterWidget(0),
    m_selectAssociatedWidgetsMapper(0)
{
    m_actionView->initialize(m_core);
    m_actionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    setWindowTitle(tr("Actions"));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    QToolBar *toolbar = new QToolBar;
    toolbar->setIconSize(QSize(22, 22));
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    l->addWidget(toolbar);
    // edit actions
    QIcon documentNewIcon = QIcon::fromTheme("document-new", createIconSet(QLatin1String("filenew.png")));
    m_actionNew->setIcon(documentNewIcon);
    m_actionNew->setEnabled(false);
    connect(m_actionNew, SIGNAL(triggered()), this, SLOT(slotNewAction()));
    toolbar->addAction(m_actionNew);

    connect(m_actionSelectAll, SIGNAL(triggered()), m_actionView, SLOT(selectAll()));

    m_actionCut->setEnabled(false);
    connect(m_actionCut, SIGNAL(triggered()), this, SLOT(slotCut()));
    QIcon editCutIcon = QIcon::fromTheme("edit-cut", createIconSet(QLatin1String("editcut.png")));
    m_actionCut->setIcon(editCutIcon);

    m_actionCopy->setEnabled(false);
    connect(m_actionCopy, SIGNAL(triggered()), this, SLOT(slotCopy()));
    QIcon editCopyIcon = QIcon::fromTheme("edit-copy", createIconSet(QLatin1String("editcopy.png")));
    m_actionCopy->setIcon(editCopyIcon);
    toolbar->addAction(m_actionCopy);

    connect(m_actionPaste, SIGNAL(triggered()), this, SLOT(slotPaste()));
    QIcon editPasteIcon = QIcon::fromTheme("edit-paste", createIconSet(QLatin1String("editpaste.png")));
    m_actionPaste->setIcon(editPasteIcon);
    toolbar->addAction(m_actionPaste);

    m_actionEdit->setEnabled(false);
    connect(m_actionEdit, SIGNAL(triggered()), this, SLOT(editCurrentAction()));

    connect(m_actionNavigateToSlot, SIGNAL(triggered()), this, SLOT(navigateToSlotCurrentAction()));

    QIcon editDeleteIcon = QIcon::fromTheme("edit-delete", createIconSet(QLatin1String("editdelete.png")));
    m_actionDelete->setIcon(editDeleteIcon);
    m_actionDelete->setEnabled(false);
    connect(m_actionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()));
    toolbar->addAction(m_actionDelete);

    // Toolbutton with menu containing action group for detailed/icon view. Steal the icons from the file dialog.
    //
    QMenu *configureMenu;
    toolbar->addWidget(createConfigureMenuButton(tr("Configure Action Editor"), &configureMenu));

    connect(m_viewModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotViewMode(QAction*)));
    m_iconViewAction = m_viewModeGroup->addAction(tr("Icon View"));
    m_iconViewAction->setData(QVariant(ActionView::IconView));
    m_iconViewAction->setCheckable(true);
    m_iconViewAction->setIcon(style()->standardIcon (QStyle::SP_FileDialogListView));
    configureMenu->addAction(m_iconViewAction);

    m_listViewAction = m_viewModeGroup->addAction(tr("Detailed View"));
    m_listViewAction->setData(QVariant(ActionView::DetailedView));
    m_listViewAction->setCheckable(true);
    m_listViewAction->setIcon(style()->standardIcon (QStyle::SP_FileDialogDetailedView));
    configureMenu->addAction(m_listViewAction);
    // filter
    m_filterWidget = new FilterWidget(toolbar);
    connect(m_filterWidget, SIGNAL(filterChanged(QString)), this, SLOT(setFilter(QString)));
    m_filterWidget->setEnabled(false);
    toolbar->addWidget(m_filterWidget);

    // main layout
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    splitter->addWidget(m_actionView);
    l->addWidget(splitter);

#if 0 // ### implement me
    m_actionGroups = new QListWidget(splitter);
    splitter->addWidget(m_actionGroups);
    m_actionGroups->setItemDelegate(new ActionGroupDelegate(m_actionGroups));
    m_actionGroups->setMovement(QListWidget::Static);
    m_actionGroups->setResizeMode(QListWidget::Fixed);
    m_actionGroups->setIconSize(QSize(48, 48));
    m_actionGroups->setFlow(QListWidget::TopToBottom);
    m_actionGroups->setViewMode(QListWidget::IconMode);
    m_actionGroups->setWrapping(false);
#endif

    connect(m_actionView, SIGNAL(resourceImageDropped(QString,QAction*)),
            this, SLOT(resourceImageDropped(QString,QAction*)));

    connect(m_actionView, SIGNAL(currentChanged(QAction*)),this, SLOT(slotCurrentItemChanged(QAction*)));
    // make it possible for vs integration to reimplement edit action dialog
    connect(m_actionView, SIGNAL(activated(QAction*)), this, SIGNAL(itemActivated(QAction*)));

    connect(m_actionView,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));

    connect(m_actionView, SIGNAL(contextMenuRequested(QContextMenuEvent*,QAction*)),
            this, SLOT(slotContextMenuRequested(QContextMenuEvent*,QAction*)));

    connect(this, SIGNAL(itemActivated(QAction*)), this, SLOT(editAction(QAction*)));

    restoreSettings();
    updateViewModeActions();
}

// Utility to create a configure button with menu for usage on toolbars
QToolButton *ActionEditor::createConfigureMenuButton(const QString &t, QMenu **ptrToMenu)
{
    QToolButton *configureButton = new QToolButton;
    QAction *configureAction = new QAction(t, configureButton);
    QIcon configureIcon = QIcon::fromTheme("document-properties", createIconSet(QLatin1String("configure.png")));
    configureAction->setIcon(configureIcon);
    QMenu *configureMenu = new QMenu;
    configureAction->setMenu(configureMenu);
    configureButton->setDefaultAction(configureAction);
    configureButton->setPopupMode(QToolButton::InstantPopup);
    *ptrToMenu = configureMenu;
    return configureButton;
}

ActionEditor::~ActionEditor()
{
    saveSettings();
}

QAction *ActionEditor::actionNew() const
{
    return m_actionNew;
}

QAction *ActionEditor::actionDelete() const
{
    return m_actionDelete;
}

QDesignerFormWindowInterface *ActionEditor::formWindow() const
{
    return m_formWindow;
}

void ActionEditor::setFormWindow(QDesignerFormWindowInterface *formWindow)
{
    if (formWindow != 0 && formWindow->mainContainer() == 0)
        formWindow = 0;

    // we do NOT rely on this function to update the action editor
    if (m_formWindow == formWindow)
        return;

    if (m_formWindow != 0) {
        const ActionList actionList = qFindChildren<QAction*>(m_formWindow->mainContainer());
        foreach (QAction *action, actionList)
            disconnect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
    }

    m_formWindow = formWindow;

    m_actionView->model()->clearActions();

    m_actionEdit->setEnabled(false);
    m_actionCopy->setEnabled(false);
    m_actionCut->setEnabled(false);
    m_actionDelete->setEnabled(false);

    if (!formWindow || !formWindow->mainContainer()) {
        m_actionNew->setEnabled(false);
        m_filterWidget->setEnabled(false);
        return;
    }

    m_actionNew->setEnabled(true);
    m_filterWidget->setEnabled(true);

    const ActionList actionList = qFindChildren<QAction*>(formWindow->mainContainer());
    foreach (QAction *action, actionList)
        if (!action->isSeparator() && core()->metaDataBase()->item(action) != 0) {
            // Show unless it has a menu. However, listen for change on menu actions also as it might be removed
            if (!action->menu())
                m_actionView->model()->addAction(action);
            connect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
        }

    setFilter(m_filter);
}

void  ActionEditor::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    const bool hasSelection = !selected.indexes().empty();
    m_actionCopy->setEnabled(hasSelection);
    m_actionCut->setEnabled(hasSelection);
    m_actionDelete->setEnabled(hasSelection);
}

void ActionEditor::slotCurrentItemChanged(QAction *action)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;

    const bool hasCurrentAction = action != 0;
    m_actionEdit->setEnabled(hasCurrentAction);

    if (!action) {
        fw->clearSelection();
        return;
    }

    QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core()->objectInspector());

    if (action->associatedWidgets().empty()) {
        // Special case: action not in object tree. Deselect all and set in property editor
        fw->clearSelection(false);
        if (oi)
            oi->clearSelection();
        core()->propertyEditor()->setObject(action);
    } else {
        if (oi)
            oi->selectObject(action);
    }
}

void ActionEditor::slotActionChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action != 0);

    ActionModel *model = m_actionView->model();
    const int row = model->findAction(action);
    if (row == -1) {
        if (action->menu() == 0) // action got its menu deleted, create item
            model->addAction(action);
    } else if (action->menu() != 0) { // action got its menu created, remove item
        model->removeRow(row);
    } else {
        // action text or icon changed, update item
        model->update(row);
    }
}

QDesignerFormEditorInterface *ActionEditor::core() const
{
    return m_core;
}

QString ActionEditor::filter() const
{
    return m_filter;
}

void ActionEditor::setFilter(const QString &f)
{
    m_filter = f;
    m_actionView->filter(m_filter);
}

// Set changed state of icon property,  reset when icon is cleared
static void refreshIconPropertyChanged(const QAction *action, QDesignerPropertySheetExtension *sheet)
{
    sheet->setChanged(sheet->indexOf(QLatin1String(iconPropertyC)), !action->icon().isNull());
}

void ActionEditor::manageAction(QAction *action)
{
    action->setParent(formWindow()->mainContainer());
    core()->metaDataBase()->add(action);

    if (action->isSeparator() || action->menu() != 0)
        return;

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    sheet->setChanged(sheet->indexOf(QLatin1String(objectNamePropertyC)), true);
    sheet->setChanged(sheet->indexOf(QLatin1String(textPropertyC)), true);
    refreshIconPropertyChanged(action, sheet);

    m_actionView->setCurrentIndex(m_actionView->model()->addAction(action));
    connect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));
}

void ActionEditor::unmanageAction(QAction *action)
{
    core()->metaDataBase()->remove(action);
    action->setParent(0);

    disconnect(action, SIGNAL(changed()), this, SLOT(slotActionChanged()));

    const int row = m_actionView->model()->findAction(action);
    if (row != -1)
        m_actionView->model()->remove(row);
}

// Set an initial property and mark it as changed in the sheet
static void setInitialProperty(QDesignerPropertySheetExtension *sheet, const QString &name, const QVariant &value)
{
    const int index = sheet->indexOf(name);
    Q_ASSERT(index != -1);
    sheet->setProperty(index, value);
    sheet->setChanged(index, true);
}

void ActionEditor::slotNewAction()
{
    NewActionDialog dlg(this);
    dlg.setWindowTitle(tr("New action"));

    if (dlg.exec() == QDialog::Accepted) {
        const ActionData actionData = dlg.actionData();
        m_actionView->clearSelection();

        QAction *action = new QAction(formWindow());
        action->setObjectName(actionData.name);
        formWindow()->ensureUniqueObjectName(action);
        action->setText(actionData.text);

        QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
        if (!actionData.toolTip.isEmpty())
            setInitialProperty(sheet, QLatin1String(toolTipPropertyC), actionData.toolTip);

        if (actionData.checkable)
            setInitialProperty(sheet, QLatin1String(checkablePropertyC), QVariant(true));

        if (!actionData.keysequence.value().isEmpty())
            setInitialProperty(sheet, QLatin1String(shortcutPropertyC), qVariantFromValue(actionData.keysequence));

        sheet->setProperty(sheet->indexOf(QLatin1String(iconPropertyC)), qVariantFromValue(actionData.icon));

        AddActionCommand *cmd = new AddActionCommand(formWindow());
        cmd->init(action);
        formWindow()->commandHistory()->push(cmd);
    }
}

static inline bool isSameIcon(const QIcon &i1, const QIcon &i2)
{
    return i1.serialNumber() == i2.serialNumber();
}

// return a FormWindow command to apply an icon or a reset command in case it
//  is empty.

static QDesignerFormWindowCommand *setIconPropertyCommand(const PropertySheetIconValue &newIcon, QAction *action, QDesignerFormWindowInterface *fw)
{
    const QString iconProperty = QLatin1String(iconPropertyC);
    if (newIcon.paths().isEmpty()) {
        ResetPropertyCommand *cmd = new ResetPropertyCommand(fw);
        cmd->init(action, iconProperty);
        return cmd;
    }
    SetPropertyCommand *cmd = new SetPropertyCommand(fw);
    cmd->init(action, iconProperty, qVariantFromValue(newIcon));
    return cmd;
}

// return a FormWindow command to apply a QKeySequence or a reset command
// in case it is empty.

static QDesignerFormWindowCommand *setKeySequencePropertyCommand(const PropertySheetKeySequenceValue &ks, QAction *action, QDesignerFormWindowInterface *fw)
{
    const QString shortcutProperty = QLatin1String(shortcutPropertyC);
    if (ks.value().isEmpty()) {
        ResetPropertyCommand *cmd = new ResetPropertyCommand(fw);
        cmd->init(action, shortcutProperty);
        return cmd;
    }
    SetPropertyCommand *cmd = new SetPropertyCommand(fw);
    cmd->init(action, shortcutProperty, qVariantFromValue(ks));
    return cmd;
}

// return a FormWindow command to apply a POD value or reset command in case
// it is equal to the default value.

template <class T>
QDesignerFormWindowCommand *setPropertyCommand(const QString &name, T value, T defaultValue,
                                               QObject *o, QDesignerFormWindowInterface *fw)
{
    if (value == defaultValue) {
        ResetPropertyCommand *cmd = new ResetPropertyCommand(fw);
        cmd->init(o, name);
        return cmd;
    }
    SetPropertyCommand *cmd = new SetPropertyCommand(fw);
    cmd->init(o, name, QVariant(value));
    return cmd;
}

// Return the text value of a string property via PropertySheetStringValue
static inline QString textPropertyValue(const QDesignerPropertySheetExtension *sheet, const QString &name)
{
    const int index = sheet->indexOf(name);
    Q_ASSERT(index != -1);
    const PropertySheetStringValue ps = qVariantValue<PropertySheetStringValue>(sheet->property(index));
    return ps.value();
}

void ActionEditor::editAction(QAction *action)
{
    if (!action)
        return;

    NewActionDialog dlg(this);
    dlg.setWindowTitle(tr("Edit action"));

    ActionData oldActionData;
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    oldActionData.name = action->objectName();
    oldActionData.text = action->text();
    oldActionData.toolTip = textPropertyValue(sheet, QLatin1String(toolTipPropertyC));
    oldActionData.icon = qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf(QLatin1String(iconPropertyC))));
    oldActionData.keysequence = ActionModel::actionShortCut(sheet);
    oldActionData.checkable =  action->isCheckable();
    dlg.setActionData(oldActionData);

    if (!dlg.exec())
        return;

    // figure out changes and whether to start a macro
    const ActionData newActionData = dlg.actionData();
    const unsigned changeMask = newActionData.compare(oldActionData);
    if (changeMask == 0u)
        return;

    const bool severalChanges = (changeMask != ActionData::TextChanged)      && (changeMask != ActionData::NameChanged)
                             && (changeMask != ActionData::ToolTipChanged)   && (changeMask != ActionData::IconChanged)
                             && (changeMask != ActionData::CheckableChanged) && (changeMask != ActionData::KeysequenceChanged);

    QDesignerFormWindowInterface *fw = formWindow();
    QUndoStack *undoStack = fw->commandHistory();
    if (severalChanges)
        fw->beginCommand(QLatin1String("Edit action"));

    if (changeMask & ActionData::NameChanged)
        undoStack->push(createTextPropertyCommand(QLatin1String(objectNamePropertyC), newActionData.name, action, fw));

    if (changeMask & ActionData::TextChanged)
        undoStack->push(createTextPropertyCommand(QLatin1String(textPropertyC), newActionData.text, action, fw));

    if (changeMask & ActionData::ToolTipChanged)
        undoStack->push(createTextPropertyCommand(QLatin1String(toolTipPropertyC), newActionData.toolTip, action, fw));

    if (changeMask & ActionData::IconChanged)
        undoStack->push(setIconPropertyCommand(newActionData.icon, action, fw));

    if (changeMask & ActionData::CheckableChanged)
        undoStack->push(setPropertyCommand(QLatin1String(checkablePropertyC), newActionData.checkable, false, action, fw));

    if (changeMask & ActionData::KeysequenceChanged)
        undoStack->push(setKeySequencePropertyCommand(newActionData.keysequence, action, fw));

    if (severalChanges)
        fw->endCommand();
}

void ActionEditor::editCurrentAction()
{
    if (QAction *a = m_actionView->currentAction())
        editAction(a);
}

void ActionEditor::navigateToSlotCurrentAction()
{
    if (QAction *a = m_actionView->currentAction())
        QDesignerTaskMenu::navigateToSlot(m_core, a, QLatin1String("triggered()"));
}

void ActionEditor::deleteActions(QDesignerFormWindowInterface *fw, const ActionList &actions)
{
    // We need a macro even in the case of single action because the commands might cause the
    // scheduling of other commands (signal slots connections)
    const QString description = actions.size() == 1 ?
        tr("Remove action '%1'").arg(actions.front()->objectName()) : tr("Remove actions");
    fw->beginCommand(description);
    foreach(QAction *action, actions) {
        RemoveActionCommand *cmd = new RemoveActionCommand(fw);
        cmd->init(action);
        fw->commandHistory()->push(cmd);
    }
    fw->endCommand();
}

void ActionEditor::copyActions(QDesignerFormWindowInterface *fwi, const ActionList &actions)
{
    FormWindowBase *fw = qobject_cast<FormWindowBase *>(fwi);
    if (!fw )
        return;

    FormBuilderClipboard clipboard;
    clipboard.m_actions = actions;

    if (clipboard.empty())
        return;

    QEditorFormBuilder *formBuilder = fw->createFormBuilder();
    Q_ASSERT(formBuilder);

    QBuffer buffer;
    if (buffer.open(QIODevice::WriteOnly))
        if (formBuilder->copy(&buffer, clipboard))
            qApp->clipboard()->setText(QString::fromUtf8(buffer.buffer()), QClipboard::Clipboard);
    delete formBuilder;
}

void ActionEditor::slotDelete()
{
    QDesignerFormWindowInterface *fw =  formWindow();
    if (!fw)
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    deleteActions(fw,  selection);
}

QString ActionEditor::actionTextToName(const QString &text, const QString &prefix)
{
    QString name = text;
    if (name.isEmpty())
        return QString();

    name[0] = name.at(0).toUpper();
    name.prepend(prefix);
    const QString underscore = QString(QLatin1Char('_'));
    name.replace(QRegExp(QString(QLatin1String("[^a-zA-Z_0-9]"))), underscore);
    name.replace(QRegExp(QLatin1String("__*")), underscore);
    if (name.endsWith(underscore.at(0)))
        name.truncate(name.size() - 1);

    return name;
}

void  ActionEditor::resourceImageDropped(const QString &path, QAction *action)
{
    QDesignerFormWindowInterface *fw =  formWindow();
    if (!fw)
        return;

    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core()->extensionManager(), action);
    const PropertySheetIconValue oldIcon =
            qVariantValue<PropertySheetIconValue>(sheet->property(sheet->indexOf(QLatin1String(iconPropertyC))));
    PropertySheetIconValue newIcon;
    newIcon.setPixmap(QIcon::Normal, QIcon::Off, PropertySheetPixmapValue(path));
    if (newIcon.paths().isEmpty() || newIcon.paths() == oldIcon.paths())
        return;

    fw->commandHistory()->push(setIconPropertyCommand(newIcon , action, fw));
}

void ActionEditor::mainContainerChanged()
{
    // Invalidate references to objects kept in model
    if (sender() == formWindow())
        setFormWindow(0);
}

void ActionEditor::slotViewMode(QAction *a)
{
    m_actionView->setViewMode(a->data().toInt());
    updateViewModeActions();
}

void ActionEditor::slotSelectAssociatedWidget(QWidget *w)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    QDesignerObjectInspector *oi = qobject_cast<QDesignerObjectInspector *>(core()->objectInspector());
    if (!oi)
        return;

    fw->clearSelection(); // Actually, there are no widgets selected due to focus in event handling. Just to be sure.
    oi->selectObject(w);
}

void ActionEditor::restoreSettings()
{
    QDesignerSettingsInterface *settings = m_core->settingsManager();
    m_actionView->setViewMode(settings->value(QLatin1String(actionEditorViewModeKey), 0).toInt());
    updateViewModeActions();
}

void ActionEditor::saveSettings()
{
    QDesignerSettingsInterface *settings = m_core->settingsManager();
    settings->setValue(QLatin1String(actionEditorViewModeKey), m_actionView->viewMode());
}

void ActionEditor::updateViewModeActions()
{
    switch (m_actionView->viewMode()) {
    case ActionView::IconView:
        m_iconViewAction->setChecked(true);
        break;
    case ActionView::DetailedView:
        m_listViewAction->setChecked(true);
        break;
    }
}

void ActionEditor::slotCopy()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    copyActions(fw, selection);
}

void ActionEditor::slotCut()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw )
        return;

    const ActionView::ActionList selection = m_actionView->selectedActions();
    if (selection.empty())
        return;

    copyActions(fw, selection);
    deleteActions(fw, selection);
}

void ActionEditor::slotPaste()
{
    FormWindowBase *fw = qobject_cast<FormWindowBase *>(formWindow());
    if (!fw)
        return;
    m_actionView->clearSelection();
    fw->paste(FormWindowBase::PasteActionsOnly);
}

void ActionEditor::slotContextMenuRequested(QContextMenuEvent *e, QAction *item)
{
    // set up signal mapper
    if (!m_selectAssociatedWidgetsMapper) {
        m_selectAssociatedWidgetsMapper = new QSignalMapper(this);
        connect(m_selectAssociatedWidgetsMapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotSelectAssociatedWidget(QWidget*)));
    }

    QMenu menu(this);
    menu.addAction(m_actionNew);
    menu.addSeparator();
    menu.addAction(m_actionEdit);
    if (QDesignerTaskMenu::isSlotNavigationEnabled(m_core))
        menu.addAction(m_actionNavigateToSlot);

    // Associated Widgets
    if (QAction *action = m_actionView->currentAction()) {
        const QWidgetList associatedWidgets = ActionModel::associatedWidgets(action);
        if (!associatedWidgets.empty()) {
            QMenu *associatedWidgetsSubMenu =  menu.addMenu(tr("Used In"));
            foreach (QWidget *w, associatedWidgets) {
                QAction *action = associatedWidgetsSubMenu->addAction(w->objectName());
                m_selectAssociatedWidgetsMapper->setMapping(action, w);
                connect(action, SIGNAL(triggered()), m_selectAssociatedWidgetsMapper, SLOT(map()));
            }
        }
    }

    menu.addSeparator();
    menu.addAction(m_actionCut);
    menu.addAction(m_actionCopy);
    menu.addAction(m_actionPaste);
    menu.addAction(m_actionSelectAll);
    menu.addAction(m_actionDelete);
    menu.addSeparator();
    menu.addAction(m_iconViewAction);
    menu.addAction(m_listViewAction);

    emit contextMenuRequested(&menu, item);

    menu.exec(e->globalPos());
    e->accept();
}

} // namespace qdesigner_internal

QT_END_NAMESPACE

