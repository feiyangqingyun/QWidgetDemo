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

#include "qdesigner.h"
#include "qdesigner_toolwindow.h"
#include "qdesigner_settings.h"
#include "qdesigner_workbench.h"

#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerActionEditorInterface>
#include <QtDesigner/QDesignerObjectInspectorInterface>
#include <QtDesigner/QDesignerWidgetBoxInterface>
#include <QtDesigner/QDesignerComponents>

#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>

enum { debugToolWindow = 0 };

QT_BEGIN_NAMESPACE

// ---------------- QDesignerToolWindowFontSettings
ToolWindowFontSettings::ToolWindowFontSettings() :
    m_writingSystem(QFontDatabase::Any),
    m_useFont(false)
{
}

bool ToolWindowFontSettings::equals(const ToolWindowFontSettings &rhs) const
{
    return m_useFont == rhs.m_useFont &&
           m_writingSystem == rhs.m_writingSystem &&
           m_font == rhs.m_font;
}

// ---------------- QDesignerToolWindow
QDesignerToolWindow::QDesignerToolWindow(QDesignerWorkbench *workbench,
                                         QWidget *w,
                                         const QString &objectName,
                                         const QString &title,
                                         const QString &actionObjectName,
                                         Qt::DockWidgetArea dockAreaHint,
                                         QWidget *parent,
                                         Qt::WindowFlags flags) :
    MainWindowBase(parent, flags),
    m_dockAreaHint(dockAreaHint),
    m_workbench(workbench),
    m_action(new QAction(this))
{
    setObjectName(objectName);
    setCentralWidget(w);

    setWindowTitle(title);

    m_action->setObjectName(actionObjectName);
    m_action->setShortcutContext(Qt::ApplicationShortcut);
    m_action->setText(title);
    m_action->setCheckable(true);
    connect(m_action, SIGNAL(triggered(bool)), this, SLOT(showMe(bool)));
}

void QDesignerToolWindow::showMe(bool v)
{
    // Access the QMdiSubWindow in MDI mode.
    if (QWidget *target = m_workbench->mode() == DockedMode ? parentWidget() : this) {
        if (v)
            target->setWindowState(target->windowState() & ~Qt::WindowMinimized);
        target->setVisible(v);
    }
}

void QDesignerToolWindow::showEvent(QShowEvent *e)
{
    Q_UNUSED(e);

    bool blocked = m_action->blockSignals(true);
    m_action->setChecked(true);
    m_action->blockSignals(blocked);
}

void QDesignerToolWindow::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);

    bool blocked = m_action->blockSignals(true);
    m_action->setChecked(false);
    m_action->blockSignals(blocked);
}

QAction *QDesignerToolWindow::action() const
{
    return m_action;
}

void QDesignerToolWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
        case QEvent::WindowTitleChange:
            m_action->setText(windowTitle());
            break;
        case QEvent::WindowIconChange:
            m_action->setIcon(windowIcon());
            break;
        default:
            break;
    }
    QMainWindow::changeEvent(e);
}

QDesignerWorkbench *QDesignerToolWindow::workbench() const
{
    return m_workbench;
}

QRect QDesignerToolWindow::geometryHint() const
{
    return QRect();
}

QRect QDesignerToolWindow::availableToolWindowGeometry() const
{
    return m_workbench->availableGeometry();
}

//  ---------------------- PropertyEditorToolWindow

static inline QWidget *createPropertyEditor(QDesignerFormEditorInterface *core, QWidget *parent = 0)
{
    QDesignerPropertyEditorInterface *widget = QDesignerComponents::createPropertyEditor(core, parent);
    core->setPropertyEditor(widget);
    return widget;
}

class PropertyEditorToolWindow : public QDesignerToolWindow
{
public:
    explicit PropertyEditorToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;

protected:
    virtual void showEvent(QShowEvent *event);
};

PropertyEditorToolWindow::PropertyEditorToolWindow(QDesignerWorkbench *workbench) :
    QDesignerToolWindow(workbench,
                        createPropertyEditor(workbench->core()),
                        QLatin1String("qt_designer_propertyeditor"),
                        QDesignerToolWindow::tr("Property Editor"),
                        QLatin1String("__qt_property_editor_action"),
                        Qt::RightDockWidgetArea)
{
    action()->setShortcut(Qt::CTRL + Qt::Key_I);

}

QRect PropertyEditorToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();
    const int spacing = 40;
    const QSize sz(g.width() * 1/4, g.height() * 4/6);

    const QRect rc = QRect((g.right() + 1 - sz.width() - margin),
                           (g.top() + margin + g.height() * 1/6) + spacing,
                           sz.width(), sz.height());
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << rc;
    return rc;
}

void PropertyEditorToolWindow::showEvent(QShowEvent *event)
{
    if (QDesignerPropertyEditorInterface *e = workbench()->core()->propertyEditor()) {
        // workaround to update the propertyeditor when it is not visible!
        e->setObject(e->object()); // ### remove me
    }

    QDesignerToolWindow::showEvent(event);
}

//  ---------------------- ActionEditorToolWindow

static inline QWidget *createActionEditor(QDesignerFormEditorInterface *core, QWidget *parent = 0)
{
    QDesignerActionEditorInterface *widget = QDesignerComponents::createActionEditor(core, parent);
    core->setActionEditor(widget);
    return widget;
}

class ActionEditorToolWindow: public QDesignerToolWindow
{
public:
    explicit ActionEditorToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;
};

ActionEditorToolWindow::ActionEditorToolWindow(QDesignerWorkbench *workbench) :
    QDesignerToolWindow(workbench,
                        createActionEditor(workbench->core()),
                        QLatin1String("qt_designer_actioneditor"),
                        QDesignerToolWindow::tr("Action Editor"),
                        QLatin1String("__qt_action_editor_tool_action"),
                        Qt::RightDockWidgetArea)
{
}

QRect ActionEditorToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();

    const QSize sz(g.width() * 1/4, g.height() * 1/6);

    const QRect rc = QRect((g.right() + 1 - sz.width() - margin),
                            g.top() + margin,
                            sz.width(), sz.height());
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << rc;
    return rc;
}

//  ---------------------- ObjectInspectorToolWindow

static inline QWidget *createObjectInspector(QDesignerFormEditorInterface *core, QWidget *parent = 0)
{
    QDesignerObjectInspectorInterface *widget = QDesignerComponents::createObjectInspector(core, parent);
    core->setObjectInspector(widget);
    return widget;
}

class ObjectInspectorToolWindow: public QDesignerToolWindow
{
public:
    explicit ObjectInspectorToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;
};

ObjectInspectorToolWindow::ObjectInspectorToolWindow(QDesignerWorkbench *workbench) :
    QDesignerToolWindow(workbench,
                        createObjectInspector(workbench->core()),
                        QLatin1String("qt_designer_objectinspector"),
                        QDesignerToolWindow::tr("Object Inspector"),
                        QLatin1String("__qt_object_inspector_tool_action"),
                        Qt::RightDockWidgetArea)
{
}

QRect ObjectInspectorToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();

    const QSize sz(g.width() * 1/4, g.height() * 1/6);

    const QRect rc = QRect((g.right() + 1 - sz.width() - margin),
                            g.top() + margin,
                           sz.width(), sz.height());
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << rc;
    return rc;
}

//  ---------------------- ResourceEditorToolWindow

class ResourceEditorToolWindow: public QDesignerToolWindow
{
public:
    explicit ResourceEditorToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;
};

ResourceEditorToolWindow::ResourceEditorToolWindow(QDesignerWorkbench *workbench)  :
    QDesignerToolWindow(workbench,
                        QDesignerComponents::createResourceEditor(workbench->core(), 0),
                        QLatin1String("qt_designer_resourceeditor"),
                        QDesignerToolWindow::tr("Resource Browser"),
                        QLatin1String("__qt_resource_editor_tool_action"),
                        Qt::RightDockWidgetArea)
{
}

QRect ResourceEditorToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();

    const QSize sz(g.width() * 1/3, g.height() * 1/6);
    QRect r(QPoint(0, 0), sz);
    r.moveCenter(g.center());
    r.moveBottom(g.bottom() - margin);
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << r;
    return r;
}

//  ---------------------- SignalSlotEditorToolWindow

class SignalSlotEditorToolWindow: public QDesignerToolWindow
{
public:
    explicit SignalSlotEditorToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;
};

SignalSlotEditorToolWindow::SignalSlotEditorToolWindow(QDesignerWorkbench *workbench) :
    QDesignerToolWindow(workbench,
                        QDesignerComponents::createSignalSlotEditor(workbench->core(), 0),
                        QLatin1String("qt_designer_signalsloteditor"),
                        QDesignerToolWindow::tr("Signal/Slot Editor"),
                        QLatin1String("__qt_signal_slot_editor_tool_action"),
                        Qt::RightDockWidgetArea)
{
}

QRect SignalSlotEditorToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();

    const QSize sz(g.width() * 1/3, g.height() * 1/6);
    QRect r(QPoint(0, 0), sz);
    r.moveCenter(g.center());
    r.moveTop(margin + g.top());
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << r;
    return r;
}

//  ---------------------- WidgetBoxToolWindow

static inline QWidget *createWidgetBox(QDesignerFormEditorInterface *core, QWidget *parent = 0)
{
    QDesignerWidgetBoxInterface *widget = QDesignerComponents::createWidgetBox(core, parent);
    core->setWidgetBox(widget);
    return widget;
}

class WidgetBoxToolWindow: public QDesignerToolWindow
{
public:
    explicit WidgetBoxToolWindow(QDesignerWorkbench *workbench);

    virtual QRect geometryHint() const;
};

WidgetBoxToolWindow::WidgetBoxToolWindow(QDesignerWorkbench *workbench) :
    QDesignerToolWindow(workbench,
                        createWidgetBox(workbench->core()),
                        QLatin1String("qt_designer_widgetbox"),
                        QDesignerToolWindow::tr("Widget Box"),
                        QLatin1String("__qt_widget_box_tool_action"),
                        Qt::LeftDockWidgetArea)
{
}

QRect WidgetBoxToolWindow::geometryHint() const
{
    const QRect g = availableToolWindowGeometry();
    const int margin = workbench()->marginHint();
    const  QRect rc = QRect(g.left() + margin,
                            g.top() + margin,
                            g.width() * 1/4, g.height() * 5/6);
    if (debugToolWindow)
        qDebug() << Q_FUNC_INFO << rc;
    return rc;
}

// -- Factory
QDesignerToolWindow *QDesignerToolWindow::createStandardToolWindow(StandardToolWindow which,
                                                                   QDesignerWorkbench *workbench)
{
    switch (which) {
    case ActionEditor:
        return new ActionEditorToolWindow(workbench);
    case ResourceEditor:
        return new ResourceEditorToolWindow(workbench);
    case SignalSlotEditor:
        return new SignalSlotEditorToolWindow(workbench);
    case PropertyEditor:
        return new PropertyEditorToolWindow(workbench);
    case ObjectInspector:
        return new ObjectInspectorToolWindow(workbench);
    case WidgetBox:
        return new WidgetBoxToolWindow(workbench);
    default:
        break;
    }
    return 0;
}


QT_END_NAMESPACE
