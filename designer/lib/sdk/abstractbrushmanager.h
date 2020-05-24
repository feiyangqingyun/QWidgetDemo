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

#ifndef ABSTRACTBRUSHMANAGER_H
#define ABSTRACTBRUSHMANAGER_H

#include <QtDesigner/sdk_global.h>

#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtGui/qbrush.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QObject;

class QDESIGNER_SDK_EXPORT QDesignerBrushManagerInterface : public QObject
{
    Q_OBJECT
public:
    QDesignerBrushManagerInterface(QObject *parentObject = 0) : QObject(parentObject) {}

    virtual QBrush brush(const QString &name) const = 0;
    virtual QMap<QString, QBrush> brushes() const = 0;
    virtual QString currentBrush() const = 0;

    virtual QString addBrush(const QString &name, const QBrush &brush) = 0;
    virtual void removeBrush(const QString &name) = 0;
    virtual void setCurrentBrush(const QString &name) = 0;

    virtual QPixmap brushPixmap(const QBrush &brush) const = 0;
Q_SIGNALS:
    void brushAdded(const QString &name, const QBrush &brush);
    void brushRemoved(const QString &name);
    void currentBrushChanged(const QString &name, const QBrush &brush);

};

QT_END_NAMESPACE

QT_END_HEADER

#endif
