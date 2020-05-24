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

#include "zoomwidget_p.h"

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsProxyWidget>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QScrollBar>

#include <QtCore/QTextStream>
#include <QtCore/qmath.h>
#include <QtCore/QDebug>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

typedef QList<QAction*> ActionList;
typedef QList<QGraphicsItem *> GraphicsItemList;

enum { debugZoomWidget = 0 };

static const int menuZoomList[] = { 100, 25, 50, 75, 125, 150 , 175, 200 };

static inline QSize qCeiling(const QSizeF &s)
{
    return QSize(qCeil(s.width()), qCeil(s.height()));
}

namespace qdesigner_internal {

// ---------- ZoomMenu

ZoomMenu::ZoomMenu(QObject *parent) :
   QObject(parent),
   m_menuActions(new QActionGroup(this))
{
    connect(m_menuActions, SIGNAL(triggered(QAction*)), this, SLOT(slotZoomMenu(QAction*)));
    const int nz = sizeof(menuZoomList)/sizeof(int);
    for (int i = 0; i < nz; i++) {
        const int zoom = menuZoomList[i];
        //: Zoom factor
        QAction *a = m_menuActions->addAction(tr("%1 %").arg(zoom));
        a->setCheckable(true);
        a->setData(QVariant(zoom));
        if (zoom == 100)
            a->setChecked(true);
        m_menuActions->addAction(a);
    }
}

int ZoomMenu::zoomOf(const QAction *a)
{
    return a->data().toInt();
}

void ZoomMenu::addActions(QMenu *m)
{
    const ActionList za = m_menuActions->actions();
    const ActionList::const_iterator cend = za.constEnd();
    for (ActionList::const_iterator it =  za.constBegin(); it != cend; ++it) {
        m->addAction(*it);
        if (zoomOf(*it)  == 100)
            m->addSeparator();
    }
}

int ZoomMenu::zoom() const
{
    return m_menuActions->checkedAction()->data().toInt();
}

void ZoomMenu::setZoom(int percent)
{
    const ActionList za = m_menuActions->actions();
    const ActionList::const_iterator cend = za.constEnd();
    for (ActionList::const_iterator it =  za.constBegin(); it != cend; ++it)
        if (zoomOf(*it) == percent) {
            (*it)->setChecked(true);
            return;
        }
}

void ZoomMenu::slotZoomMenu(QAction *a)
{
    emit zoomChanged(zoomOf(a));
}

QList<int> ZoomMenu::zoomValues()
{
    QList<int> rc;
    const int nz = sizeof(menuZoomList)/sizeof(int);
    for (int i = 0; i < nz; i++)
        rc.push_back(menuZoomList[i]);
    return rc;
}

// --------- ZoomView
ZoomView::ZoomView(QWidget *parent) :
    QGraphicsView(parent),
    m_scene(new QGraphicsScene(this)),
    m_zoom(100),
    m_zoomFactor(1.0),
    m_zoomContextMenuEnabled(false),
    m_zoomMenu(0)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setScene(m_scene);
    if (debugZoomWidget)
        qDebug() << "scene" << m_scene->sceneRect();

}

int ZoomView::zoom() const
{
    return m_zoom;
}

void ZoomView::scrollToOrigin()
{
    const QPoint origin(0 ,0);
    const QPoint current = scrollPosition();
    if (current != origin) {
        if (debugZoomWidget)
            qDebug() << "ZoomView::scrollToOrigin from " << current;
        setScrollPosition(origin);
    }
}

void ZoomView::setZoom(int percent)
{
    if (debugZoomWidget)
        qDebug() << "ZoomView::setZoom" << percent;

    if (m_zoom == percent)
        return;

    m_zoom = percent;
    const qreal hundred = 100.0;
    m_zoomFactor = static_cast<qreal>(m_zoom) / hundred;

    applyZoom();
    if (m_zoomMenu) // Do not force them into existence
        m_zoomMenu->setZoom(m_zoom);

    resetTransform();
    scale(m_zoomFactor, m_zoomFactor);
}

void ZoomView::applyZoom()
{
}

qreal ZoomView::zoomFactor() const
{
    return m_zoomFactor;
}

bool ZoomView::isZoomContextMenuEnabled() const
{
    return m_zoomContextMenuEnabled;
}

void ZoomView::setZoomContextMenuEnabled(bool e)
{
    m_zoomContextMenuEnabled = e;
}

ZoomMenu *ZoomView::zoomMenu()
{
    if (!m_zoomMenu) {
        m_zoomMenu = new ZoomMenu(this);
        m_zoomMenu->setZoom(m_zoom);
        connect(m_zoomMenu, SIGNAL(zoomChanged(int)), this, SLOT(setZoom(int)));
    }
    return m_zoomMenu;
}

void ZoomView::contextMenuEvent(QContextMenuEvent *event)
{
    if (debugZoomWidget > 1)
        qDebug() << "ZoomView::contextMenuEvent" << event->pos() << event->globalPos() << zoom() << '%';

    if (m_zoomContextMenuEnabled) {
        showContextMenu(event->globalPos());
    } else {
        QGraphicsView::contextMenuEvent(event);
    }
}

void ZoomView::showContextMenu(const QPoint &globalPos)
{
    QMenu menu;
    zoomMenu()->addActions(&menu);
    if (debugZoomWidget) {
        menu.addSeparator();
        QAction *da = menu.addAction(QLatin1String("Dump"));
        connect(da, SIGNAL(triggered()), this, SLOT(dump()));
    }
    menu.exec(globalPos);
}

QPoint ZoomView::scrollPosition() const
{
    return QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
}

void ZoomView::setScrollPosition(const QPoint& pos)
{
    horizontalScrollBar()->setValue(pos.x());
    verticalScrollBar()->setValue(pos.y());
}

// -------------- ZoomProxyWidget
ZoomProxyWidget::ZoomProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags) :
    QGraphicsProxyWidget(parent, wFlags)
{
}

QVariant ZoomProxyWidget::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionChange: {
        const QPointF newPos = value.toPointF();
        const QPointF desiredPos = QPointF(0, 0);
        if (newPos != desiredPos && debugZoomWidget)
            qDebug() << "ZoomProxyWidget::itemChange: refusing " << newPos;
        return desiredPos;
    }
    default:
        break;
    }
    return QGraphicsProxyWidget::itemChange(change, value);
}

/* ZoomedEventFilterRedirector: Event filter for the zoomed widget.
 * It redirects the events to another handler of ZoomWidget as its
 * base class QScrollArea also implements eventFilter() for its viewport. */

static const char *zoomedEventFilterRedirectorNameC = "__qt_ZoomedEventFilterRedirector";

class ZoomedEventFilterRedirector : public QObject {
    Q_DISABLE_COPY(ZoomedEventFilterRedirector)

public:
    explicit ZoomedEventFilterRedirector(ZoomWidget *zw, QObject *parent);
    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    ZoomWidget *m_zw;
};

ZoomedEventFilterRedirector::ZoomedEventFilterRedirector(ZoomWidget *zw, QObject *parent) :
    QObject(parent),
    m_zw(zw)
{
    setObjectName(QLatin1String(zoomedEventFilterRedirectorNameC));
}

bool ZoomedEventFilterRedirector::eventFilter(QObject *watched, QEvent *event)
{
    return m_zw->zoomedEventFilter(watched, event);
}


// --------- ZoomWidget

ZoomWidget::ZoomWidget(QWidget *parent) :
    ZoomView(parent),
    m_proxy(0),
    m_viewResizeBlocked(false),
    m_widgetResizeBlocked(false),
    m_widgetZoomContextMenuEnabled(false)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void ZoomWidget::setWidget(QWidget *w, Qt::WindowFlags wFlags)
{
    if (debugZoomWidget)
        qDebug() << "ZoomWidget::setWidget" << w << bin << wFlags;

    if (m_proxy) {
        scene().removeItem(m_proxy);
        if (QWidget *w = m_proxy->widget()) {
            // remove the event filter
            if (QObject *evf =  qFindChild<QObject*>(w, QLatin1String(zoomedEventFilterRedirectorNameC)))
                w->removeEventFilter(evf);
        }
        m_proxy->deleteLater();
    }
    // Set window flags on the outer proxy for them to take effect
    m_proxy = createProxyWidget(0, Qt::Window);
    m_proxy->setWidget(w);

    m_proxy->setWindowFlags(wFlags);
    scene().addItem(m_proxy);
    w->installEventFilter(new ZoomedEventFilterRedirector(this, w));
    resizeToWidgetSize(); // Do manually for new widget
    m_proxy->show();
}

bool ZoomWidget::isWidgetZoomContextMenuEnabled() const
{
    return m_widgetZoomContextMenuEnabled;
}
void ZoomWidget::setWidgetZoomContextMenuEnabled(bool e)
{
    m_widgetZoomContextMenuEnabled = e;
}

QSize ZoomWidget::viewPortMargin() const
{
    return QSize(0, 0);
}

QSizeF ZoomWidget::widgetDecorationSizeF() const
{
    qreal left, top, right, bottom;
    m_proxy->getWindowFrameMargins (&left, &top, &right, &bottom);
    const QSizeF rc = QSizeF(left + right, top + bottom);
    return rc;
}

QSize ZoomWidget::widgetSize() const
{
    if (m_proxy)
        return m_proxy->widget()->size();
    return QSize(0, 0);
}

/* Convert widget size to QGraphicsView size.
 * Watch out for limits (0, QWIDGETSIZE_MAX); just pass them on */

QSize ZoomWidget::widgetSizeToViewSize(const QSize &s, bool *ptrToValid) const
{
    const QSize vpMargin = viewPortMargin();
    const QSizeF deco = widgetDecorationSizeF();
    const int width = s.width();

    QSize rc = s;
    bool valid = false;
    if (width != 0 && width != QWIDGETSIZE_MAX) {
        valid = true;
        rc.setWidth(vpMargin.width() + qCeil(deco.width() + zoomFactor() * static_cast<qreal>(width)));
    }

    const int height = s.height();
    if (height != 0 && height != QWIDGETSIZE_MAX) {
        valid = true;
        rc.setHeight(vpMargin.height() + qCeil(deco.height() + zoomFactor() * static_cast<qreal>(height)));
    }

    if (ptrToValid)
        *ptrToValid = valid;

    return rc;
}

// On changing zoom: Make QGraphicsView big enough to hold the widget
void ZoomWidget::resizeToWidgetSize()
{
    if (!m_proxy)
        return;

    m_viewResizeBlocked = true;
    // Convert size, apply transformed min/max size if applicable
    const QSize wsize = widgetSize();
    const QSize viewSize = widgetSizeToViewSize(wsize);

    bool hasMinimumSize = false;
    const QSize minimumSize = m_proxy->widget()->minimumSize();
    const QSize viewMinimumSize = widgetSizeToViewSize(minimumSize, &hasMinimumSize);

    bool hasMaximumSize = false;
    const QSize maximumSize = m_proxy->widget()->maximumSize();
    const QSize viewMaximumSize = widgetSizeToViewSize(maximumSize, &hasMaximumSize);

  if (debugZoomWidget) {
        qDebug()
            << "ZoomWidget::resizeToWidgetSize()\n"
            << "Widget: " <<  wsize << "(scaled)"  << (wsize * zoomFactor()) << " Min/Max" << minimumSize  <<  maximumSize << '\n'
            << "  View: " << viewSize <<  hasMinimumSize << viewMinimumSize << hasMaximumSize << viewMaximumSize;
    }
    // Apply
    if (hasMinimumSize)
        setMinimumSize(viewMinimumSize);
    if (hasMaximumSize)
        setMaximumSize(viewMaximumSize);
    // now resize
    doResize(viewSize);
    if (debugZoomWidget)
        qDebug() << "ZoomWidget::resizeToWidgetSize(): resulting view size" << size();
    m_viewResizeBlocked = false;
}

void ZoomWidget::applyZoom()
{
    resizeToWidgetSize();
}

/* virtual */ void  ZoomWidget::doResize(const QSize &s)
{
    if (debugZoomWidget > 1)
        qDebug() << ">ZoomWidget::doResize() " << s;
    resize(s);
}

void ZoomWidget::resizeEvent(QResizeEvent *event)
{
    /* QGraphicsView Resized from outside: Adapt widget. For some reason,
     * the size passed in the event is not to be trusted. This might be due
     * to some QScrollArea event fiddling. Have QScrollArea resize first
     * and the use the size ZoomView::resizeEvent(event); */
    if (m_proxy && !m_viewResizeBlocked) {
        if (debugZoomWidget > 1)
            qDebug() << '>' << Q_FUNC_INFO << size() << ")::resizeEvent from " << event->oldSize() << " to " << event->size();
        const QSizeF newViewPortSize = size() - viewPortMargin();
        const QSizeF widgetSizeF = newViewPortSize / zoomFactor() - widgetDecorationSizeF();
        m_widgetResizeBlocked = true;
        m_proxy->widget()->resize(widgetSizeF.toSize());
        setSceneRect(QRectF(QPointF(0, 0), widgetSizeF));
        scrollToOrigin();
        m_widgetResizeBlocked = false;
        if (debugZoomWidget > 1)
            qDebug() << '<' << Q_FUNC_INFO << widgetSizeF << m_proxy->widget()->size() << m_proxy->size();
    }
}

QSize ZoomWidget::minimumSizeHint() const
{
    if (!m_proxy)
        return QGraphicsView::minimumSizeHint();

    const QSizeF wmsh = m_proxy->widget()->minimumSizeHint();
    const QSize rc = viewPortMargin() + (wmsh * zoomFactor()).toSize();
    if (debugZoomWidget > 1)
        qDebug() << "minimumSizeHint()" << rc;
    return rc;
}

QSize ZoomWidget::sizeHint() const
{
    if (!m_proxy)
        return QGraphicsView::sizeHint();

    const QSizeF wsh = m_proxy->widget()->sizeHint();
    const QSize rc = viewPortMargin() + (wsh * zoomFactor()).toSize();
    if (debugZoomWidget > 1)
        qDebug() << "sizeHint()" << rc;
    return rc;
}

bool ZoomWidget::zoomedEventFilter(QObject * /*watched*/, QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
        if (debugZoomWidget) { // Debug helper: Press 'D' on the zoomed widget
            const QKeyEvent *kevent = static_cast<QKeyEvent*>(event);
            if (kevent->key() == Qt::Key_D)
                dump();
        }
        break;
    case QEvent::Resize:
        if (debugZoomWidget > 1) {
            const QResizeEvent *re = static_cast<const QResizeEvent *>(event);
            qDebug() << "ZoomWidget::zoomedEventFilter" << re->oldSize() << re->size() << " at " << m_proxy->widget()->geometry();
        }
        if (!m_widgetResizeBlocked)
            resizeToWidgetSize();

        break;
    case QEvent::ContextMenu:
        if (m_widgetZoomContextMenuEnabled) {
            // Calculate global position from scaled
            QContextMenuEvent *ce = static_cast<QContextMenuEvent*>(event);
            const QPointF origin = mapToGlobal(QPoint(0, 0)) - scrollPosition();
            const QPointF pos = QPointF(origin + (QPointF(ce->pos()) * zoomFactor()));
            showContextMenu(pos.toPoint());
            ce->accept();
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void ZoomWidget::setItemAcceptDrops(bool)
{
    if (m_proxy)
        m_proxy->setAcceptDrops(true);
}

bool ZoomWidget::itemAcceptDrops() const
{
    return m_proxy ? m_proxy->acceptDrops() : false;
}

       // Factory function for QGraphicsProxyWidgets which can be overwritten. Default creates a ZoomProxyWidget
QGraphicsProxyWidget *ZoomWidget::createProxyWidget(QGraphicsItem *parent, Qt::WindowFlags wFlags) const
{
    return new ZoomProxyWidget(parent, wFlags);
}

void ZoomWidget::dump() const
{

    qDebug() << "ZoomWidget::dump " << geometry() << " Viewport " << viewport()->geometry()
        << "Scroll: " << scrollPosition() << "Matrix: " << matrix() << " SceneRect: " << sceneRect();
    if (m_proxy) {
        qDebug() << "Proxy Pos: " << m_proxy->pos() << "Proxy " << m_proxy->size()
            << "\nProxy size hint"
            <<  m_proxy->effectiveSizeHint(Qt::MinimumSize)
            <<  m_proxy->effectiveSizeHint(Qt::PreferredSize)
            << m_proxy->effectiveSizeHint(Qt::MaximumSize)
            << "\nMatrix: " << m_proxy->matrix()
            << "\nWidget: " <<  m_proxy->widget()->geometry()
            << "scaled" << (zoomFactor() * m_proxy->widget()->size());
    }
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
