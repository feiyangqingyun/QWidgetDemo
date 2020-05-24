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


#include "connectionedit_p.h"

#include <QtDesigner/abstractformwindow.h>

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>
#include <QtGui/QMatrix>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QAction>

#include <QtCore/QMultiMap>

QT_BEGIN_NAMESPACE

static const int BG_ALPHA =              32;
static const int LINE_PROXIMITY_RADIUS =  3;
static const int LOOP_MARGIN  =          20;
static const int VLABEL_MARGIN =          1;
static const int HLABEL_MARGIN =          3;
static const int GROUND_W =              20;
static const int GROUND_H =              25;

/*******************************************************************************
** Tools
*/

static QRect fixRect(const QRect &r)
{
    return QRect(r.x(), r.y(), r.width() - 1, r.height() - 1);
}

static QRect expand(const QRect &r, int i)
{
    return QRect(r.x() - i, r.y() - i, r.width() + 2*i, r.height() + 2*i);
}

static QRect endPointRectHelper(const QPoint &pos)
{
    const QRect r(pos + QPoint(-LINE_PROXIMITY_RADIUS, -LINE_PROXIMITY_RADIUS),
                  QSize(2*LINE_PROXIMITY_RADIUS, 2*LINE_PROXIMITY_RADIUS));
    return r;
}

static void paintGround(QPainter *p, QRect r)
{
    const QPoint mid = r.center();
    p->drawLine(mid.x(), r.top(), mid.x(), mid.y());
    p->drawLine(r.left(), mid.y(), r.right(), mid.y());
    int y = r.top() + 4*r.height()/6;
    int x = GROUND_W/6;
    p->drawLine(r.left() + x, y, r.right() - x, y);
    y = r.top() + 5*r.height()/6;
    x = 2*GROUND_W/6;
    p->drawLine(r.left() + x, y, r.right() - x, y);
    p->drawLine(mid.x(), r.bottom(), mid.x() + 1, r.bottom());
}

static void paintEndPoint(QPainter *p, const QPoint &pos)
{
    const QRect r(pos + QPoint(-LINE_PROXIMITY_RADIUS, -LINE_PROXIMITY_RADIUS),
                  QSize(2*LINE_PROXIMITY_RADIUS, 2*LINE_PROXIMITY_RADIUS));
    p->fillRect(fixRect(r), p->pen().color());
}

static qdesigner_internal::CETypes::LineDir classifyLine(const QPoint &p1, const QPoint &p2)
{
    if (p1.x() == p2.x())
        return p1.y() < p2.y() ? qdesigner_internal::CETypes::DownDir : qdesigner_internal::CETypes::UpDir;
    Q_ASSERT(p1.y() == p2.y());
    return p1.x() < p2.x() ? qdesigner_internal::CETypes::RightDir : qdesigner_internal::CETypes::LeftDir;
}

static QPoint pointInsideRect(const QRect &r, QPoint p)
{
    if (p.x() < r.left())
        p.setX(r.left());
    else if (p.x() > r.right())
        p.setX(r.right());

    if (p.y() < r.top())
        p.setY(r.top());
    else if (p.y() > r.bottom())
        p.setY(r.bottom());

    return p;
}

namespace qdesigner_internal {

/*******************************************************************************
** Commands
*/

AddConnectionCommand::AddConnectionCommand(ConnectionEdit *edit, Connection *con)
    : CECommand(edit), m_con(con)
{
    setText(QApplication::translate("Command", "Add connection"));
}

void AddConnectionCommand::redo()
{
    edit()->selectNone();
    emit edit()->aboutToAddConnection(edit()->m_con_list.size());
    edit()->m_con_list.append(m_con);
    m_con->inserted();
    edit()->setSelected(m_con, true);
    emit edit()->connectionAdded(m_con);
}

void AddConnectionCommand::undo()
{
    const int idx = edit()->indexOfConnection(m_con);
    emit edit()->aboutToRemoveConnection(m_con);
    edit()->setSelected(m_con, false);
    m_con->update();
    m_con->removed();
    edit()->m_con_list.removeAll(m_con);
    emit edit()->connectionRemoved(idx);
}

class AdjustConnectionCommand : public CECommand
{
public:
    AdjustConnectionCommand(ConnectionEdit *edit, Connection *con,
                            const QPoint &old_source_pos,
                            const QPoint &old_target_pos,
                            const QPoint &new_source_pos,
                            const QPoint &new_target_pos);
    virtual void redo();
    virtual void undo();
private:
    Connection *m_con;
    const QPoint m_old_source_pos;
    const QPoint m_old_target_pos;
    const QPoint m_new_source_pos;
    const QPoint m_new_target_pos;
};

AdjustConnectionCommand::AdjustConnectionCommand(ConnectionEdit *edit, Connection *con,
                                                    const QPoint &old_source_pos,
                                                    const QPoint &old_target_pos,
                                                    const QPoint &new_source_pos,
                                                    const QPoint &new_target_pos) :
    CECommand(edit),
    m_con(con),
    m_old_source_pos(old_source_pos),
    m_old_target_pos(old_target_pos),
    m_new_source_pos(new_source_pos),
    m_new_target_pos(new_target_pos)
{
    setText(QApplication::translate("Command", "Adjust connection"));
}

void AdjustConnectionCommand::undo()
{
    m_con->setEndPoint(EndPoint::Source, m_con->widget(EndPoint::Source), m_old_source_pos);
    m_con->setEndPoint(EndPoint::Target, m_con->widget(EndPoint::Target), m_old_target_pos);
}

void AdjustConnectionCommand::redo()
{
    m_con->setEndPoint(EndPoint::Source, m_con->widget(EndPoint::Source), m_new_source_pos);
    m_con->setEndPoint(EndPoint::Target, m_con->widget(EndPoint::Target), m_new_target_pos);
}

DeleteConnectionsCommand::DeleteConnectionsCommand(ConnectionEdit *edit,
                                                    const ConnectionList &con_list)
    : CECommand(edit), m_con_list(con_list)
{
   setText(QApplication::translate("Command", "Delete connections"));
}

void DeleteConnectionsCommand::redo()
{
    foreach (Connection *con, m_con_list) {
        const int idx = edit()->indexOfConnection(con);
        emit edit()->aboutToRemoveConnection(con);
        Q_ASSERT(edit()->m_con_list.contains(con));
        edit()->setSelected(con, false);
        con->update();
        con->removed();
        edit()->m_con_list.removeAll(con);
        emit edit()->connectionRemoved(idx);
    }
}

void DeleteConnectionsCommand::undo()
{
    foreach (Connection *con, m_con_list) {
        Q_ASSERT(!edit()->m_con_list.contains(con));
        emit edit()->aboutToAddConnection(edit()->m_con_list.size());
        edit()->m_con_list.append(con);
        edit()->setSelected(con, true);
        con->update();
        con->inserted();
        emit edit()->connectionAdded(con);
    }
}

class SetEndPointCommand : public CECommand
{
public:
    SetEndPointCommand(ConnectionEdit *edit, Connection *con, EndPoint::Type type, QObject *object);
    virtual void redo();
    virtual void undo();
private:
    Connection *m_con;
    const EndPoint::Type m_type;
    QObject *m_old_widget, *m_new_widget;
    const QPoint m_old_pos;
    QPoint m_new_pos;
};

SetEndPointCommand::SetEndPointCommand(ConnectionEdit *edit, Connection *con,
                                        EndPoint::Type type, QObject *object) :
    CECommand(edit),
    m_con(con),
    m_type(type),
    m_old_widget(con->object(type)),
    m_new_widget(object),
    m_old_pos(con->endPointPos(type))
{
    if (QWidget *widget = qobject_cast<QWidget*>(object)) {
        m_new_pos = edit->widgetRect(widget).center();
    }

    if (m_type == EndPoint::Source)
        setText(QApplication::translate("Command", "Change source"));
    else
        setText(QApplication::translate("Command", "Change target"));
}

void SetEndPointCommand::redo()
{
    m_con->setEndPoint(m_type, m_new_widget, m_new_pos);
    emit edit()->connectionChanged(m_con);
}

void SetEndPointCommand::undo()
{
    m_con->setEndPoint(m_type, m_old_widget, m_old_pos);
    emit edit()->connectionChanged(m_con);
}

/*******************************************************************************
** Connection
*/

Connection::Connection(ConnectionEdit *edit) :
    m_source_pos(QPoint(-1, -1)),
    m_target_pos(QPoint(-1, -1)),
    m_source(0),
    m_target(0),
    m_edit(edit),
    m_visible(true)
{

}

Connection::Connection(ConnectionEdit *edit, QObject *source, QObject *target) :
    m_source_pos(QPoint(-1, -1)),
    m_target_pos(QPoint(-1, -1)),
    m_source(source),
    m_target(target),
    m_edit(edit),
    m_visible(true)
{
}

void Connection::setVisible(bool b)
{
    m_visible = b;
}

void Connection::updateVisibility()
{
    QWidget *source = widget(EndPoint::Source);
    QWidget *target = widget(EndPoint::Target);

    if (source == 0 || target == 0) {
        setVisible(false);
        return;
    }

    QWidget *w = source;
    while (w && w->parentWidget()) {
        if (!w->isVisibleTo(w->parentWidget())) {
            setVisible(false);
            return;
        }
        w = w->parentWidget();
    }

    w = target;
    while (w && w->parentWidget()) {
        if (!w->isVisibleTo(w->parentWidget())) {
            setVisible(false);
            return;
        }
        w = w->parentWidget();
    }

    setVisible(true);
}

bool Connection::isVisible() const
{
    return m_visible;
}

bool Connection::ground() const
{
    return m_target != 0 && m_target == m_edit->m_bg_widget;
}

QPoint Connection::endPointPos(EndPoint::Type type) const
{
    if (type == EndPoint::Source)
        return m_source_pos;
    else
        return m_target_pos;
}

static QPoint lineEntryPos(const QPoint &p1, const QPoint &p2, const QRect &rect)
{
    QPoint result;

    switch (classifyLine(p1, p2)) {
        case CETypes::UpDir:
            result = QPoint(p1.x(), rect.bottom());
            break;
        case CETypes::DownDir:
            result = QPoint(p1.x(), rect.top());
            break;
        case CETypes::LeftDir:
            result = QPoint(rect.right(), p1.y());
            break;
        case CETypes::RightDir:
            result = QPoint(rect.left(), p1.y());
            break;
    }

    return result;
}

static QPolygonF arrowHead(const QPoint &p1, const QPoint &p2)
{
    QPolygonF result;

    switch (classifyLine(p1, p2)) {
        case CETypes::UpDir:
            result.append(p2 + QPoint(0, 1));
            result.append(p2 + QPoint(LINE_PROXIMITY_RADIUS, LINE_PROXIMITY_RADIUS*2 + 1));
            result.append(p2 + QPoint(-LINE_PROXIMITY_RADIUS, LINE_PROXIMITY_RADIUS*2 + 1));
            break;
        case CETypes::DownDir:
            result.append(p2);
            result.append(p2 + QPoint(LINE_PROXIMITY_RADIUS, -LINE_PROXIMITY_RADIUS*2));
            result.append(p2 + QPoint(-LINE_PROXIMITY_RADIUS, -LINE_PROXIMITY_RADIUS*2));
            break;
        case CETypes::LeftDir:
            result.append(p2 + QPoint(1, 0));
            result.append(p2 + QPoint(2*LINE_PROXIMITY_RADIUS + 1, -LINE_PROXIMITY_RADIUS));
            result.append(p2 + QPoint(2*LINE_PROXIMITY_RADIUS + 1, LINE_PROXIMITY_RADIUS));
            break;
        case CETypes::RightDir:
            result.append(p2);
            result.append(p2 + QPoint(-2*LINE_PROXIMITY_RADIUS, -LINE_PROXIMITY_RADIUS));
            result.append(p2 + QPoint(-2*LINE_PROXIMITY_RADIUS, LINE_PROXIMITY_RADIUS));
            break;
    }

    return result;
}

static CETypes::LineDir closestEdge(const QPoint &p, const QRect &r)
{
    CETypes::LineDir result = CETypes::UpDir;
    int min = p.y() - r.top();

    int d = p.x() - r.left();
    if (d < min) {
        min = d;
        result = CETypes::LeftDir;
    }

    d = r.bottom() - p.y();
    if (d < min) {
        min = d;
        result = CETypes::DownDir;
    }

    d = r.right() - p.x();
    if (d < min) {
        min = d;
        result = CETypes::RightDir;
    }

    return result;
}

static bool pointAboveLine(const QPoint &l1, const QPoint &l2, const QPoint &p)
{
    if (l1.x() == l2.x())
        return p.x() >= l1.x();
    return p.y() <= l1.y() + (p.x() - l1.x())*(l2.y() - l1.y())/(l2.x() - l1.x());
}

void Connection::updateKneeList()
{
    const LineDir old_source_label_dir = labelDir(EndPoint::Source);
    const LineDir old_target_label_dir = labelDir(EndPoint::Target);

    QPoint s = endPointPos(EndPoint::Source);
    QPoint t = endPointPos(EndPoint::Target);
    const QRect sr = m_source_rect;
    const QRect tr = m_target_rect;

    m_knee_list.clear();
    m_arrow_head.clear();

    if (m_source == 0 || s == QPoint(-1, -1) || t == QPoint(-1, -1))
        return;

    const QRect r = sr | tr;

    m_knee_list.append(s);
    if (m_target == 0) {
        m_knee_list.append(QPoint(t.x(), s.y()));
    } else if (m_target == m_edit->m_bg_widget) {
        m_knee_list.append(QPoint(s.x(), t.y()));
    } else if (tr.contains(sr) || sr.contains(tr)) {
/*
        +------------------+
        | +----------+     |
        | |          |     |
        | |   o      |     |
        | +---|------+     |
        |     |     x      |
        +-----|-----|------+
              +-----+

        We find out which edge of the outer rectangle is closest to the target
        point, and make a loop which exits and re-enters through that edge.
*/
        const LineDir dir = closestEdge(t, tr);
        switch (dir) {
            case UpDir:
                m_knee_list.append(QPoint(s.x(), r.top() - LOOP_MARGIN));
                m_knee_list.append(QPoint(t.x(), r.top() - LOOP_MARGIN));
                break;
            case DownDir:
                m_knee_list.append(QPoint(s.x(), r.bottom() + LOOP_MARGIN));
                m_knee_list.append(QPoint(t.x(), r.bottom() + LOOP_MARGIN));
                break;
            case LeftDir:
                m_knee_list.append(QPoint(r.left() - LOOP_MARGIN, s.y()));
                m_knee_list.append(QPoint(r.left() - LOOP_MARGIN, t.y()));
                break;
            case RightDir:
                m_knee_list.append(QPoint(r.right() + LOOP_MARGIN, s.y()));
                m_knee_list.append(QPoint(r.right() + LOOP_MARGIN, t.y()));
                break;
        }
    } else {
        if (r.height() < sr.height() + tr.height()) {
            if ((s.y() >= tr.top() && s.y() <= tr.bottom()) || (t.y() >= sr.bottom() || t.y() <= sr.top())) {
/*
                +--------+
                |        |   +--------+
                |     o--+---+--x     |
                |     o  |   |        |
                +-----|--+   |        |
                      +------+--x     |
                             +--------+

                When dragging one end point, move the other end point to the same y position,
                if that does not cause it to exit it's rectangle.
*/
                if (m_edit->state() == ConnectionEdit::Dragging) {
                    if (m_edit->m_drag_end_point.type == EndPoint::Source) {
                        const QPoint p(t.x(), s.y());
                        m_knee_list.append(p);
                        if (tr.contains(p))
                            t = m_target_pos = p;
                    } else {
                        const QPoint p(s.x(), t.y());
                        m_knee_list.append(p);
                        if (sr.contains(p))
                            s = m_source_pos = p;
                    }
                } else {
                    m_knee_list.append(QPoint(s.x(), t.y()));
                }
            } else {
/*
                +--------+
                |   o----+-------+
                |        |   +---|----+
                +--------+   |   |    |
                             |   x    |
                             +--------+
*/
                m_knee_list.append(QPoint(t.x(), s.y()));
            }
        } else if (r.width() < sr.width() + tr.width()) {
            if ((s.x() >= tr.left() && s.x() <= tr.right()) || t.x() >= sr.right() || t.x() <= sr.left()) {
/*
                +--------+
                |        |
                |    o  o+--+
                +----|---+  |
                   +-|------|-+
                   | x      x |
                   |          |
                   +----------+

                When dragging one end point, move the other end point to the same x position,
                if that does not cause it to exit it's rectangle.
*/
                if (m_edit->state() == ConnectionEdit::Dragging) {
                    if (m_edit->m_drag_end_point.type == EndPoint::Source) {
                        const QPoint p(s.x(), t.y());
                        m_knee_list.append(p);
                        if (tr.contains(p))
                            t = m_target_pos = p;
                    } else {
                        const QPoint p(t.x(), s.y());
                        m_knee_list.append(p);
                        if (sr.contains(p))
                            s = m_source_pos = p;
                    }
                } else {
                    m_knee_list.append(QPoint(t.x(), s.y()));
                }
            } else {
/*
                +--------+
                |        |
                |  o     |
                +--|-----+
                   |   +--------+
                   +---+-x      |
                       |        |
                       +--------+

*/
                m_knee_list.append(QPoint(s.x(), t.y()));
            }
        } else {
/*
            +--------+
            |        |
            |  o   o-+--------+
            +--|-----+        |
               |        +-----|--+
               |        |     x  |
               +--------+-x      |
                        +--------+

            The line enters the target rectangle through the closest edge.
*/
            if (sr.topLeft() == r.topLeft()) {
                if (pointAboveLine(tr.topLeft(), tr.bottomRight(), t))
                    m_knee_list.append(QPoint(t.x(), s.y()));
                else
                    m_knee_list.append(QPoint(s.x(), t.y()));
            } else if (sr.topRight() == r.topRight()) {
                if (pointAboveLine(tr.bottomLeft(), tr.topRight(), t))
                    m_knee_list.append(QPoint(t.x(), s.y()));
                else
                    m_knee_list.append(QPoint(s.x(), t.y()));
            } else if (sr.bottomRight() == r.bottomRight()) {
                if (pointAboveLine(tr.topLeft(), tr.bottomRight(), t))
                    m_knee_list.append(QPoint(s.x(), t.y()));
                else
                    m_knee_list.append(QPoint(t.x(), s.y()));
            } else {
                if (pointAboveLine(tr.bottomLeft(), tr.topRight(), t))
                    m_knee_list.append(QPoint(s.x(), t.y()));
                else
                    m_knee_list.append(QPoint(t.x(), s.y()));
            }
        }
    }
    m_knee_list.append(t);

    if (m_knee_list.size() == 2)
        m_knee_list.clear();

    trimLine();

    const LineDir new_source_label_dir = labelDir(EndPoint::Source);
    const LineDir new_target_label_dir = labelDir(EndPoint::Target);
    if (new_source_label_dir != old_source_label_dir)
        updatePixmap(EndPoint::Source);
    if (new_target_label_dir != old_target_label_dir)
        updatePixmap(EndPoint::Target);
}

void Connection::trimLine()
{
    if (m_source == 0 || m_source_pos == QPoint(-1, -1) || m_target_pos == QPoint(-1, -1))
        return;
    int cnt = m_knee_list.size();
    if (cnt < 2)
        return;

    const QRect sr = m_source_rect;
    const QRect tr = m_target_rect;

    if (sr.contains(m_knee_list.at(1)))
        m_knee_list.removeFirst();

    cnt = m_knee_list.size();
    if (cnt < 2)
        return;

    if (!tr.contains(sr) && tr.contains(m_knee_list.at(cnt - 2)))
        m_knee_list.removeLast();

    cnt = m_knee_list.size();
    if (cnt < 2)
        return;

    if (sr.contains(m_knee_list.at(0)) && !sr.contains(m_knee_list.at(1)))
        m_knee_list[0] = lineEntryPos(m_knee_list.at(1), m_knee_list.at(0), sr);

    if (tr.contains(m_knee_list.at(cnt - 1)) && !tr.contains(m_knee_list.at(cnt - 2))) {
        m_knee_list[cnt - 1]
            = lineEntryPos(m_knee_list.at(cnt - 2), m_knee_list.at(cnt - 1), tr);
        m_arrow_head = arrowHead(m_knee_list.at(cnt - 2), m_knee_list.at(cnt - 1));
    }
}

void Connection::setSource(QObject *source, const QPoint &pos)
{
    if (source == m_source && m_source_pos == pos)
        return;

    update(false);

    m_source = source;
    if (QWidget *widget = qobject_cast<QWidget*>(source)) {
        m_source_pos = pos;
        m_source_rect = m_edit->widgetRect(widget);
        updateKneeList();
    }

    update(false);
}

void Connection::setTarget(QObject *target, const QPoint &pos)
{
    if (target == m_target && m_target_pos == pos)
        return;

    update(false);

    m_target = target;
    if (QWidget *widget = qobject_cast<QWidget*>(target)) {
        m_target_pos = pos;
        m_target_rect = m_edit->widgetRect(widget);
        updateKneeList();
    }

    update(false);
}

static QRect lineRect(const QPoint &a, const QPoint &b)
{
    const QPoint c(qMin(a.x(), b.x()), qMin(a.y(), b.y()));
    const QPoint d(qMax(a.x(), b.x()), qMax(a.y(), b.y()));

    QRect result(c, d);
    return expand(result, LINE_PROXIMITY_RADIUS);
}

QRect Connection::groundRect() const
{
    if (!ground())
        return QRect();
    if (m_knee_list.isEmpty())
        return QRect();

    const QPoint p = m_knee_list.last();
    return QRect(p.x() - GROUND_W/2, p.y(), GROUND_W, GROUND_H);
}

QRegion Connection::region() const
{
    QRegion result;

    for (int i = 0; i < m_knee_list.size() - 1; ++i)
        result = result.unite(lineRect(m_knee_list.at(i), m_knee_list.at(i + 1)));

    if (!m_arrow_head.isEmpty()) {
        QRect r = m_arrow_head.boundingRect().toRect();
        r = expand(r, 1);
        result = result.unite(r);
    } else if (ground()) {
        result = result.unite(groundRect());
    }

    result = result.unite(labelRect(EndPoint::Source));
    result = result.unite(labelRect(EndPoint::Target));

    return result;
}

void Connection::update(bool update_widgets) const
{
    m_edit->update(region());
    if (update_widgets) {
        if (m_source != 0)
            m_edit->update(m_source_rect);
        if (m_target != 0)
            m_edit->update(m_target_rect);
    }

    m_edit->update(endPointRect(EndPoint::Source));
    m_edit->update(endPointRect(EndPoint::Target));
}

void Connection::paint(QPainter *p) const
{
    for (int i = 0; i < m_knee_list.size() - 1; ++i)
        p->drawLine(m_knee_list.at(i), m_knee_list.at(i + 1));

    if (!m_arrow_head.isEmpty()) {
        p->save();
        p->setBrush(p->pen().color());
        p->drawPolygon(m_arrow_head);
        p->restore();
    } else if (ground()) {
        paintGround(p, groundRect());
    }
}

bool Connection::contains(const QPoint &pos) const
{
    return region().contains(pos);
}

QRect Connection::endPointRect(EndPoint::Type type) const
{
    if (type == EndPoint::Source) {
        if (m_source_pos != QPoint(-1, -1))
            return endPointRectHelper(m_source_pos);
    } else {
        if (m_target_pos != QPoint(-1, -1))
            return endPointRectHelper(m_target_pos);
    }
    return QRect();
}

CETypes::LineDir Connection::labelDir(EndPoint::Type type) const
{
    const int cnt = m_knee_list.size();
    if (cnt < 2)
        return RightDir;

    LineDir dir;
    if (type == EndPoint::Source)
        dir = classifyLine(m_knee_list.at(0), m_knee_list.at(1));
    else
        dir = classifyLine(m_knee_list.at(cnt - 2), m_knee_list.at(cnt - 1));

    if (dir == LeftDir)
        dir = RightDir;
    if (dir == UpDir)
        dir = DownDir;

    return dir;
}

QRect Connection::labelRect(EndPoint::Type type) const
{
    const int cnt = m_knee_list.size();
    if (cnt < 2)
        return QRect();
    const QString text = label(type);
    if (text.isEmpty())
        return QRect();

    const QSize size = labelPixmap(type).size();
    QPoint p1, p2;
    if (type == EndPoint::Source) {
        p1 = m_knee_list.at(0);
        p2 = m_knee_list.at(1);
    } else {
        p1 = m_knee_list.at(cnt - 1);
        p2 = m_knee_list.at(cnt - 2);
    }
    const LineDir dir = classifyLine(p1, p2);

    QRect result;
    switch (dir) {
        case UpDir:
            result = QRect(p1 + QPoint(-size.width()/2, 0), size);
            break;
        case DownDir:
            result = QRect(p1 + QPoint(-size.width()/2, -size.height()), size);
            break;
        case LeftDir:
            result = QRect(p1 + QPoint(0, -size.height()/2), size);
            break;
        case RightDir:
            result = QRect(p1 + QPoint(-size.width(), -size.height()/2), size);
            break;
    }

    return result;
}

void Connection::setLabel(EndPoint::Type type, const QString &text)
{
    if (text == label(type))
        return;

    if (type == EndPoint::Source)
        m_source_label = text;
    else
        m_target_label = text;

    updatePixmap(type);
}

void Connection::updatePixmap(EndPoint::Type type)
{
    QPixmap *pm = type == EndPoint::Source ? &m_source_label_pm : &m_target_label_pm;

    const QString text = label(type);
    if (text.isEmpty()) {
        *pm = QPixmap();
        return;
    }

    const QFontMetrics fm = m_edit->fontMetrics();
    const QSize size = fm.size(Qt::TextSingleLine, text) + QSize(HLABEL_MARGIN*2, VLABEL_MARGIN*2);
    *pm = QPixmap(size);
    QColor color = m_edit->palette().color(QPalette::Normal, QPalette::Base);
    color.setAlpha(190);
    pm->fill(color);

    QPainter p(pm);
    p.setPen(m_edit->palette().color(QPalette::Normal, QPalette::Text));
    p.drawText(-fm.leftBearing(text.at(0)) + HLABEL_MARGIN, fm.ascent() + VLABEL_MARGIN, text);
    p.end();

    const LineDir dir = labelDir(type);

    if (dir == DownDir)
        *pm = pm->transformed(QMatrix(0.0, -1.0, 1.0, 0.0, 0.0, 0.0));
}

void Connection::checkWidgets()
{
    bool changed = false;

    if (QWidget *sourceWidget = qobject_cast<QWidget*>(m_source)) {
        const QRect r = m_edit->widgetRect(sourceWidget);
        if (r != m_source_rect) {
            if (m_source_pos != QPoint(-1, -1) && !r.contains(m_source_pos)) {
                QPoint offset = m_source_pos - m_source_rect.topLeft();
                QPoint old_pos = m_source_pos;
                m_source_pos = pointInsideRect(r, r.topLeft() + offset);
            }
            m_edit->update(m_source_rect);
            m_source_rect = r;
            changed = true;
        }
    }

    if (QWidget *targetWidget = qobject_cast<QWidget*>(m_target)) {
        const QRect r = m_edit->widgetRect(targetWidget);
        if (r != m_target_rect) {
            if (m_target_pos != QPoint(-1, -1) && !r.contains(m_target_pos)) {
                const QPoint offset = m_target_pos - m_target_rect.topLeft();
                const QPoint old_pos = m_target_pos;
                m_target_pos = pointInsideRect(r, r.topLeft() + offset);
            }
            m_edit->update(m_target_rect);
            m_target_rect = r;
            changed = true;
        }
    }

    if (changed) {
        update();
        updateKneeList();
        update();
    }
}

/*******************************************************************************
** ConnectionEdit
*/

ConnectionEdit::ConnectionEdit(QWidget *parent, QDesignerFormWindowInterface *form) :
    QWidget(parent),
    m_bg_widget(0),
    m_undo_stack(form->commandHistory()),
    m_enable_update_background(false),
    m_tmp_con(0),
    m_start_connection_on_drag(true),
    m_widget_under_mouse(0),
    m_inactive_color(Qt::blue),
    m_active_color(Qt::red)
{
    setAttribute(Qt::WA_MouseTracking, true);
    setFocusPolicy(Qt::ClickFocus);

    connect(form, SIGNAL(widgetRemoved(QWidget*)), this, SLOT(widgetRemoved(QWidget*)));
    connect(form, SIGNAL(objectRemoved(QObject*)), this, SLOT(objectRemoved(QObject*)));
}

ConnectionEdit::~ConnectionEdit()
{
    qDeleteAll(m_con_list);
}

void ConnectionEdit::clear()
{
    m_con_list.clear();
    m_sel_con_set.clear();
    m_bg_widget = 0;
    m_widget_under_mouse = 0;
    m_tmp_con = 0;
}

void ConnectionEdit::setBackground(QWidget *background)
{
    if (background == m_bg_widget) {
        // nothing to do
        return;
    }

    m_bg_widget = background;
    updateBackground();
}

void ConnectionEdit::enableUpdateBackground(bool enable)
{
    m_enable_update_background = enable;
    if (enable)
        updateBackground();
}

void ConnectionEdit::updateBackground()
{
    // Might happen while reloading a form.
    if (m_bg_widget == 0)
        return;

    if (!m_enable_update_background)
        return;

    foreach(Connection *c, m_con_list)
        c->updateVisibility();

    updateLines();
    update();
}

QWidget *ConnectionEdit::widgetAt(const QPoint &pos) const
{
    if (m_bg_widget == 0)
        return 0;
    QWidget *widget = m_bg_widget->childAt(pos);
    if (widget == 0)
        widget = m_bg_widget;

    return widget;
}


QRect ConnectionEdit::widgetRect(QWidget *w) const
{
    if (w == 0)
        return QRect();
    QRect r = w->geometry();
    QPoint pos = w->mapToGlobal(QPoint(0, 0));
    pos = mapFromGlobal(pos);
    r.moveTopLeft(pos);
    return r;
}

ConnectionEdit::State ConnectionEdit::state() const
{
    if (m_tmp_con != 0)
        return Connecting;
    if (!m_drag_end_point.isNull())
        return Dragging;
    return Editing;
}

void ConnectionEdit::paintLabel(QPainter *p, EndPoint::Type type, Connection *con)
{
    if (con->label(type).isEmpty())
        return;

    const bool heavy = selected(con) || con == m_tmp_con;
    p->setPen(heavy ? m_active_color : m_inactive_color);
    p->setBrush(Qt::NoBrush);
    const QRect r = con->labelRect(type);
    p->drawPixmap(r.topLeft(), con->labelPixmap(type));
    p->drawRect(fixRect(r));
}

void ConnectionEdit::paintConnection(QPainter *p, Connection *con,
                                        WidgetSet *heavy_highlight_set,
                                        WidgetSet *light_highlight_set) const
{
    QWidget *source = con->widget(EndPoint::Source);
    QWidget *target = con->widget(EndPoint::Target);

    const bool heavy = selected(con) || con == m_tmp_con;
    WidgetSet *set = heavy ? heavy_highlight_set : light_highlight_set;
    p->setPen(heavy ? m_active_color : m_inactive_color);
    con->paint(p);

    if (source != 0 && source != m_bg_widget)
        set->insert(source, source);

    if (target != 0 && target != m_bg_widget)
        set->insert(target, target);
}

void ConnectionEdit::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setClipRegion(e->region());

    WidgetSet heavy_highlight_set, light_highlight_set;

    foreach (Connection *con, m_con_list) {
        if (!con->isVisible())
            continue;

        paintConnection(&p, con, &heavy_highlight_set, &light_highlight_set);
    }

    if (m_tmp_con != 0)
        paintConnection(&p, m_tmp_con, &heavy_highlight_set, &light_highlight_set);

    if (!m_widget_under_mouse.isNull() && m_widget_under_mouse != m_bg_widget)
        heavy_highlight_set.insert(m_widget_under_mouse, m_widget_under_mouse);

    QColor c = m_active_color;
    p.setPen(c);
    c.setAlpha(BG_ALPHA);
    p.setBrush(c);

    foreach (QWidget *w, heavy_highlight_set) {
        p.drawRect(fixRect(widgetRect(w)));
        light_highlight_set.remove(w);
    }

    c = m_inactive_color;
    p.setPen(c);
    c.setAlpha(BG_ALPHA);
    p.setBrush(c);

    foreach (QWidget *w, light_highlight_set)
        p.drawRect(fixRect(widgetRect(w)));

    p.setBrush(palette().color(QPalette::Base));
    p.setPen(palette().color(QPalette::Text));
    foreach (Connection *con, m_con_list) {
        if (!con->isVisible())
            continue;

        paintLabel(&p, EndPoint::Source, con);
        paintLabel(&p, EndPoint::Target, con);
    }

    p.setPen(m_active_color);
    p.setBrush(m_active_color);

    foreach (Connection *con, m_con_list) {
        if (!selected(con) || !con->isVisible())
            continue;

        paintEndPoint(&p, con->endPointPos(EndPoint::Source));

        if (con->widget(EndPoint::Target) != 0)
            paintEndPoint(&p, con->endPointPos(EndPoint::Target));
    }
}

void ConnectionEdit::abortConnection()
{
    m_tmp_con->update();
    delete m_tmp_con;
    m_tmp_con = 0;
#ifndef QT_NO_CURSOR
    setCursor(QCursor());
#endif
    if (m_widget_under_mouse == m_bg_widget)
        m_widget_under_mouse = 0;
}

void ConnectionEdit::mousePressEvent(QMouseEvent *e)
{
    // Right click only to cancel
    const Qt::MouseButton button = e->button();
    const State cstate = state();
    if (button != Qt::LeftButton && !(button == Qt::RightButton && cstate == Connecting)) {
        QWidget::mousePressEvent(e);
        return;
    }

    e->accept();
    // Prefer a non-background widget over the connection,
    // otherwise, widgets covered by the connection labels cannot be accessed
    Connection *con_under_mouse = 0;
    if (!m_widget_under_mouse || m_widget_under_mouse == m_bg_widget)
        con_under_mouse = connectionAt(e->pos());

    m_start_connection_on_drag = false;
    switch (cstate) {
        case Connecting:
            if (button == Qt::RightButton)
                abortConnection();
            break;
        case Dragging:
            break;
        case Editing:
            if (!m_end_point_under_mouse.isNull()) {
                if (!(e->modifiers() & Qt::ShiftModifier)) {
                    startDrag(m_end_point_under_mouse, e->pos());
                }
            } else if (con_under_mouse != 0) {
                if (!(e->modifiers() & Qt::ShiftModifier)) {
                    selectNone();
                    setSelected(con_under_mouse, true);
                } else {
                    setSelected(con_under_mouse, !selected(con_under_mouse));
                }
            } else {
                if (!(e->modifiers() & Qt::ShiftModifier)) {
                    selectNone();
                    if (!m_widget_under_mouse.isNull())
                        m_start_connection_on_drag = true;
                }
            }
            break;
    }
}

void ConnectionEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) {
        QWidget::mouseDoubleClickEvent(e);
        return;
    }

    e->accept();
    switch (state()) {
        case Connecting:
            abortConnection();
            break;
        case Dragging:
            break;
        case Editing:
            if (!m_widget_under_mouse.isNull()) {
                emit widgetActivated(m_widget_under_mouse);
            } else if (m_sel_con_set.size() == 1) {
                Connection *con = m_sel_con_set.keys().first();
                modifyConnection(con);
            }
            break;
    }

}

void ConnectionEdit::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(e);
        return;
    }
    e->accept();

    switch (state()) {
        case Connecting:
            if (m_widget_under_mouse.isNull())
                abortConnection();
            else
                endConnection(m_widget_under_mouse, e->pos());
#ifndef QT_NO_CURSOR
            setCursor(QCursor());
#endif
            break;
        case Editing:
            break;
        case Dragging:
            endDrag(e->pos());
            break;
    }
}


void ConnectionEdit::findObjectsUnderMouse(const QPoint &pos)
{
    Connection *con_under_mouse = connectionAt(pos);

    QWidget *w = widgetAt(pos);
    // Prefer a non-background widget over the connection,
    // otherwise, widgets covered by the connection labels cannot be accessed
    if (w == m_bg_widget && con_under_mouse)
        w = 0;
    else
        con_under_mouse = 0;

    if (w != m_widget_under_mouse) {
        if (!m_widget_under_mouse.isNull())
            update(widgetRect(m_widget_under_mouse));
        m_widget_under_mouse = w;
        if (!m_widget_under_mouse.isNull())
            update(widgetRect(m_widget_under_mouse));
    }

    const EndPoint hs = endPointAt(pos);
    if (hs != m_end_point_under_mouse) {
#ifndef QT_NO_CURSOR
        if (m_end_point_under_mouse.isNull())
            setCursor(Qt::PointingHandCursor);
        else
            setCursor(QCursor());
#endif
        m_end_point_under_mouse = hs;
    }
}

void ConnectionEdit::mouseMoveEvent(QMouseEvent *e)
{
    findObjectsUnderMouse(e->pos());
    switch (state()) {
        case Connecting:
            continueConnection(m_widget_under_mouse, e->pos());
            break;
        case Editing:
            if ((e->buttons() & Qt::LeftButton)
                    && m_start_connection_on_drag
                    && !m_widget_under_mouse.isNull()) {
                m_start_connection_on_drag = false;
                startConnection(m_widget_under_mouse, e->pos());
#ifndef QT_NO_CURSOR
                setCursor(Qt::CrossCursor);
#endif
            }
            break;
        case Dragging:
            continueDrag(e->pos());
            break;
    }

    e->accept();
}

void ConnectionEdit::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_Delete:
            if (state() == Editing)
                deleteSelected();
            break;
        case Qt::Key_Escape:
            if (state() == Connecting)
                abortConnection();
            break;
    }

    e->accept();
}

void ConnectionEdit::startConnection(QWidget *source, const QPoint &pos)
{
    Q_ASSERT(m_tmp_con == 0);

    m_tmp_con = new Connection(this);
    m_tmp_con->setEndPoint(EndPoint::Source, source, pos);
}

void ConnectionEdit::endConnection(QWidget *target, const QPoint &pos)
{
    Q_ASSERT(m_tmp_con != 0);

    m_tmp_con->setEndPoint(EndPoint::Target, target, pos);

    QWidget *source = m_tmp_con->widget(EndPoint::Source);
    Q_ASSERT(source != 0);
    Q_ASSERT(target != 0);
    setEnabled(false);
    Connection *new_con = createConnection(source, target);
    setEnabled(true);
    if (new_con != 0) {
        new_con->setEndPoint(EndPoint::Source, source, m_tmp_con->endPointPos(EndPoint::Source));
        new_con->setEndPoint(EndPoint::Target, target, m_tmp_con->endPointPos(EndPoint::Target));
        m_undo_stack->push(new AddConnectionCommand(this, new_con));
        emit connectionChanged(new_con);
    }

    delete m_tmp_con;
    m_tmp_con = 0;

    findObjectsUnderMouse(mapFromGlobal(QCursor::pos()));
}

void ConnectionEdit::continueConnection(QWidget *target, const QPoint &pos)
{
    Q_ASSERT(m_tmp_con != 0);

    m_tmp_con->setEndPoint(EndPoint::Target, target, pos);
}

void ConnectionEdit::modifyConnection(Connection *)
{
}

Connection *ConnectionEdit::createConnection(QWidget *source, QWidget *target)
{
    Connection *con = new Connection(this, source, target);
    return con;
}

// Find all connections which in which a sequence of objects is involved
template <class ObjectIterator>
static ConnectionEdit::ConnectionSet findConnectionsOf(const ConnectionEdit::ConnectionList &cl, ObjectIterator oi1, const ObjectIterator &oi2)
{
    ConnectionEdit::ConnectionSet rc;

    const ConnectionEdit::ConnectionList::const_iterator ccend = cl.constEnd();
    for ( ; oi1 != oi2; ++oi1) {
        for (ConnectionEdit::ConnectionList::const_iterator cit = cl.constBegin(); cit != ccend; ++cit) {
            Connection *con = *cit;
            if (con->object(ConnectionEdit::EndPoint::Source) == *oi1 || con->object(ConnectionEdit::EndPoint::Target) == *oi1)
                rc.insert(con, con);
        }
    }
    return rc;
}

void ConnectionEdit::widgetRemoved(QWidget *widget)
{
    // Remove all connections of that widget and its children.
    if (m_con_list.empty())
        return;

    QWidgetList child_list = qFindChildren<QWidget*>(widget);
    child_list.prepend(widget);

    const ConnectionSet remove_set = findConnectionsOf(m_con_list, child_list.constBegin(),  child_list.constEnd());

    if (!remove_set.isEmpty())
        m_undo_stack->push(new DeleteConnectionsCommand(this, remove_set.keys()));

    updateBackground();
}

void ConnectionEdit::objectRemoved(QObject *o)
{
    // Remove all connections of that object and its children (in case of action groups).
    if (m_con_list.empty())
        return;

    QObjectList child_list = o->children();
    child_list.prepend(o);
    const ConnectionSet remove_set = findConnectionsOf(m_con_list, child_list.constBegin(),  child_list.constEnd());
    if (!remove_set.isEmpty())
        m_undo_stack->push(new DeleteConnectionsCommand(this, remove_set.keys()));

    updateBackground();
}

void ConnectionEdit::setSelected(Connection *con, bool sel)
{
    if (!con || sel == m_sel_con_set.contains(con))
        return;

    if (sel) {
        m_sel_con_set.insert(con, con);
        emit connectionSelected(con);
    } else {
        m_sel_con_set.remove(con);
    }

    con->update();
}

bool ConnectionEdit::selected(const Connection *con) const
{
    return m_sel_con_set.contains(const_cast<Connection*>(con));
}

void ConnectionEdit::selectNone()
{
    foreach (Connection *con, m_sel_con_set)
        con->update();

    m_sel_con_set.clear();
}

void ConnectionEdit::selectAll()
{
    if (m_sel_con_set.size() == m_con_list.size())
        return;
    foreach (Connection *con, m_con_list)
        setSelected(con, true);
}

Connection *ConnectionEdit::connectionAt(const QPoint &pos) const
{
    foreach (Connection *con, m_con_list) {
        if (con->contains(pos))
            return con;
    }
    return 0;
}

CETypes::EndPoint ConnectionEdit::endPointAt(const QPoint &pos) const
{
    foreach (Connection *con, m_con_list) {
        if (!selected(con))
            continue;
        const QRect sr = con->endPointRect(EndPoint::Source);
        const QRect tr = con->endPointRect(EndPoint::Target);

        if (sr.contains(pos))
            return EndPoint(con, EndPoint::Source);
        if (tr.contains(pos))
            return EndPoint(con, EndPoint::Target);
    }
    return EndPoint();
}

void ConnectionEdit::startDrag(const EndPoint &end_point, const QPoint &pos)
{
    Q_ASSERT(m_drag_end_point.isNull());
    m_drag_end_point = end_point;
    m_old_source_pos = m_drag_end_point.con->endPointPos(EndPoint::Source);
    m_old_target_pos = m_drag_end_point.con->endPointPos(EndPoint::Target);
    adjustHotSopt(m_drag_end_point, pos);
}

void ConnectionEdit::continueDrag(const QPoint &pos)
{
    Q_ASSERT(!m_drag_end_point.isNull());
    adjustHotSopt(m_drag_end_point, pos);
}

void ConnectionEdit::endDrag(const QPoint &pos)
{
    Q_ASSERT(!m_drag_end_point.isNull());
    adjustHotSopt(m_drag_end_point, pos);

    Connection *con = m_drag_end_point.con;
    const QPoint new_source_pos = con->endPointPos(EndPoint::Source);
    const QPoint new_target_pos = con->endPointPos(EndPoint::Target);
    m_undo_stack->push(new AdjustConnectionCommand(this, con, m_old_source_pos, m_old_target_pos,
                                                    new_source_pos, new_target_pos));

    m_drag_end_point = EndPoint();
}

void ConnectionEdit::adjustHotSopt(const EndPoint &end_point, const QPoint &pos)
{
    QWidget *w = end_point.con->widget(end_point.type);
    end_point.con->setEndPoint(end_point.type, w, pointInsideRect(widgetRect(w), pos));
}

void ConnectionEdit::deleteSelected()
{
    if (m_sel_con_set.isEmpty())
        return;
    m_undo_stack->push(new DeleteConnectionsCommand(this, m_sel_con_set.keys()));
}

void ConnectionEdit::addConnection(Connection *con)
{
    m_con_list.append(con);
}

void ConnectionEdit::updateLines()
{
    foreach (Connection *con, m_con_list)
        con->checkWidgets();
}

void ConnectionEdit::resizeEvent(QResizeEvent *e)
{
    updateBackground();
    QWidget::resizeEvent(e);
}

void ConnectionEdit::setSource(Connection *con, const QString &obj_name)
{
    QObject *object = 0;
    if (!obj_name.isEmpty()) {
        object = qFindChild<QObject*>(m_bg_widget, obj_name);
        if (object == 0 && m_bg_widget->objectName() == obj_name)
            object = m_bg_widget;

        if (object == con->object(EndPoint::Source))
            return;
    }
    m_undo_stack->push(new SetEndPointCommand(this, con, EndPoint::Source, object));
}

void ConnectionEdit::setTarget(Connection *con, const QString &obj_name)
{
    QObject *object = 0;
    if (!obj_name.isEmpty()) {
        object = qFindChild<QObject*>(m_bg_widget, obj_name);
        if (object == 0 && m_bg_widget->objectName() == obj_name)
            object = m_bg_widget;

        if (object == con->object(EndPoint::Target))
            return;
    }
    m_undo_stack->push(new SetEndPointCommand(this, con, EndPoint::Target, object));
}

Connection *ConnectionEdit::takeConnection(Connection *con)
{
    if (!m_con_list.contains(con))
        return 0;
    m_con_list.removeAll(con);
    return con;
}

void ConnectionEdit::clearNewlyAddedConnection()
{
    delete m_tmp_con;
    m_tmp_con = 0;
}

void ConnectionEdit::createContextMenu(QMenu &menu)
{
    // Select
    QAction *selectAllAction = menu.addAction(tr("Select All"));
    selectAllAction->setEnabled(connectionList().size());
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));
    QAction *deselectAllAction = menu.addAction(tr("Deselect All"));
    deselectAllAction->setEnabled(selection().size());
    connect(deselectAllAction, SIGNAL(triggered()), this, SLOT(selectNone()));
    menu.addSeparator();
    // Delete
    QAction *deleteAction = menu.addAction(tr("Delete"));
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setEnabled(!selection().isEmpty());
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
}

void ConnectionEdit::contextMenuEvent(QContextMenuEvent * event)
{
    QMenu menu;
    createContextMenu(menu);
    menu.exec(event->globalPos());
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
