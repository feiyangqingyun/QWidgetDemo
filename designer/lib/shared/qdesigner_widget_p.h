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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_WIDGET_H
#define QDESIGNER_WIDGET_H

#include "shared_global_p.h"
#include <QtGui/QDialog>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;

namespace qdesigner_internal {
    class FormWindowBase;
}

class QDESIGNER_SHARED_EXPORT QDesignerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDesignerWidget(QDesignerFormWindowInterface* formWindow, QWidget *parent = 0);
    virtual ~QDesignerWidget();

    QDesignerFormWindowInterface* formWindow() const;

    void updatePixmap();

    virtual QSize minimumSizeHint() const
    { return QWidget::minimumSizeHint().expandedTo(QSize(16, 16)); }

protected:
    virtual void paintEvent(QPaintEvent *e);

private:
    qdesigner_internal::FormWindowBase* m_formWindow;
};

class QDESIGNER_SHARED_EXPORT QDesignerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QDesignerDialog(QDesignerFormWindowInterface *fw, QWidget *parent);

    virtual QSize minimumSizeHint() const
    { return QWidget::minimumSizeHint().expandedTo(QSize(16, 16)); }

protected:
    void paintEvent(QPaintEvent *e);

private:
    qdesigner_internal::FormWindowBase* m_formWindow;
};

class QDESIGNER_SHARED_EXPORT Line : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
public:
    explicit Line(QWidget *parent) : QFrame(parent)
    { setAttribute(Qt::WA_MouseNoMask); setFrameStyle(HLine | Sunken); }

    inline void setOrientation(Qt::Orientation orient)
    { setFrameShape(orient == Qt::Horizontal ? HLine : VLine); }

    inline Qt::Orientation orientation() const
    { return frameShape() == HLine ? Qt::Horizontal : Qt::Vertical; }
};

QT_END_NAMESPACE

#endif // QDESIGNER_WIDGET_H
