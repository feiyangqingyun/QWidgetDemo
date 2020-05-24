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

#ifndef QDESIGNER_FORMWINDOWCOMMAND_H
#define QDESIGNER_FORMWINDOWCOMMAND_H

#include "shared_global_p.h"

#include <QtCore/QPointer>
#include <QtGui/QUndoCommand>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerPropertySheetExtension;

namespace qdesigner_internal {

class QDESIGNER_SHARED_EXPORT QDesignerFormWindowCommand: public QUndoCommand
{

public:
    QDesignerFormWindowCommand(const QString &description,
                               QDesignerFormWindowInterface *formWindow,
                               QUndoCommand *parent = 0);

    virtual void undo();
    virtual void redo();

    static void updateBuddies(QDesignerFormWindowInterface *form,
                              const QString &old_name, const QString &new_name);
protected:
    QDesignerFormWindowInterface *formWindow() const;
    QDesignerFormEditorInterface *core() const;
    QDesignerPropertySheetExtension* propertySheet(QObject *object) const;

    void cheapUpdate();

    void selectUnmanagedObject(QObject *unmanagedObject);
private:
    QPointer<QDesignerFormWindowInterface> m_formWindow;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_COMMAND_H
