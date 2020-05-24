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

#include "qdesigner_tabwidget_p.h"
#include "qdesigner_command_p.h"
#include "qdesigner_propertycommand_p.h"
#include "promotiontaskmenu_p.h"
#include "formwindowbase_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>

#include <QtGui/QApplication>
#include <QtGui/QTabBar>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {
// Store tab widget as drag source
class MyMimeData : public QMimeData
{
    Q_OBJECT
public:
    MyMimeData(const QTabWidget *tab) : m_tab(tab) {}
    static bool fromMyTab(const QMimeData *mimeData, const QTabWidget *tab) {
        if (!mimeData)
            return false;
        const MyMimeData *m = qobject_cast<const MyMimeData *>(mimeData);
        return m &&  m->m_tab ==  tab;
    }
private:
    const QTabWidget *m_tab;
};

} // namespace qdesigner_internal

// ------------- QTabWidgetEventFilter

QTabWidgetEventFilter::QTabWidgetEventFilter(QTabWidget *parent) :
    QObject(parent),
    m_tabWidget(parent),
    m_dropIndicator(0),
    m_dragPage(0),
    m_mousePressed(false),
    m_actionDeletePage(new QAction(tr("Delete"),  this)),
    m_actionInsertPage(new QAction(tr("Before Current Page"), this)),
    m_actionInsertPageAfter(new QAction(tr("After Current Page"), this)),
    m_pagePromotionTaskMenu(new qdesigner_internal::PromotionTaskMenu(0, qdesigner_internal::PromotionTaskMenu::ModeSingleWidget, this))
{
    tabBar()->setAcceptDrops(true);
    tabBar()->installEventFilter(this);

    connect(m_actionInsertPage, SIGNAL(triggered()), this, SLOT(addPage()));
    connect(m_actionInsertPageAfter, SIGNAL(triggered()), this, SLOT(addPageAfter()));
    connect(m_actionDeletePage, SIGNAL(triggered()), this, SLOT(removeCurrentPage()));
}

QTabWidgetEventFilter::~QTabWidgetEventFilter()
{
}

void QTabWidgetEventFilter::install(QTabWidget *tabWidget)
{
    new QTabWidgetEventFilter(tabWidget);
}

QTabWidgetEventFilter *QTabWidgetEventFilter::eventFilterOf(const QTabWidget *tabWidget)
{
    // Look for 1st order children only..otherwise, we might get filters of nested tab widgets
    const QObjectList children = tabWidget->children();
    const QObjectList::const_iterator cend = children.constEnd();
    for (QObjectList::const_iterator it = children.constBegin(); it != cend; ++it) {
        QObject *o = *it;
        if (!o->isWidgetType())
            if (QTabWidgetEventFilter *ef = qobject_cast<QTabWidgetEventFilter*>(o))
                return ef;
    }
    return 0;
}

QMenu *QTabWidgetEventFilter::addTabWidgetContextMenuActions(const QTabWidget *tabWidget, QMenu *popup)
{
    QTabWidgetEventFilter *filter = eventFilterOf(tabWidget);
    if (!filter)
        return 0;
    return filter->addContextMenuActions(popup);
}

QTabBar *QTabWidgetEventFilter::tabBar() const
{
    // QTabWidget::tabBar() accessor is protected, grmbl...
    if (!m_cachedTabBar) {
        const QList<QTabBar *> tabBars = qFindChildren<QTabBar *>(m_tabWidget);
        Q_ASSERT(tabBars.size() == 1);
        m_cachedTabBar = tabBars.front();
    }
    return m_cachedTabBar;

}

static bool canMove(const QPoint &pressPoint, const QMouseEvent *e)
{
    const QPoint pt = pressPoint - e->pos();
    return pt.manhattanLength() > QApplication::startDragDistance();
}

bool QTabWidgetEventFilter::eventFilter(QObject *o, QEvent *e)
{
    const QEvent::Type type = e->type();
    // Do not try to locate tab bar and form window, etc. for uninteresting events and
    // avoid asserts about missing tab bars when being destroyed
    switch (type) {
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::DragLeave:
    case QEvent::DragEnter:
    case QEvent::DragMove:
    case QEvent::Drop:
        break;
    default:
        return false;
    }

    if (o != tabBar())
        return false;

    QDesignerFormWindowInterface *fw = formWindow();
    if (!fw)
        return false;

    switch (type) {
    case QEvent::MouseButtonDblClick:
        break;
    case QEvent::MouseButtonPress: {
        QMouseEvent *mev = static_cast<QMouseEvent*>(e);
        if (QDesignerFormWindowInterface *fw = formWindow()) {
            fw->clearSelection();
            fw->selectWidget(m_tabWidget, true);
        }
        if (mev->button() & Qt::LeftButton) {
            m_mousePressed = true;
            m_pressPoint = mev->pos();

            QTabBar *tabbar = tabBar();
            const int count = tabbar->count();
            for (int i = 0; i < count; ++i) {
                if (tabbar->tabRect(i).contains(m_pressPoint)) {
                    if (i != tabbar->currentIndex()) {
                        qdesigner_internal::SetPropertyCommand *cmd = new qdesigner_internal::SetPropertyCommand(fw);
                        cmd->init(m_tabWidget, QLatin1String("currentIndex"), i);
                        fw->commandHistory()->push(cmd);
                    }
                    break;
                }
            }
        }
    } break;

    case QEvent::MouseButtonRelease:
        m_mousePressed = false;
        break;

    case QEvent::MouseMove: {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
        if (m_mousePressed && canMove(m_pressPoint, mouseEvent)) {
            const int index = m_tabWidget->currentIndex();
            if (index == -1)
                break;

            m_mousePressed = false;
            QDrag *drg = new QDrag(m_tabWidget);
            drg->setMimeData(new qdesigner_internal::MyMimeData(m_tabWidget));

            m_dragIndex = index;
            m_dragPage  = m_tabWidget->currentWidget();
            m_dragLabel = m_tabWidget->tabText(index);
            m_dragIcon  = m_tabWidget->tabIcon(index);
            if (m_dragIcon.isNull()) {
                QLabel *label = new QLabel(m_dragLabel);
                label->adjustSize();
                drg->setPixmap(QPixmap::grabWidget(label));
                label->deleteLater();
            } else {
                drg->setPixmap(m_dragIcon.pixmap(22, 22));
            }

            m_tabWidget->removeTab(m_dragIndex);

            const Qt::DropActions dropAction = drg->start(Qt::MoveAction);

            if (dropAction == Qt::IgnoreAction) {
                // abort
                m_tabWidget->insertTab(m_dragIndex, m_dragPage, m_dragIcon, m_dragLabel);
                m_tabWidget->setCurrentIndex(m_dragIndex);
            }

            if (m_dropIndicator)
                m_dropIndicator->hide();
        }
    } break;

    case QEvent::DragLeave: {
        if (m_dropIndicator)
            m_dropIndicator->hide();
    } break;

    case QEvent::DragEnter:
    case QEvent::DragMove: {
        QDragMoveEvent *de = static_cast<QDragMoveEvent*>(e);
        if (!qdesigner_internal::MyMimeData::fromMyTab(de->mimeData(), m_tabWidget))
            return false;

        if (de->proposedAction() == Qt::MoveAction)
            de->acceptProposedAction();
        else {
            de->setDropAction(Qt::MoveAction);
            de->accept();
        }

        QRect rect;
        const int index = pageFromPosition(de->pos(), rect);

        if (!m_dropIndicator) {
            m_dropIndicator = new QWidget(m_tabWidget);
            QPalette p = m_dropIndicator->palette();
            p.setColor(m_tabWidget->backgroundRole(), Qt::red);
            m_dropIndicator->setPalette(p);
        }

        QPoint pos;
        if (index == m_tabWidget->count())
            pos = tabBar()->mapToParent(QPoint(rect.x() + rect.width(), rect.y()));
        else
            pos = tabBar()->mapToParent(QPoint(rect.x(), rect.y()));

        m_dropIndicator->setGeometry(pos.x(), pos.y() , 3, rect.height());
        m_dropIndicator->show();
    } break;

    case QEvent::Drop: {
        QDropEvent *de = static_cast<QDropEvent*>(e);
        if (!qdesigner_internal::MyMimeData::fromMyTab(de->mimeData(), m_tabWidget))
            return false;
        de->acceptProposedAction();
        de->accept();

        QRect rect;
        const int newIndex = pageFromPosition(de->pos(), rect);

        qdesigner_internal::MoveTabPageCommand *cmd = new qdesigner_internal::MoveTabPageCommand(fw);
        m_tabWidget->insertTab(m_dragIndex, m_dragPage, m_dragIcon, m_dragLabel);
        cmd->init(m_tabWidget, m_dragPage, m_dragIcon, m_dragLabel, m_dragIndex, newIndex);
        fw->commandHistory()->push(cmd);
    } break;

    default:
        break;
    }

    return false;
}

void QTabWidgetEventFilter::removeCurrentPage()
{
    if (!m_tabWidget->currentWidget())
        return;

    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::DeleteTabPageCommand *cmd = new qdesigner_internal::DeleteTabPageCommand(fw);
        cmd->init(m_tabWidget);
        fw->commandHistory()->push(cmd);
    }
}

void QTabWidgetEventFilter::addPage()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::AddTabPageCommand *cmd = new qdesigner_internal::AddTabPageCommand(fw);
        cmd->init(m_tabWidget, qdesigner_internal::AddTabPageCommand::InsertBefore);
        fw->commandHistory()->push(cmd);
    }
}

void QTabWidgetEventFilter::addPageAfter()
{
    if (QDesignerFormWindowInterface *fw = formWindow()) {
        qdesigner_internal::AddTabPageCommand *cmd = new qdesigner_internal::AddTabPageCommand(fw);
        cmd->init(m_tabWidget, qdesigner_internal::AddTabPageCommand::InsertAfter);
        fw->commandHistory()->push(cmd);
    }
}

QDesignerFormWindowInterface *QTabWidgetEventFilter::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(const_cast<QTabWidget*>(m_tabWidget));
}

// Get page from mouse position. Default to new page if in right half of last page?
int QTabWidgetEventFilter::pageFromPosition(const QPoint &pos, QRect &rect) const
{
    int index = 0;
    const QTabBar *tabbar = tabBar();
    const int count = m_tabWidget->count();
    for (; index < count; index++) {
        const QRect rc = tabbar->tabRect(index);
        if (rc.contains(pos)) {
            rect = rc;
            break;
        }
    }

    if (index == count -1) {
        QRect rect2 = rect;
        rect2.setLeft(rect2.left() + rect2.width() / 2);
        if (rect2.contains(pos))
            index++;
    }
    return index;
}

QMenu *QTabWidgetEventFilter::addContextMenuActions(QMenu *popup)
{
    QMenu *pageMenu = 0;
    const int count = m_tabWidget->count();
    m_actionDeletePage->setEnabled(count);
    if (count) {
        const int currentIndex = m_tabWidget->currentIndex();
        const QString pageSubMenuLabel = tr("Page %1 of %2").arg(currentIndex + 1).arg(count);
        pageMenu = popup->addMenu(pageSubMenuLabel);
        pageMenu->addAction(m_actionDeletePage);
        // Set up promotion menu for current widget.
        if (QWidget *page =  m_tabWidget->currentWidget ()) {
            m_pagePromotionTaskMenu->setWidget(page);
            m_pagePromotionTaskMenu->addActions(QDesignerFormWindowInterface::findFormWindow(m_tabWidget),
                                                qdesigner_internal::PromotionTaskMenu::SuppressGlobalEdit,
                                                pageMenu);
        }
        QMenu *insertPageMenu = popup->addMenu(tr("Insert Page"));
        insertPageMenu->addAction(m_actionInsertPageAfter);
        insertPageMenu->addAction(m_actionInsertPage);
    } else {
        QAction *insertPageAction = popup->addAction(tr("Insert Page"));
        connect(insertPageAction, SIGNAL(triggered()), this, SLOT(addPage()));
    }
    popup->addSeparator();
    return pageMenu;
}

// ----------- QTabWidgetPropertySheet

static const char *currentTabTextKey = "currentTabText";
static const char *currentTabNameKey = "currentTabName";
static const char *currentTabIconKey = "currentTabIcon";
static const char *currentTabToolTipKey = "currentTabToolTip";
static const char *currentTabWhatsThisKey = "currentTabWhatsThis";
static const char *tabMovableKey = "movable";

QTabWidgetPropertySheet::QTabWidgetPropertySheet(QTabWidget *object, QObject *parent) :
    QDesignerPropertySheet(object, parent),
    m_tabWidget(object)
{
    createFakeProperty(QLatin1String(currentTabTextKey), qVariantFromValue(qdesigner_internal::PropertySheetStringValue()));
    createFakeProperty(QLatin1String(currentTabNameKey), QString());
    createFakeProperty(QLatin1String(currentTabIconKey), qVariantFromValue(qdesigner_internal::PropertySheetIconValue()));
    if (formWindowBase())
        formWindowBase()->addReloadableProperty(this, indexOf(QLatin1String(currentTabIconKey)));
    createFakeProperty(QLatin1String(currentTabToolTipKey), qVariantFromValue(qdesigner_internal::PropertySheetStringValue()));
    createFakeProperty(QLatin1String(currentTabWhatsThisKey), qVariantFromValue(qdesigner_internal::PropertySheetStringValue()));
    // Prevent the tab widget's drag and drop handling from interfering with Designer's
    createFakeProperty(QLatin1String(tabMovableKey), QVariant(false));
}

QTabWidgetPropertySheet::TabWidgetProperty QTabWidgetPropertySheet::tabWidgetPropertyFromName(const QString &name)
{
    typedef QHash<QString, TabWidgetProperty> TabWidgetPropertyHash;
    static TabWidgetPropertyHash tabWidgetPropertyHash;
    if (tabWidgetPropertyHash.empty()) {
        tabWidgetPropertyHash.insert(QLatin1String(currentTabTextKey),      PropertyCurrentTabText);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabNameKey),      PropertyCurrentTabName);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabIconKey),      PropertyCurrentTabIcon);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabToolTipKey),   PropertyCurrentTabToolTip);
        tabWidgetPropertyHash.insert(QLatin1String(currentTabWhatsThisKey), PropertyCurrentTabWhatsThis);
    }
    return tabWidgetPropertyHash.value(name, PropertyTabWidgetNone);
}

void QTabWidgetPropertySheet::setProperty(int index, const QVariant &value)
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone) {
        QDesignerPropertySheet::setProperty(index, value);
        return;
    }

    // index-dependent
    const int currentIndex = m_tabWidget->currentIndex();
    QWidget *currentWidget = m_tabWidget->currentWidget();
    if (!currentWidget)
        return;

    switch (tabWidgetProperty) {
    case PropertyCurrentTabText:
        m_tabWidget->setTabText(currentIndex, qvariant_cast<QString>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].text = qVariantValue<qdesigner_internal::PropertySheetStringValue>(value);
        break;
    case PropertyCurrentTabName:
        currentWidget->setObjectName(value.toString());
        break;
    case PropertyCurrentTabIcon:
        m_tabWidget->setTabIcon(currentIndex, qvariant_cast<QIcon>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].icon = qVariantValue<qdesigner_internal::PropertySheetIconValue>(value);
        break;
    case PropertyCurrentTabToolTip:
        m_tabWidget->setTabToolTip(currentIndex, qvariant_cast<QString>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].tooltip = qVariantValue<qdesigner_internal::PropertySheetStringValue>(value);
        break;
    case PropertyCurrentTabWhatsThis:
        m_tabWidget->setTabWhatsThis(currentIndex, qvariant_cast<QString>(resolvePropertyValue(index, value)));
        m_pageToData[currentWidget].whatsthis = qVariantValue<qdesigner_internal::PropertySheetStringValue>(value);
        break;
    case PropertyTabWidgetNone:
        break;
    }
}

bool QTabWidgetPropertySheet::isEnabled(int index) const
{
    if (tabWidgetPropertyFromName(propertyName(index)) == PropertyTabWidgetNone)
        return QDesignerPropertySheet::isEnabled(index);
    return m_tabWidget->currentIndex() != -1;
}

QVariant QTabWidgetPropertySheet::property(int index) const
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone)
        return  QDesignerPropertySheet::property(index);

    // index-dependent
    QWidget *currentWidget = m_tabWidget->currentWidget();
    if (!currentWidget) {
        if (tabWidgetProperty == PropertyCurrentTabIcon)
            return qVariantFromValue(qdesigner_internal::PropertySheetIconValue());
        if (tabWidgetProperty == PropertyCurrentTabText)
            return qVariantFromValue(qdesigner_internal::PropertySheetStringValue());
        if (tabWidgetProperty == PropertyCurrentTabToolTip)
            return qVariantFromValue(qdesigner_internal::PropertySheetStringValue());
        if (tabWidgetProperty == PropertyCurrentTabWhatsThis)
            return qVariantFromValue(qdesigner_internal::PropertySheetStringValue());
        return QVariant(QString());
    }

    // index-dependent
    switch (tabWidgetProperty) {
    case PropertyCurrentTabText:
        return qVariantFromValue(m_pageToData.value(currentWidget).text);
    case PropertyCurrentTabName:
        return currentWidget->objectName();
    case PropertyCurrentTabIcon:
        return qVariantFromValue(m_pageToData.value(currentWidget).icon);
    case PropertyCurrentTabToolTip:
        return qVariantFromValue(m_pageToData.value(currentWidget).tooltip);
    case PropertyCurrentTabWhatsThis:
        return qVariantFromValue(m_pageToData.value(currentWidget).whatsthis);
    case PropertyTabWidgetNone:
        break;
    }
    return QVariant();
}

bool QTabWidgetPropertySheet::reset(int index)
{
    const TabWidgetProperty tabWidgetProperty = tabWidgetPropertyFromName(propertyName(index));
    if (tabWidgetProperty == PropertyTabWidgetNone)
        return QDesignerPropertySheet::reset(index);

    // index-dependent
    QWidget *currentWidget = m_tabWidget->currentWidget();
    if (!currentWidget)
        return false;

    // index-dependent
    switch (tabWidgetProperty) {
    case PropertyCurrentTabName:
        setProperty(index, QString());
        break;
    case PropertyCurrentTabToolTip:
        m_pageToData[currentWidget].tooltip = qdesigner_internal::PropertySheetStringValue();
        setProperty(index, QString());
        break;
    case PropertyCurrentTabWhatsThis:
        m_pageToData[currentWidget].whatsthis = qdesigner_internal::PropertySheetStringValue();
        setProperty(index, QString());
        break;
    case PropertyCurrentTabText:
        m_pageToData[currentWidget].text = qdesigner_internal::PropertySheetStringValue();
        setProperty(index, QString());
        break;
    case PropertyCurrentTabIcon:
        m_pageToData[currentWidget].icon = qdesigner_internal::PropertySheetIconValue();
        setProperty(index, QIcon());
        break;
    case PropertyTabWidgetNone:
        break;
    }
    return true;
}

bool QTabWidgetPropertySheet::checkProperty(const QString &propertyName)
{
    switch (tabWidgetPropertyFromName(propertyName)) {
    case PropertyCurrentTabText:
    case PropertyCurrentTabName:
    case PropertyCurrentTabToolTip:
    case PropertyCurrentTabWhatsThis:
    case PropertyCurrentTabIcon:
        return false;
    default:
        break;
    }
    return true;
}

QT_END_NAMESPACE

#include "qdesigner_tabwidget.moc" // required for MyMimeData
