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

#ifndef NEWACTIONDIALOG_P_H
#define NEWACTIONDIALOG_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdesigner_utils_p.h" //  PropertySheetIconValue

#include <QtGui/QDialog>
#include <QtGui/QKeySequence>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

namespace Ui {
    class NewActionDialog;
}

class ActionEditor;

struct ActionData {

    enum ChangeMask {
        TextChanged = 0x1, NameChanged = 0x2, ToolTipChanged = 0x4,
        IconChanged = 0x8, CheckableChanged = 0x10, KeysequenceChanged = 0x20
    };

    ActionData();
    // Returns a combination of ChangeMask flags
    unsigned compare(const  ActionData &rhs) const;

    QString text;
    QString name;
    QString toolTip;
    PropertySheetIconValue icon;
    bool checkable;
    PropertySheetKeySequenceValue keysequence;
};

inline bool operator==(const ActionData &a1, const ActionData &a2) {  return a1.compare(a2) == 0u; }
inline bool operator!=(const ActionData &a1, const ActionData &a2) {  return a1.compare(a2) != 0u; }

class NewActionDialog: public QDialog
{
    Q_OBJECT
public:
    explicit NewActionDialog(ActionEditor *parent);
    virtual ~NewActionDialog();

    ActionData actionData() const;
    void setActionData(const ActionData &d);

    QString actionText() const;
    QString actionName() const;

private slots:
    void on_editActionText_textEdited(const QString &text);
    void on_editObjectName_textEdited(const QString &text);
    void slotEditToolTip();
    void slotResetKeySequence();

private:
    Ui::NewActionDialog *m_ui;
    ActionEditor *m_actionEditor;
    bool m_auto_update_object_name;

    void updateButtons();
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // NEWACTIONDIALOG_P_H
