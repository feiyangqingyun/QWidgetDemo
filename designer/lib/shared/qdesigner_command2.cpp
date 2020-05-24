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

#include "qdesigner_command2_p.h"
#include "formwindowbase_p.h"
#include "layoutinfo_p.h"
#include "qdesigner_command_p.h"
#include "widgetfactory_p.h"
#include "qlayout_widget_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include <QtGui/QApplication>
#include <QtGui/QLayout>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

MorphLayoutCommand::MorphLayoutCommand(QDesignerFormWindowInterface *formWindow) :
    QDesignerFormWindowCommand(QString(), formWindow),
    m_breakLayoutCommand(new BreakLayoutCommand(formWindow)),
    m_layoutCommand(new LayoutCommand(formWindow)),
    m_newType(LayoutInfo::VBox),
    m_layoutBase(0)
{
}

MorphLayoutCommand::~MorphLayoutCommand()
{
    delete m_layoutCommand;
    delete m_breakLayoutCommand;
}

bool MorphLayoutCommand::init(QWidget *w, int newType)
{
    int oldType;
    QDesignerFormWindowInterface *fw = formWindow();
    if (!canMorph(fw, w, &oldType) || oldType == newType)
        return false;
    m_layoutBase = w;
    m_newType = newType;
    // Find all managed widgets
    m_widgets.clear();
    const QLayout *layout = LayoutInfo::managedLayout(fw->core(), w);
    const int count = layout->count();
    for (int i = 0; i < count ; i++) {
        if (QWidget *w = layout->itemAt(i)->widget())
            if (fw->isManaged(w))
                m_widgets.push_back(w);
    }
    const bool reparentLayoutWidget = false; // leave QLayoutWidget intact
    m_breakLayoutCommand->init(m_widgets, m_layoutBase, reparentLayoutWidget);
    m_layoutCommand->init(m_layoutBase, m_widgets, static_cast<LayoutInfo::Type>(m_newType), m_layoutBase, reparentLayoutWidget);
    setText(formatDescription(core(), m_layoutBase, oldType, newType));
    return true;
}

bool MorphLayoutCommand::canMorph(const QDesignerFormWindowInterface *formWindow, QWidget *w, int *ptrToCurrentType)
{
    if (ptrToCurrentType)
        *ptrToCurrentType = LayoutInfo::NoLayout;
    // We want a managed widget or a container page
    // with a level-0 managed layout
    QDesignerFormEditorInterface *core = formWindow->core();
    QLayout *layout = LayoutInfo::managedLayout(core, w);
    if (!layout)
        return false;
    const LayoutInfo::Type type = LayoutInfo::layoutType(core, layout);
    if (ptrToCurrentType)
        *ptrToCurrentType = type;
    switch (type) {
    case LayoutInfo::HBox:
    case LayoutInfo::VBox:
    case LayoutInfo::Grid:
    case LayoutInfo::Form:
        return true;
        break;
    case LayoutInfo::NoLayout:
    case LayoutInfo::HSplitter: // Nothing doing
    case LayoutInfo::VSplitter:
    case LayoutInfo::UnknownLayout:
        break;
    }
    return false;
}

void MorphLayoutCommand::redo()
{
    m_breakLayoutCommand->redo();
    m_layoutCommand->redo();
    /* Transfer applicable properties which is a cross-section of the modified
     * properties except object name. */
    if (const LayoutProperties *properties = m_breakLayoutCommand->layoutProperties()) {
        const int oldMask = m_breakLayoutCommand->propertyMask();
        QLayout *newLayout = LayoutInfo::managedLayout(core(), m_layoutBase);
        const int newMask = LayoutProperties::visibleProperties(newLayout);
        const int applicableMask = (oldMask & newMask) & ~LayoutProperties::ObjectNameProperty;
        if (applicableMask)
            properties->toPropertySheet(core(), newLayout, applicableMask);
    }
}

void MorphLayoutCommand::undo()
{
    m_layoutCommand->undo();
    m_breakLayoutCommand->undo();
}

QString MorphLayoutCommand::formatDescription(QDesignerFormEditorInterface * /* core*/, const QWidget *w, int oldType, int newType)
{
    const QString oldName = LayoutInfo::layoutName(static_cast<LayoutInfo::Type>(oldType));
    const QString newName = LayoutInfo::layoutName(static_cast<LayoutInfo::Type>(newType));
    const QString widgetName = qobject_cast<const QLayoutWidget*>(w) ? w->layout()->objectName() : w->objectName();
    return QApplication::translate("Command", "Change layout of '%1' from %2 to %3").arg(widgetName, oldName, newName);
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
