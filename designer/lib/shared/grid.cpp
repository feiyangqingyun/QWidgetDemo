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

#include "grid_p.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtGui/QPainter>
#include <QtGui/QWidget>
#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

static const bool defaultSnap = true;
static const bool defaultVisible = true;
static const int DEFAULT_GRID = 10;
static const char* KEY_VISIBLE = "gridVisible";
static const char* KEY_SNAPX =  "gridSnapX";
static const char* KEY_SNAPY =  "gridSnapY";
static const char* KEY_DELTAX =  "gridDeltaX";
static const char* KEY_DELTAY =  "gridDeltaY";

// Insert a value into the serialization map unless default
template <class T>
    static inline void valueToVariantMap(T value, T defaultValue, const QString &key, QVariantMap &v, bool forceKey) {
        if (forceKey || value != defaultValue)
            v.insert(key, QVariant(value));
    }

// Obtain a value form QVariantMap
template <class T>
    static inline bool valueFromVariantMap(const QVariantMap &v, const QString &key, T &value) {
        const QVariantMap::const_iterator it = v.constFind(key);
        const bool found = it != v.constEnd();
        if (found)
            value = qVariantValue<T>(it.value());
        return found;
    }

namespace qdesigner_internal
{

Grid::Grid() :
    m_visible(defaultVisible),
    m_snapX(defaultSnap),
    m_snapY(defaultSnap),
    m_deltaX(DEFAULT_GRID),
    m_deltaY(DEFAULT_GRID)
{
}

bool Grid::fromVariantMap(const QVariantMap& vm)
{
    *this = Grid();
    valueFromVariantMap(vm, QLatin1String(KEY_VISIBLE), m_visible);
    valueFromVariantMap(vm, QLatin1String(KEY_SNAPX), m_snapX);
    valueFromVariantMap(vm, QLatin1String(KEY_SNAPY), m_snapY);
    valueFromVariantMap(vm, QLatin1String(KEY_DELTAX), m_deltaX);
    return valueFromVariantMap(vm, QLatin1String(KEY_DELTAY), m_deltaY);
}

QVariantMap Grid::toVariantMap(bool forceKeys) const
{
    QVariantMap rc;
    addToVariantMap(rc, forceKeys);
    return rc;
}

void  Grid::addToVariantMap(QVariantMap& vm, bool forceKeys) const
{
    valueToVariantMap(m_visible, defaultVisible, QLatin1String(KEY_VISIBLE), vm, forceKeys);
    valueToVariantMap(m_snapX, defaultSnap, QLatin1String(KEY_SNAPX), vm, forceKeys);
    valueToVariantMap(m_snapY, defaultSnap, QLatin1String(KEY_SNAPY), vm, forceKeys);
    valueToVariantMap(m_deltaX, DEFAULT_GRID, QLatin1String(KEY_DELTAX), vm, forceKeys);
    valueToVariantMap(m_deltaY, DEFAULT_GRID, QLatin1String(KEY_DELTAY), vm, forceKeys);
}

void Grid::paint(QWidget *widget, QPaintEvent *e) const
{
    QPainter p(widget);
    paint(p, widget, e);
}

void Grid::paint(QPainter &p, const QWidget *widget, QPaintEvent *e) const
{
    p.setPen(widget->palette().dark().color());

    if (m_visible) {
        const int xstart = (e->rect().x() / m_deltaX) * m_deltaX;
        const int ystart = (e->rect().y() / m_deltaY) * m_deltaY;

        const int xend = e->rect().right();
        const int yend = e->rect().bottom();

        typedef QVector<QPointF> Points;
        static Points points;
        points.clear();

        for (int x = xstart; x <= xend; x += m_deltaX) {
            points.reserve((yend - ystart) / m_deltaY + 1);
            for (int y = ystart; y <= yend; y += m_deltaY)
                points.push_back(QPointF(x, y));
            p.drawPoints( &(*points.begin()), points.count());
            points.clear();
        }
    }
}

int Grid::snapValue(int value, int grid) const
{
    const int rest = value % grid;
    const int absRest = (rest < 0) ? -rest : rest;
    int offset = 0;
    if (2 * absRest > grid)
        offset = 1;
    if (rest < 0)
        offset *= -1;
    return (value / grid + offset) * grid;
}

QPoint Grid::snapPoint(const QPoint &p) const
{
    const int sx = m_snapX ? snapValue(p.x(), m_deltaX) : p.x();
    const int sy = m_snapY ? snapValue(p.y(), m_deltaY) : p.y();
    return QPoint(sx, sy);
}

int Grid::widgetHandleAdjustX(int x) const
{
    return m_snapX ? (x / m_deltaX) * m_deltaX + 1 : x;
}

int Grid::widgetHandleAdjustY(int y) const
{
    return m_snapY ? (y / m_deltaY) * m_deltaY + 1 : y;
}

bool Grid::equals(const Grid &rhs) const
{
    return m_visible == rhs.m_visible &&
           m_snapX   == rhs.m_snapX &&
           m_snapY   == rhs.m_snapY &&
           m_deltaX  == rhs.m_deltaX &&
           m_deltaY  == rhs.m_deltaY;
}
}

QT_END_NAMESPACE
