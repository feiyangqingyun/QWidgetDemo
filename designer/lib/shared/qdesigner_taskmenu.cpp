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

#include "qdesigner_taskmenu_p.h"
#include "qdesigner_command_p.h"
#include "richtexteditor_p.h"
#include "plaintexteditor_p.h"
#include "stylesheeteditor_p.h"
#include "qlayout_widget_p.h"
#include "layout_p.h"
#include "spacer_widget_p.h"
#include "textpropertyeditor_p.h"
#include "promotiontaskmenu_p.h"
#include "metadatabase_p.h"
#include "scriptdialog_p.h"
#include "scriptcommand_p.h"
#include "signalslotdialog_p.h"
#include "qdesigner_membersheet_p.h"
#include "qdesigner_propertycommand_p.h"
#include "qdesigner_utils_p.h"
#include "qdesigner_objectinspector_p.h"
#include "morphmenu_p.h"
#include "qdesigner_integration_p.h"
#include "formlayoutmenu_p.h"
#include "ui_selectsignaldialog.h"
#include "widgetfactory_p.h"
#include "abstractintrospection_p.h"
#include "widgetdatabase_p.h"

#include <shared_enums_p.h>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerLanguageExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QWidget>
#include <QtGui/QMenuBar>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QUndoStack>
#include <QtCore/QDebug>
#include <QtCore/QSignalMapper>
#include <QtCore/QCoreApplication>

QT_BEGIN_NAMESPACE

static QMenuBar *findMenuBar(const QWidget *widget)
{
    const QList<QObject*> children = widget->children();
    foreach (QObject *obj, widget->children()) {
        if (QMenuBar *mb = qobject_cast<QMenuBar*>(obj)) {
            return mb;
        }
    }

    return 0;
}

static QStatusBar *findStatusBar(const QWidget *widget)
{
    const QList<QObject*> children = widget->children();
    foreach (QObject *obj, widget->children()) {
        if (QStatusBar *sb = qobject_cast<QStatusBar*>(obj)) {
            return sb;
        }
    }

    return 0;
}

static inline QAction *createSeparatorHelper(QObject *parent) {
    QAction *rc = new QAction(parent);
    rc->setSeparator(true);
    return rc;
}

static inline qdesigner_internal::QDesignerIntegration *integration(const QDesignerFormEditorInterface *core) {
    return qobject_cast<qdesigner_internal::QDesignerIntegration *>(core->integration());
}

static QString objName(const QDesignerFormEditorInterface *core, QObject *object) {
    QDesignerPropertySheetExtension *sheet
            = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), object);
    Q_ASSERT(sheet != 0);

    const QString objectNameProperty = QLatin1String("objectName");
    const int index = sheet->indexOf(objectNameProperty);
    const qdesigner_internal::PropertySheetStringValue objectNameValue
            = qVariantValue<qdesigner_internal::PropertySheetStringValue>(sheet->property(index));
    return objectNameValue.value();
}

enum { ApplyMinimumWidth = 0x1, ApplyMinimumHeight = 0x2, ApplyMaximumWidth = 0x4, ApplyMaximumHeight = 0x8 };

namespace  {
// --------------- ObjectNameDialog
class ObjectNameDialog : public QDialog
{
     public:
         ObjectNameDialog(QWidget *parent, const QString &oldName);
         QString newObjectName() const;

     private:
         qdesigner_internal::TextPropertyEditor *m_editor;
};

ObjectNameDialog::ObjectNameDialog(QWidget *parent, const QString &oldName)
    : QDialog(parent),
      m_editor( new qdesigner_internal::TextPropertyEditor(this, qdesigner_internal::TextPropertyEditor::EmbeddingNone,
                                                           qdesigner_internal::ValidationObjectName))
{
    setWindowTitle(QCoreApplication::translate("ObjectNameDialog", "Change Object Name"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addWidget(new QLabel(QCoreApplication::translate("ObjectNameDialog", "Object Name")));

    m_editor->setText(oldName);
    m_editor->selectAll();
    m_editor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vboxLayout->addWidget(m_editor);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    vboxLayout->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString ObjectNameDialog::newObjectName() const
{
    return m_editor->text();
}

}

namespace qdesigner_internal {
// -------------- QDesignerTaskMenuPrivate
class QDesignerTaskMenuPrivate {
public:
    QDesignerTaskMenuPrivate(QWidget *widget, QObject *parent);

    QDesignerTaskMenu *m_q;
    QPointer<QWidget> m_widget;
    QAction *m_separator;
    QAction *m_separator2;
    QAction *m_separator3;
    QAction *m_separator4;
    QAction *m_separator5;
    QAction *m_separator6;
    QAction *m_separator7;
    QAction *m_changeObjectNameAction;
    QAction *m_changeToolTip;
    QAction *m_changeWhatsThis;
    QAction *m_changeStyleSheet;
    MorphMenu *m_morphMenu;
    FormLayoutMenu *m_formLayoutMenu;

    QAction *m_addMenuBar;
    QAction *m_addToolBar;
    QAction *m_addStatusBar;
    QAction *m_removeStatusBar;
    QAction *m_changeScript;
    QAction *m_containerFakeMethods;
    QAction *m_navigateToSlot;
    PromotionTaskMenu* m_promotionTaskMenu;
    QActionGroup *m_sizeActionGroup;
    QAction *m_sizeActionsSubMenu;
};

QDesignerTaskMenuPrivate::QDesignerTaskMenuPrivate(QWidget *widget, QObject *parent) :
    m_q(0),
    m_widget(widget),
    m_separator(createSeparatorHelper(parent)),
    m_separator2(createSeparatorHelper(parent)),
    m_separator3(createSeparatorHelper(parent)),
    m_separator4(createSeparatorHelper(parent)),
    m_separator5(createSeparatorHelper(parent)),
    m_separator6(createSeparatorHelper(parent)),
    m_separator7(createSeparatorHelper(parent)),
    m_changeObjectNameAction(new QAction(QDesignerTaskMenu::tr("Change objectName..."), parent)),
    m_changeToolTip(new QAction(QDesignerTaskMenu::tr("Change toolTip..."), parent)),
    m_changeWhatsThis(new QAction(QDesignerTaskMenu::tr("Change whatsThis..."), parent)),
    m_changeStyleSheet(new QAction(QDesignerTaskMenu::tr("Change styleSheet..."), parent)),
    m_morphMenu(new MorphMenu(parent)),
    m_formLayoutMenu(new FormLayoutMenu(parent)),
    m_addMenuBar(new QAction(QDesignerTaskMenu::tr("Create Menu Bar"), parent)),
    m_addToolBar(new QAction(QDesignerTaskMenu::tr("Add Tool Bar"), parent)),
    m_addStatusBar(new QAction(QDesignerTaskMenu::tr("Create Status Bar"), parent)),
    m_removeStatusBar(new QAction(QDesignerTaskMenu::tr("Remove Status Bar"), parent)),
    m_changeScript(new QAction(QDesignerTaskMenu::tr("Change script..."), parent)),
    m_containerFakeMethods(new QAction(QDesignerTaskMenu::tr("Change signals/slots..."), parent)),
    m_navigateToSlot(new QAction(QDesignerTaskMenu::tr("Go to slot..."), parent)),
    m_promotionTaskMenu(new PromotionTaskMenu(widget, PromotionTaskMenu::ModeManagedMultiSelection, parent)),
    m_sizeActionGroup(new QActionGroup(parent)),
    m_sizeActionsSubMenu(new QAction(QDesignerTaskMenu::tr("Size Constraints"), parent))
{
    QMenu *sizeMenu = new QMenu;
    m_sizeActionsSubMenu->setMenu(sizeMenu);
    QAction *sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Width"));
    sizeAction->setData(ApplyMinimumWidth);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Height"));
    sizeAction->setData(ApplyMinimumHeight);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Minimum Size"));
    sizeAction->setData(ApplyMinimumWidth|ApplyMinimumHeight);
    sizeMenu->addAction(sizeAction);

    sizeMenu->addSeparator();

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Width"));
    sizeAction->setData(ApplyMaximumWidth);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Height"));
    sizeAction->setData(ApplyMaximumHeight);
    sizeMenu->addAction(sizeAction);

    sizeAction = m_sizeActionGroup->addAction(QDesignerTaskMenu::tr("Set Maximum Size"));
    sizeAction->setData(ApplyMaximumWidth|ApplyMaximumHeight);
    sizeMenu->addAction(sizeAction);
}

// --------- QDesignerTaskMenu
QDesignerTaskMenu::QDesignerTaskMenu(QWidget *widget, QObject *parent) :
    QObject(parent),
    d(new QDesignerTaskMenuPrivate(widget, parent))
{
    d->m_q = this;
    Q_ASSERT(qobject_cast<QDesignerFormWindowInterface*>(widget) == 0);

    connect(d->m_changeObjectNameAction, SIGNAL(triggered()), this, SLOT(changeObjectName()));
    connect(d->m_changeToolTip, SIGNAL(triggered()), this, SLOT(changeToolTip()));
    connect(d->m_changeWhatsThis, SIGNAL(triggered()), this, SLOT(changeWhatsThis()));
    connect(d->m_changeStyleSheet, SIGNAL(triggered()), this,  SLOT(changeStyleSheet()));
    connect(d->m_addMenuBar, SIGNAL(triggered()), this, SLOT(createMenuBar()));
    connect(d->m_addToolBar, SIGNAL(triggered()), this, SLOT(addToolBar()));
    connect(d->m_addStatusBar, SIGNAL(triggered()), this, SLOT(createStatusBar()));
    connect(d->m_removeStatusBar, SIGNAL(triggered()), this, SLOT(removeStatusBar()));
    connect(d->m_changeScript, SIGNAL(triggered()), this, SLOT(changeScript()));
    connect(d->m_containerFakeMethods, SIGNAL(triggered()), this, SLOT(containerFakeMethods()));
    connect(d->m_navigateToSlot, SIGNAL(triggered()), this, SLOT(slotNavigateToSlot()));
    connect(d->m_sizeActionGroup, SIGNAL(triggered(QAction*)), this,  SLOT(applySize(QAction*)));
}

QDesignerTaskMenu::~QDesignerTaskMenu()
{
    delete d;
}

QAction *QDesignerTaskMenu::createSeparator()
{
    return createSeparatorHelper(this);
}

QWidget *QDesignerTaskMenu::widget() const
{
    return d->m_widget;
}

QDesignerFormWindowInterface *QDesignerTaskMenu::formWindow() const
{
    QDesignerFormWindowInterface *result = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(result != 0);
    return result;
}

void QDesignerTaskMenu::createMenuBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        CreateMenuBarCommand *cmd = new CreateMenuBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::addToolBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        AddToolBarCommand *cmd = new AddToolBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::createStatusBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        CreateStatusBarCommand *cmd = new CreateStatusBarCommand(fw);
        cmd->init(mw);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::removeStatusBar()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) {
            // ### warning message
            return;
        }

        DeleteStatusBarCommand *cmd = new DeleteStatusBarCommand(fw);
        cmd->init(findStatusBar(mw));
        fw->commandHistory()->push(cmd);
    }
}

QList<QAction*> QDesignerTaskMenu::taskActions() const
{
    QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(formWindow);

    const bool isMainContainer = formWindow->mainContainer() == widget();

    QList<QAction*> actions;

    if (const QMainWindow *mw = qobject_cast<const QMainWindow*>(formWindow->mainContainer()))  {
        if (isMainContainer || mw->centralWidget() == widget()) {
            if (!findMenuBar(mw)) {
                actions.append(d->m_addMenuBar);
            }

            actions.append(d->m_addToolBar);
            // ### create the status bar
            if (!findStatusBar(mw))
                actions.append(d->m_addStatusBar);
            else
                actions.append(d->m_removeStatusBar);
            actions.append(d->m_separator);
        }
    }
    actions.append(d->m_changeObjectNameAction);
    d->m_morphMenu->populate(d->m_widget, formWindow, actions);
    d->m_formLayoutMenu->populate(d->m_widget, formWindow, actions);
    actions.append(d->m_separator2);
    actions.append(d->m_changeToolTip);
    actions.append(d->m_changeWhatsThis);
    actions.append(d->m_changeStyleSheet);
    actions.append(d->m_separator6);
    actions.append(d->m_sizeActionsSubMenu);
    d->m_promotionTaskMenu->setMode(formWindow->isManaged(d->m_widget) ?
                                    PromotionTaskMenu::ModeManagedMultiSelection : PromotionTaskMenu::ModeUnmanagedMultiSelection);
    d->m_promotionTaskMenu->addActions(formWindow, PromotionTaskMenu::LeadingSeparator, actions);

#ifdef WANT_SCRIPT_OPTION
    if (!isMainContainer) {
        actions.append(d->m_separator4);
        actions.append(d->m_changeScript);
    }
#endif
    if (isMainContainer && !qt_extension<QDesignerLanguageExtension*>(formWindow->core()->extensionManager(), formWindow->core())) {
        actions.append(d->m_separator5);
        actions.append(d->m_containerFakeMethods);
    }

    if (isSlotNavigationEnabled(formWindow->core())) {
        actions.append(d->m_separator7);
        actions.append(d->m_navigateToSlot);
    }

    return actions;
}

void QDesignerTaskMenu::changeObjectName()
{
    QDesignerFormWindowInterface *fw = formWindow();
    Q_ASSERT(fw != 0);

    const QString oldObjectName = objName(fw->core(), widget());

    ObjectNameDialog dialog(fw, oldObjectName);
    if (dialog.exec() == QDialog::Accepted) {
        const QString newObjectName = dialog.newObjectName();
        if (!newObjectName.isEmpty() && newObjectName  != oldObjectName ) {
            const QString objectNameProperty = QLatin1String("objectName");
            PropertySheetStringValue objectNameValue;
            objectNameValue.setValue(newObjectName);
            setProperty(fw, CurrentWidgetMode, objectNameProperty, qVariantFromValue(objectNameValue));
        }
    }
}

void QDesignerTaskMenu::changeTextProperty(const QString &propertyName, const QString &windowTitle, PropertyMode pm, Qt::TextFormat desiredFormat)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;
    Q_ASSERT(d->m_widget->parentWidget() != 0);

    const QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(fw->core()->extensionManager(), d->m_widget);
    const int index = sheet->indexOf(propertyName);
    if (index == -1) {
        qDebug() << "** WARNING Invalid property" << propertyName << " passed to changeTextProperty!";
        return;
    }
    PropertySheetStringValue textValue = qVariantValue<PropertySheetStringValue>(sheet->property(index));
    const QString oldText = textValue.value();
    // Pop up respective dialog
    bool accepted = false;
    QString newText;
    switch (desiredFormat) {
    case Qt::PlainText: {
        PlainTextEditorDialog dlg(fw->core(), fw);
        if (!windowTitle.isEmpty())
            dlg.setWindowTitle(windowTitle);
        dlg.setDefaultFont(d->m_widget->font());
        dlg.setText(oldText);
        accepted = dlg.showDialog() == QDialog::Accepted;
        newText = dlg.text();
    }
        break;
    default: {
        RichTextEditorDialog dlg(fw->core(), fw);
        if (!windowTitle.isEmpty())
            dlg.setWindowTitle(windowTitle);
        dlg.setDefaultFont(d->m_widget->font());
        dlg.setText(oldText);
        accepted = dlg.showDialog() == QDialog::Accepted;
        newText = dlg.text(desiredFormat);
    }
        break;
    }
    // change property
    if (!accepted || oldText == newText)
          return;


    textValue.setValue(newText);
    setProperty(fw, pm, propertyName, qVariantFromValue(textValue));
}

void QDesignerTaskMenu::changeToolTip()
{
    changeTextProperty(QLatin1String("toolTip"), tr("Edit ToolTip"), MultiSelectionMode, Qt::AutoText);
}

void QDesignerTaskMenu::changeWhatsThis()
{
    changeTextProperty(QLatin1String("whatsThis"), tr("Edit WhatsThis"), MultiSelectionMode, Qt::AutoText);
}

void QDesignerTaskMenu::changeStyleSheet()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        StyleSheetPropertyEditorDialog dlg(fw, fw, d->m_widget);
        dlg.exec();
    }
}

void QDesignerTaskMenu::changeScript()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;

    MetaDataBase *metaDataBase = qobject_cast<MetaDataBase*>(fw->core()->metaDataBase());
    if (!metaDataBase)
        return;

    const MetaDataBaseItem* item = metaDataBase->metaDataBaseItem(d->m_widget);
    if (!item)
        return;

    const QString oldScript = item->script();
    QString newScript = oldScript;

    ScriptDialog scriptDialog(fw->core()->dialogGui(), fw);
    if (!scriptDialog.editScript(newScript))
        return;

    // compile list of selected objects
    ScriptCommand *scriptCommand = new ScriptCommand(fw);
    if (!scriptCommand->init(applicableObjects(fw, MultiSelectionMode), newScript)) {
        delete scriptCommand;
        return;
    }

    fw->commandHistory()->push(scriptCommand);
}

void QDesignerTaskMenu::containerFakeMethods()
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;
    SignalSlotDialog::editMetaDataBase(fw, d->m_widget, fw);
}

static QString declaredInClass(const QDesignerMetaObjectInterface *metaObject, const QString &member)
{
    // Find class whose superclass does not contain the method.
    const QDesignerMetaObjectInterface *meta = metaObject;

    for (;;) {
        const QDesignerMetaObjectInterface *tmpMeta = meta->superClass();
        if (tmpMeta == 0)
            break;
        if (tmpMeta->indexOfMethod(member) == -1)
            break;
        meta = tmpMeta;
    }
    return meta->className();
}

bool QDesignerTaskMenu::isSlotNavigationEnabled(const QDesignerFormEditorInterface *core)
{
    if (QDesignerIntegration *integr = integration(core))
        return integr->isSlotNavigationEnabled();
    return false;
}

void QDesignerTaskMenu::slotNavigateToSlot()
{
    QDesignerFormEditorInterface *core = formWindow()->core();
    Q_ASSERT(core);
    navigateToSlot(core, widget());
}

void QDesignerTaskMenu::navigateToSlot(QDesignerFormEditorInterface *core,
                                       QObject *object,
                                       const QString &defaultSignal)
{
    const QString objectName = objName(core, object);
    QMap<QString, QMap<QString, QStringList> > classToSignalList;

    QDesignerIntegration *integr = integration(core);

    // "real" signals
    if (const QDesignerMetaObjectInterface *metaObject = core->introspection()->metaObject(object)) {
        const int methodCount = metaObject->methodCount();
        for (int i = 0; i < methodCount; ++i) {
            const QDesignerMetaMethodInterface *metaMethod = metaObject->method(i);
            if (metaMethod->methodType() == QDesignerMetaMethodInterface::Signal) {
                const QString signature = metaMethod->signature();
                const QStringList parameterNames = metaMethod->parameterNames();
                classToSignalList[declaredInClass(metaObject, signature)][signature] = parameterNames;
            }
        }
    }

    // fake signals
    if (qdesigner_internal::MetaDataBase *metaDataBase
        = qobject_cast<qdesigner_internal::MetaDataBase *>(core->metaDataBase())) {
        qdesigner_internal::MetaDataBaseItem *item = metaDataBase->metaDataBaseItem(object);
        Q_ASSERT(item);
        const QStringList fakeSignals = item->fakeSignals();
        foreach (const QString &fakeSignal, fakeSignals)
            classToSignalList[item->customClassName()][fakeSignal] = QStringList();
    }

    if (object->isWidgetType()) {
        QWidget *widget = static_cast<QWidget *>(object);
        if (WidgetDataBase *db = qobject_cast<WidgetDataBase *>(core->widgetDataBase())) {
            const QString promotedClassName = promotedCustomClassName(core, widget);
            const int index = core->widgetDataBase()->indexOfClassName(promotedClassName);
            if (index >= 0) {
                WidgetDataBaseItem* item = static_cast<WidgetDataBaseItem*>(db->item(index));
                const QStringList fakeSignals = item->fakeSignals();
                foreach (const QString &fakeSignal, fakeSignals)
                    classToSignalList[promotedClassName][fakeSignal] = QStringList();
            }
        }
    }

    Ui::SelectSignalDialog dialogUi;
    QDialog selectSignalDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    dialogUi.setupUi(&selectSignalDialog);

    QMap<QString, QMap<QString, QStringList> >::const_iterator iter(classToSignalList.constBegin());
    for (; iter != classToSignalList.constEnd(); ++iter) {
        const QString className = iter.key();
        QMap<QString, QStringList> signalNames = iter.value();

        QMap<QString, QStringList>::const_iterator itSignal(signalNames.constBegin());
        for (; itSignal != signalNames.constEnd(); ++itSignal) {
            const QString signalName = itSignal.key();
            QTreeWidgetItem *row = new QTreeWidgetItem(QStringList() << signalName << className);
            row->setData(0, Qt::UserRole, itSignal.value());
            dialogUi.signalList->addTopLevelItem(row);
        }
    }
    if (dialogUi.signalList->topLevelItemCount() == 0) {
        QTreeWidgetItem *row = new QTreeWidgetItem(QStringList() << tr("no signals available"));
        dialogUi.signalList->addTopLevelItem(row);
        dialogUi.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        connect(dialogUi.signalList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                &selectSignalDialog, SLOT(accept()));
    }

    if (defaultSignal.isEmpty()) {
        dialogUi.signalList->setCurrentItem(dialogUi.signalList->topLevelItem(0));
    } else {
        const QList<QTreeWidgetItem *> items = dialogUi.signalList->findItems (defaultSignal, Qt::MatchExactly, 0);
        if (!items.empty())
            dialogUi.signalList->setCurrentItem(items.front());
    }

    dialogUi.signalList->resizeColumnToContents(0);

    if (selectSignalDialog.exec() == QDialog::Accepted) {
        QTreeWidgetItem *selectedItem = dialogUi.signalList->selectedItems().first();
        const QString signalSignature = selectedItem->text(0);
        const QStringList parameterNames = qVariantValue<QStringList>(selectedItem->data(0, Qt::UserRole));

        // TODO: Check whether signal is connected to slot
        integr->emitNavigateToSlot(objectName, signalSignature, parameterNames);
    }
}

// Add a command that takes over the value of the current geometry as
// minimum/maximum size according to the flags.
static void createSizeCommand(QDesignerFormWindowInterface *fw, QWidget *w, int flags)
{
    const QSize size = w->size();
    if (flags & (ApplyMinimumWidth|ApplyMinimumHeight)) {
        QSize minimumSize = w-> minimumSize();
        if (flags & ApplyMinimumWidth)
            minimumSize.setWidth(size.width());
        if (flags & ApplyMinimumHeight)
             minimumSize.setHeight(size.height());
        SetPropertyCommand* cmd = new SetPropertyCommand(fw);
        cmd->init(w, QLatin1String("minimumSize"), minimumSize);
        fw->commandHistory()->push(cmd);
    }
    if (flags & (ApplyMaximumWidth|ApplyMaximumHeight)) {
        QSize maximumSize = w-> maximumSize();
        if (flags & ApplyMaximumWidth)
            maximumSize.setWidth(size.width());
        if (flags & ApplyMaximumHeight)
             maximumSize.setHeight(size.height());
        SetPropertyCommand* cmd = new SetPropertyCommand(fw);
        cmd->init(w, QLatin1String("maximumSize"), maximumSize);
        fw->commandHistory()->push(cmd);
    }
}

void QDesignerTaskMenu::applySize(QAction *a)
{
    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return;

    const QWidgetList selection = applicableWidgets(fw, MultiSelectionMode);
    if (selection.isEmpty())
        return;

    const int mask = a->data().toInt();
    const int size = selection.size();
    fw->commandHistory()->beginMacro(tr("Set size constraint on %n widget(s)", 0, size));
    for (int i = 0; i < size; i++)
        createSizeCommand(fw, selection.at(i), mask);
    fw->commandHistory()->endMacro();
}

template <class Container>
    static void getApplicableObjects(const QDesignerFormWindowInterface *fw, QWidget *current,
                                     QDesignerTaskMenu::PropertyMode pm, Container *c)
{
    // Current is always first
    c->push_back(current);
    if (pm == QDesignerTaskMenu::CurrentWidgetMode)
        return;
    QDesignerObjectInspector *designerObjectInspector = qobject_cast<QDesignerObjectInspector *>(fw->core()->objectInspector());
    if (!designerObjectInspector)
        return; // Ooops, someone plugged an old-style Object Inspector
    // Add managed or unmanaged selection according to current type, make current first
    Selection s;
    designerObjectInspector->getSelection(s);
    const QWidgetList &source = fw->isManaged(current) ? s.managed : s.unmanaged;
    const QWidgetList::const_iterator cend = source.constEnd();
    for ( QWidgetList::const_iterator it = source.constBegin(); it != cend; ++it)
        if (*it != current) // was first
            c->push_back(*it);
}

QObjectList QDesignerTaskMenu::applicableObjects(const QDesignerFormWindowInterface *fw, PropertyMode pm) const
{
    QObjectList rc;
    getApplicableObjects(fw, d->m_widget, pm, &rc);
    return rc;
}

QWidgetList QDesignerTaskMenu::applicableWidgets(const QDesignerFormWindowInterface *fw, PropertyMode pm) const
{
    QWidgetList rc;
    getApplicableObjects(fw, d->m_widget, pm, &rc);
    return rc;
}

void QDesignerTaskMenu::setProperty(QDesignerFormWindowInterface *fw,  PropertyMode pm, const QString &name, const QVariant &newValue)
{
    SetPropertyCommand* setPropertyCommand = new SetPropertyCommand(fw);
    if (setPropertyCommand->init(applicableObjects(fw, pm), name, newValue, d->m_widget)) {
        fw->commandHistory()->push(setPropertyCommand);
    } else {
        delete setPropertyCommand;
        qDebug() << "Unable to set property " << name << '.';
    }
}


} // namespace qdesigner_internal

QT_END_NAMESPACE
