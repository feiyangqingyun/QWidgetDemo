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

#include "actionrepository_p.h"
#include "qtresourceview_p.h"
#include "iconloader_p.h"
#include "qdesigner_utils_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>

#include <QtGui/QDrag>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QStandardItemModel>
#include <QtGui/QToolButton>
#include <QtGui/QPixmap>
#include <QtGui/QAction>
#include <QtGui/QHeaderView>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtGui/qevent.h>
#include <QtCore/QSet>
#include <QtCore/QDebug>

Q_DECLARE_METATYPE(QAction*)

QT_BEGIN_NAMESPACE

namespace {
    enum { listModeIconSize = 16, iconModeIconSize = 24 };
}

static const char *actionMimeType = "action-repository/actions";
static const char *plainTextMimeType = "text/plain";

static inline QAction *actionOfItem(const QStandardItem* item)
{
    return qvariant_cast<QAction*>(item->data(qdesigner_internal::ActionModel::ActionRole));
}

namespace qdesigner_internal {

// ----------- ActionModel
ActionModel::ActionModel(QWidget *parent ) :
    QStandardItemModel(parent),
    m_emptyIcon(emptyIcon()),
    m_core(0)
{
    QStringList headers;
    headers += tr("Name");
    headers += tr("Used");
    headers += tr("Text");
    headers += tr("Shortcut");
    headers += tr("Checkable");
    headers += tr("ToolTip");
    Q_ASSERT(NumColumns == headers.size());
    setHorizontalHeaderLabels(headers);
}

void ActionModel::clearActions()
{
    removeRows(0, rowCount());
}

int ActionModel::findAction(QAction *action) const
{
    const int rows = rowCount();
    for (int i = 0; i < rows; i++)
       if (action == actionOfItem(item(i)))
           return i;
    return -1;
}

void ActionModel::update(int row)
{
    Q_ASSERT(m_core);
    // need to create the row list ... grrr..
    if (row >= rowCount())
       return;

    QStandardItemList list;
    for (int i = 0; i < NumColumns; i++)
       list += item(row, i);

    setItems(m_core, actionOfItem(list.front()), m_emptyIcon, list);
}

void ActionModel::remove(int row)
{
    qDeleteAll(takeRow(row));
}

QModelIndex ActionModel::addAction(QAction *action)
{
    Q_ASSERT(m_core);
    QStandardItemList items;
    const  Qt::ItemFlags flags = Qt::ItemIsSelectable|Qt::ItemIsDropEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsEnabled;

    QVariant itemData;
    qVariantSetValue(itemData, action);

    for (int i = 0; i < NumColumns; i++) {
        QStandardItem *item = new QStandardItem;
        item->setData(itemData, ActionRole);
        item->setFlags(flags);
        items.push_back(item);
    }
    setItems(m_core, action, m_emptyIcon, items);
    appendRow(items);
    return indexFromItem(items.front());
}

// Find the associated menus and toolbars, ignore toolbuttons
QWidgetList ActionModel::associatedWidgets(const QAction *action)
{
    QWidgetList rc = action->associatedWidgets();
    for (QWidgetList::iterator it = rc.begin(); it != rc.end(); )
        if (qobject_cast<const QMenu *>(*it) || qobject_cast<const QToolBar *>(*it)) {
            ++it;
        } else {
            it = rc.erase(it);
        }
    return rc;
}

// shortcut is a fake property, need to retrieve it via property sheet.
PropertySheetKeySequenceValue ActionModel::actionShortCut(QDesignerFormEditorInterface *core, QAction *action)
{
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(core->extensionManager(), action);
    if (!sheet)
        return PropertySheetKeySequenceValue();
    return actionShortCut(sheet);
}

PropertySheetKeySequenceValue ActionModel::actionShortCut(const QDesignerPropertySheetExtension *sheet)
{
    const int index = sheet->indexOf(QLatin1String("shortcut"));
    if (index == -1)
        return PropertySheetKeySequenceValue();
    return qvariant_cast<PropertySheetKeySequenceValue>(sheet->property(index));
}

void  ActionModel::setItems(QDesignerFormEditorInterface *core, QAction *action,
                            const QIcon &defaultIcon,
                            QStandardItemList &sl)
{

    // Tooltip, mostly for icon view mode
    QString firstTooltip = action->objectName();
    const QString text = action->text();
    if (!text.isEmpty()) {
        firstTooltip += QLatin1Char('\n');
        firstTooltip += text;
    }

    Q_ASSERT(sl.size() == NumColumns);

    QStandardItem *item =  sl[NameColumn];
    item->setText(action->objectName());
    QIcon icon = action->icon();
    if (icon.isNull())
        icon = defaultIcon;
    item->setIcon(icon);
    item->setToolTip(firstTooltip);
    item->setWhatsThis(firstTooltip);
    // Used
    const QWidgetList associatedDesignerWidgets = associatedWidgets(action);
    const bool used = !associatedDesignerWidgets.empty();
    item = sl[UsedColumn];
    item->setCheckState(used ? Qt::Checked : Qt::Unchecked);
    if (used) {
        QString usedToolTip;
        const QString separator = QLatin1String(", ");
        const int count = associatedDesignerWidgets.size();
        for (int i = 0; i < count; i++) {
            if (i)
                usedToolTip += separator;
            usedToolTip += associatedDesignerWidgets.at(i)->objectName();
        }
        item->setToolTip(usedToolTip);
    } else {
        item->setToolTip(QString());
    }
    // text
    item = sl[TextColumn];
    item->setText(action->text());
    item->setToolTip(action->text());
    // shortcut
    const QString shortcut = actionShortCut(core, action).value().toString(QKeySequence::NativeText);
    item = sl[ShortCutColumn];
    item->setText(shortcut);
    item->setToolTip(shortcut);
    // checkable
    sl[CheckedColumn]->setCheckState(action->isCheckable() ?  Qt::Checked : Qt::Unchecked);
    // ToolTip. This might be multi-line, rich text
    QString toolTip = action->toolTip();
    item = sl[ToolTipColumn];
    item->setToolTip(toolTip);
    item->setText(toolTip.replace(QLatin1Char('\n'), QLatin1Char(' ')));
}

QMimeData *ActionModel::mimeData(const QModelIndexList &indexes ) const
{
    ActionRepositoryMimeData::ActionList actionList;

    QSet<QAction*> actions;
    foreach (const QModelIndex &index, indexes)
        if (QStandardItem *item = itemFromIndex(index))
            if (QAction *action = actionOfItem(item))
                actions.insert(action);
    return new ActionRepositoryMimeData(actions.toList(), Qt::CopyAction);
}

// Resource images are plain text. The drag needs to be restricted, however.
QStringList ActionModel::mimeTypes() const
{
    return QStringList(QLatin1String(plainTextMimeType));
}

QString ActionModel::actionName(int row) const
{
    return item(row, NameColumn)->text();
}

bool ActionModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &)
{
  if (action != Qt::CopyAction)
        return false;

    QStandardItem *droppedItem = item(row, column);
    if (!droppedItem)
        return false;


    QtResourceView::ResourceType type;
    QString path;
    if (!QtResourceView::decodeMimeData(data, &type, &path) || type !=  QtResourceView::ResourceImage)
        return false;

    emit resourceImageDropped(path, actionOfItem(droppedItem));
    return true;
}

QAction *ActionModel::actionAt(const  QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    QStandardItem *i = itemFromIndex(index);
    if (!i)
        return 0;
    return actionOfItem(i);
}

// helpers

static bool handleImageDragEnterMoveEvent(QDropEvent *event)
{
    QtResourceView::ResourceType type;
    const bool rc = QtResourceView::decodeMimeData(event->mimeData(), &type) && type ==  QtResourceView::ResourceImage;
    if (rc)
        event->acceptProposedAction();
    else
        event->ignore();
    return rc;
}

static void handleImageDropEvent(const QAbstractItemView *iv, QDropEvent *event, ActionModel *am)
{
    const QModelIndex index = iv->indexAt(event->pos());
    if (!index.isValid()) {
        event->ignore();
        return;
    }

    if (!handleImageDragEnterMoveEvent(event))
        return;

    am->dropMimeData(event->mimeData(),  event->proposedAction(), index.row(), 0, iv->rootIndex());
}

// Basically mimic  QAbstractItemView's startDrag routine, except that
// another pixmap is used, we don't want the whole row.

void startActionDrag(QWidget *dragParent, ActionModel *model, const QModelIndexList &indexes, Qt::DropActions supportedActions)
{
    if (indexes.empty())
        return;

    QDrag *drag = new QDrag(dragParent);
    QMimeData *data = model->mimeData(indexes);
    drag->setMimeData(data);
    if (ActionRepositoryMimeData *actionMimeData = qobject_cast<ActionRepositoryMimeData *>(data))
        drag->setPixmap(ActionRepositoryMimeData::actionDragPixmap(actionMimeData->actionList().front()));

    drag->start(supportedActions);
}

// ---------------- ActionTreeView:
ActionTreeView::ActionTreeView(ActionModel *model, QWidget *parent) :
    QTreeView(parent),
    m_model(model)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(DragDrop);
    setModel(model);
    setRootIsDecorated(false);
    setTextElideMode(Qt::ElideMiddle);

    setModel(model);
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(slotActivated(QModelIndex)));
    connect(header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(resizeColumnToContents(int)));

    setIconSize(QSize(listModeIconSize, listModeIconSize));

}

QAction *ActionTreeView::currentAction() const
{
    return m_model->actionAt(currentIndex());
}

void ActionTreeView::filter(const QString &text)
{
    const int rowCount = m_model->rowCount();
    const bool empty = text.isEmpty();
    const QModelIndex parent = rootIndex();
    for (int i = 0; i < rowCount; i++)
        setRowHidden(i, parent, !empty && !m_model->actionName(i).contains(text, Qt::CaseInsensitive));
}

void ActionTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    handleImageDragEnterMoveEvent(event);
}

void ActionTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    handleImageDragEnterMoveEvent(event);
}

void ActionTreeView::dropEvent(QDropEvent *event)
{
    handleImageDropEvent(this, event, m_model);
}

void ActionTreeView::focusInEvent(QFocusEvent *event)
{
    QTreeView::focusInEvent(event);
    // Make property editor display current action
    if (QAction *a = currentAction())
       emit currentChanged(a);
}

void ActionTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    emit contextMenuRequested(event, m_model->actionAt(indexAt(event->pos())));
}

void ActionTreeView::currentChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    emit currentChanged(m_model->actionAt(current));
}

void ActionTreeView::slotActivated(const QModelIndex &index)
{
    emit activated(m_model->actionAt(index));
}

void ActionTreeView::startDrag(Qt::DropActions supportedActions)
{
    startActionDrag(this, m_model, selectedIndexes(), supportedActions);
}

// ---------------- ActionListView:
ActionListView::ActionListView(ActionModel *model, QWidget *parent) :
    QListView(parent),
    m_model(model)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(DragDrop);
    setModel(model);
    setTextElideMode(Qt::ElideMiddle);
    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(slotActivated(QModelIndex)));

    // We actually want 'Static' as the user should be able to
    // drag away actions only (not to rearrange icons).
    // We emulate that by not accepting our own
    // drag data. 'Static' causes the list view to disable drag and drop
    // on the viewport.
    setMovement(Snap);
    setViewMode(IconMode);
    setIconSize(QSize(iconModeIconSize, iconModeIconSize));
    setGridSize(QSize(4 * iconModeIconSize, 2 *  iconModeIconSize));
    setSpacing(iconModeIconSize / 3);
}

QAction *ActionListView::currentAction() const
{
    return m_model->actionAt(currentIndex());
}

void ActionListView::filter(const QString &text)
{
    const int rowCount = m_model->rowCount();
    const bool empty = text.isEmpty();
    for (int i = 0; i < rowCount; i++)
        setRowHidden(i, !empty && !m_model->actionName(i).contains(text, Qt::CaseInsensitive));
}

void ActionListView::dragEnterEvent(QDragEnterEvent *event)
{
    handleImageDragEnterMoveEvent(event);
}

void ActionListView::dragMoveEvent(QDragMoveEvent *event)
{
    handleImageDragEnterMoveEvent(event);
}

void ActionListView::dropEvent(QDropEvent *event)
{
    handleImageDropEvent(this, event, m_model);
}

void ActionListView::focusInEvent(QFocusEvent *event)
{
    QListView::focusInEvent(event);
    // Make property editor display current action
    if (QAction *a = currentAction())
       emit currentChanged(a);
}

void ActionListView::contextMenuEvent(QContextMenuEvent *event)
{
    emit contextMenuRequested(event, m_model->actionAt(indexAt(event->pos())));
}

void ActionListView::currentChanged(const QModelIndex &current, const QModelIndex & /*previous*/)
{
    emit currentChanged(m_model->actionAt(current));
}

void ActionListView::slotActivated(const QModelIndex &index)
{
    emit activated(m_model->actionAt(index));
}

void ActionListView::startDrag(Qt::DropActions supportedActions)
{
    startActionDrag(this, m_model, selectedIndexes(), supportedActions);
}

//  ActionView
ActionView::ActionView(QWidget *parent) :
    QStackedWidget(parent),
    m_model(new ActionModel(this)),
    m_actionTreeView(new ActionTreeView(m_model)),
    m_actionListView(new ActionListView(m_model))
{
    addWidget(m_actionListView);
    addWidget(m_actionTreeView);
    // Wire signals
    connect(m_actionTreeView, SIGNAL(contextMenuRequested(QContextMenuEvent*,QAction*)),
            this, SIGNAL(contextMenuRequested(QContextMenuEvent*,QAction*)));
    connect(m_actionListView, SIGNAL(contextMenuRequested(QContextMenuEvent*,QAction*)),
            this, SIGNAL(contextMenuRequested(QContextMenuEvent*,QAction*)));

    // make it possible for vs integration to reimplement edit action dialog
    // [which it shouldn't do actually]
    connect(m_actionListView, SIGNAL(activated(QAction*)), this, SIGNAL(activated(QAction*)));
    connect(m_actionTreeView, SIGNAL(activated(QAction*)), this, SIGNAL(activated(QAction*)));

    connect(m_actionListView, SIGNAL(currentChanged(QAction*)),this, SLOT(slotCurrentChanged(QAction*)));
    connect(m_actionTreeView, SIGNAL(currentChanged(QAction*)),this, SLOT(slotCurrentChanged(QAction*)));

    connect(m_model, SIGNAL(resourceImageDropped(QString,QAction*)),
            this, SIGNAL(resourceImageDropped(QString,QAction*)));

    // sync selection models
    QItemSelectionModel *selectionModel = m_actionTreeView->selectionModel();
    m_actionListView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
}

int ActionView::viewMode() const
{
    return currentWidget() == m_actionListView ? IconView : DetailedView;
}

void ActionView::setViewMode(int lm)
{
    if (viewMode() == lm)
        return;

    switch (lm) {
    case IconView:
        setCurrentWidget(m_actionListView);
        break;
    case DetailedView:
        setCurrentWidget(m_actionTreeView);
        break;
    default:
        break;
    }
}

void ActionView::slotCurrentChanged(QAction *action)
{
    // emit only for currently visible
    if (sender() == currentWidget())
        emit currentChanged(action);
}

void ActionView::filter(const QString &text)
{
    m_actionTreeView->filter(text);
    m_actionListView->filter(text);
}

void ActionView::selectAll()
{
    m_actionTreeView->selectAll();
}

void ActionView::clearSelection()
{
    m_actionTreeView->selectionModel()->clearSelection();
}

void ActionView::setCurrentIndex(const QModelIndex &index)
{
    m_actionTreeView->setCurrentIndex(index);
}

QAction *ActionView::currentAction() const
{
    return m_actionListView->currentAction();
}

void ActionView::setSelectionMode(QAbstractItemView::SelectionMode sm)
{
    m_actionTreeView->setSelectionMode(sm);
    m_actionListView->setSelectionMode(sm);
}

QAbstractItemView::SelectionMode ActionView::selectionMode() const
{
    return m_actionListView->selectionMode();
}

QItemSelection ActionView::selection() const
{
    return  m_actionListView->selectionModel()->selection();
}

ActionView::ActionList ActionView::selectedActions() const
{
    ActionList rc;
    foreach (const QModelIndex &index, selection().indexes())
        if (index.column() == 0)
            rc += actionOfItem(m_model->itemFromIndex(index));
    return rc;
}
// ----------  ActionRepositoryMimeData
ActionRepositoryMimeData::ActionRepositoryMimeData(QAction *a, Qt::DropAction dropAction) :
    m_dropAction(dropAction)
{
    m_actionList += a;
}

ActionRepositoryMimeData::ActionRepositoryMimeData(const ActionList &al, Qt::DropAction dropAction) :
    m_dropAction(dropAction),
    m_actionList(al)
{
}

QStringList ActionRepositoryMimeData::formats() const
{
    return QStringList(QLatin1String(actionMimeType));
}

QPixmap  ActionRepositoryMimeData::actionDragPixmap(const QAction *action)
{

    // Try to find a suitable pixmap. Grab either widget or icon.
    const QIcon icon = action->icon();
    if (!icon.isNull())
        return icon.pixmap(QSize(22, 22));

    foreach (QWidget *w, action->associatedWidgets())
        if (QToolButton *tb = qobject_cast<QToolButton *>(w))
            return QPixmap::grabWidget(tb);

    // Create a QToolButton
    QToolButton *tb = new QToolButton;
    tb->setText(action->text());
    tb->setToolButtonStyle(Qt::ToolButtonTextOnly);
#ifdef Q_WS_WIN // Force alien off to make adjustSize() take the system minimumsize into account.
    tb->createWinId();
#endif
    tb->adjustSize();
    const QPixmap rc = QPixmap::grabWidget(tb);
    tb->deleteLater();
    return rc;
}

void ActionRepositoryMimeData::accept(QDragMoveEvent *event) const
{
    if (event->proposedAction() == m_dropAction) {
        event->acceptProposedAction();
    } else {
        event->setDropAction(m_dropAction);
        event->accept();
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
