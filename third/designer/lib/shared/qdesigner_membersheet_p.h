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

#ifndef QDESIGNER_MEMBERSHEET_H
#define QDESIGNER_MEMBERSHEET_H

#include "shared_global_p.h"

#include <QtDesigner/membersheet.h>
#include <QtDesigner/default_extensionfactory.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QDesignerMemberSheetPrivate;

class QDESIGNER_SHARED_EXPORT QDesignerMemberSheet: public QObject, public QDesignerMemberSheetExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerMemberSheetExtension)

public:
    explicit QDesignerMemberSheet(QObject *object, QObject *parent = 0);
    virtual ~QDesignerMemberSheet();

    virtual int indexOf(const QString &name) const;

    virtual int count() const;
    virtual QString memberName(int index) const;

    virtual QString memberGroup(int index) const;
    virtual void setMemberGroup(int index, const QString &group);

    virtual bool isVisible(int index) const;
    virtual void setVisible(int index, bool b);

    virtual bool isSignal(int index) const;
    virtual bool isSlot(int index) const;

    virtual bool isQt3Signal(int index) const;
    virtual bool isQt3Slot(int index) const;

    virtual bool inheritedFromWidget(int index) const;

    static bool signalMatchesSlot(const QString &signal, const QString &slot);

    virtual QString declaredInClass(int index) const;

    virtual QString signature(int index) const;
    virtual QList<QByteArray> parameterTypes(int index) const;
    virtual QList<QByteArray> parameterNames(int index) const;

private:
    QDesignerMemberSheetPrivate *d;
};

class QDESIGNER_SHARED_EXPORT QDesignerMemberSheetFactory: public QExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(QAbstractExtensionFactory)

public:
    QDesignerMemberSheetFactory(QExtensionManager *parent = 0);

protected:
    virtual QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;
};

QT_END_NAMESPACE

#endif // QDESIGNER_MEMBERSHEET_H
