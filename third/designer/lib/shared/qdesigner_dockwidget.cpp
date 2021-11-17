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

#include "qdesigner_dockwidget_p.h"
#include "layoutinfo_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormWindowCursorInterface>

#include <QtGui/QMainWindow>
#include <QtGui/QLayout>

QT_BEGIN_NAMESPACE

QDesignerDockWidget::QDesignerDockWidget(QWidget *parent)
    : QDockWidget(parent)
{
}

QDesignerDockWidget::~QDesignerDockWidget()
{
}

bool QDesignerDockWidget::docked() const
{
    return qobject_cast<QMainWindow*>(parentWidget()) != 0;
}

void QDesignerDockWidget::setDocked(bool b)
{
    if (QMainWindow *mainWindow = findMainWindow()) {
        QDesignerFormEditorInterface *core = formWindow()->core();
        QDesignerContainerExtension *c;
        c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), mainWindow);
        if (b && !docked()) {
            // Dock it
            // ### undo/redo stack
            setParent(0);
            c->addWidget(this);
            formWindow()->selectWidget(this, formWindow()->cursor()->isWidgetSelected(this));
        } else if (!b && docked()) {
            // Undock it
            for (int i = 0; i < c->count(); ++i) {
                if (c->widget(i) == this) {
                    c->remove(i);
                    break;
                }
            }
            // #### restore the position
            setParent(mainWindow->centralWidget());
            show();
            formWindow()->selectWidget(this, formWindow()->cursor()->isWidgetSelected(this));
        }
    }
}

Qt::DockWidgetArea QDesignerDockWidget::dockWidgetArea() const
{
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget()))
        return mainWindow->dockWidgetArea(const_cast<QDesignerDockWidget*>(this));

    return Qt::LeftDockWidgetArea;
}

void QDesignerDockWidget::setDockWidgetArea(Qt::DockWidgetArea dockWidgetArea)
{
    if (QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parentWidget())) {
        if ((dockWidgetArea != Qt::NoDockWidgetArea)
            && isAreaAllowed(dockWidgetArea)) {
            mainWindow->addDockWidget(dockWidgetArea, this);
        }
    }
}

bool QDesignerDockWidget::inMainWindow() const
{
    QMainWindow *mw = findMainWindow();
    if (mw && !mw->centralWidget()->layout()) {
        if (mw == parentWidget())
            return true;
        if (mw->centralWidget() == parentWidget())
            return true;
    }
    return false;
}

QDesignerFormWindowInterface *QDesignerDockWidget::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(const_cast<QDesignerDockWidget*>(this));
}

QMainWindow *QDesignerDockWidget::findMainWindow() const
{
    if (QDesignerFormWindowInterface *fw = formWindow())
        return qobject_cast<QMainWindow*>(fw->mainContainer());
    return 0;
}

QT_END_NAMESPACE
