/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "qttoolbardialog.h"
#include "ui_qttoolbardialog.h"

#include <QtCore/QSet>
#include <QtGui/QtEvents>
#include <QtGui/QAction>
#include <QtGui/QToolBar>
#include <QtGui/QMainWindow>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class QtFullToolBarManagerPrivate;

class QtFullToolBarManager : public QObject
{
    Q_OBJECT
public:
    QtFullToolBarManager(QObject *parent);
    ~QtFullToolBarManager();

    void setMainWindow(QMainWindow *mainWindow);
    QMainWindow *mainWindow() const;

    void addCategory(const QString &category);
    bool hasCategory(const QString &category) const;
    QStringList categories() const;
    QList<QAction *> categoryActions(const QString &category) const;
    QString actionCategory(QAction *action) const;

    // only non-separator
    void addAction(QAction *action, const QString &category);

    void removeAction(QAction *action);

    QSet<QAction *> actions() const;
    bool isWidgetAction(QAction *action) const;

    /*
    Adds (registers) toolBar. Adds (registers) actions that already exists in toolBar.
    Remembers toolbar and its actions as a default.
    */
    void addDefaultToolBar(QToolBar *toolBar, const QString &category);

    void removeDefaultToolBar(QToolBar *toolBar);
    // NULL on action list means separator.
    QMap<QToolBar *, QList<QAction *> > defaultToolBars() const;
    bool isDefaultToolBar(QToolBar *toolBar) const;

    QToolBar *createToolBar(const QString &toolBarName);
    void deleteToolBar(QToolBar *toolBar); // only those which were created, not added

    QList<QAction *> actions(QToolBar *toolBar) const;

    void setToolBars(const QMap<QToolBar *, QList<QAction *> > &actions);
    void setToolBar(QToolBar *toolBar, const QList<QAction *> &actions);

    QMap<QToolBar *, QList<QAction *> > toolBarsActions() const;
    QByteArray saveState(int version = 0) const;
    bool restoreState(const QByteArray &state, int version = 0);

public slots:

    void resetToolBar(QToolBar *toolBar);
    void resetAllToolBars();

signals:
    void toolBarCreated(QToolBar *toolBar);
    void toolBarRemoved(QToolBar *toolBar);

    /*
    If QToolBarWidgetAction was in another tool bar and is inserted into
    this toolBar, toolBarChanged is first emitted for other toolbar - without
    that action. (Another approach may be that user first must call setToolBar
    without that action for old tool bar)
    */
    void toolBarChanged(QToolBar *toolBar, const QList<QAction *> &actions);

private:
    QScopedPointer<QtFullToolBarManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtFullToolBarManager)
    Q_DISABLE_COPY(QtFullToolBarManager)
};

class QtFullToolBarManagerPrivate
{
    class QtFullToolBarManager *q_ptr;
    Q_DECLARE_PUBLIC(QtFullToolBarManager)

public:

    QToolBar *toolBarWidgetAction(QAction *action) const;
    void removeWidgetActions(const QMap<QToolBar *, QList<QAction *> > &actions);

    enum {
        VersionMarker = 0xff,
        ToolBarMarker = 0xfe,
        CustomToolBarMarker = 0xfd,
    };

    void saveState(QDataStream &stream) const;
    bool restoreState(QDataStream &stream) const;
    QToolBar *findDefaultToolBar(const QString &objectName) const;
    QAction *findAction(const QString &actionName) const;

    QToolBar *toolBarByName(const QString &toolBarName) const;

    QtFullToolBarManagerPrivate();

    QMap<QString, QList<QAction *> > categoryToActions;
    QMap<QAction *, QString>         actionToCategory;

    QSet<QAction *> allActions;
    QMap<QAction *, QToolBar *> widgetActions;
    QSet<QAction *> regularActions;
    QMap<QAction *, QList<QToolBar *> > actionToToolBars;

    QMap<QToolBar *, QList<QAction *> > toolBars;
    QMap<QToolBar *, QList<QAction *> > toolBarsWithSeparators;
    QMap<QToolBar *, QList<QAction *> > defaultToolBars;
    QList<QToolBar *> customToolBars;

    QMainWindow *theMainWindow;
};




QtFullToolBarManagerPrivate::QtFullToolBarManagerPrivate()
    : theMainWindow(0)
{
}

QToolBar *QtFullToolBarManagerPrivate::toolBarWidgetAction(QAction *action) const
{
    if (widgetActions.contains(action))
        return widgetActions.value(action);
    return 0;
}

void QtFullToolBarManagerPrivate::removeWidgetActions(const QMap<QToolBar *, QList<QAction *> >
            &actions)
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar = actions.constBegin();
    while (itToolBar != actions.constEnd()) {
        QToolBar *toolBar = itToolBar.key();
        QList<QAction *> newActions = toolBars.value(toolBar);
        QList<QAction *> newActionsWithSeparators = toolBarsWithSeparators.value(toolBar);

        QList<QAction *> removedActions;
        QList<QAction *> actionList = itToolBar.value();
        QListIterator<QAction *> itAction(actionList);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            if (newActions.contains(action) && toolBarWidgetAction(action) == toolBar) {
                newActions.removeAll(action);
                newActionsWithSeparators.removeAll(action);
                removedActions.append(action);
            }
        }

        //emit q_ptr->toolBarChanged(toolBar, newActions);

        toolBars.insert(toolBar, newActions);
        toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
        QListIterator<QAction *> itRemovedAction(removedActions);
        while (itRemovedAction.hasNext()) {
            QAction *oldAction = itRemovedAction.next();
            widgetActions.insert(oldAction, 0);
            actionToToolBars[oldAction].removeAll(toolBar);
        }

        ++itToolBar;
    }
}

void QtFullToolBarManagerPrivate::saveState(QDataStream &stream) const
{
    stream << (uchar) ToolBarMarker;
    stream << defaultToolBars.size();
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar =
                defaultToolBars.constBegin();
    while (itToolBar != defaultToolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (tb->objectName().isEmpty()) {
            qWarning("QtToolBarManager::saveState(): 'objectName' not set for QToolBar "
                "%p '%s', using 'windowTitle' instead",
            tb, tb->windowTitle().toLocal8Bit().constData());
            stream << tb->windowTitle();
        } else {
            stream << tb->objectName();
        }

        stream << toolBars[tb].size();
        QListIterator<QAction *> itAction(toolBars[tb]);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();

            if (action) {
                if (action->objectName().isEmpty()) {
                    qWarning("QtToolBarManager::saveState(): 'objectName' not set for QAction "
                                "%p '%s', using 'text' instead",
                            action, action->text().toLocal8Bit().constData());
                    stream << action->text();
                } else {
                    stream << action->objectName();
                }
            } else {
                stream << QString();
            }
        }
        ++itToolBar;
    }


    stream << (uchar) CustomToolBarMarker;
    stream << toolBars.size() - defaultToolBars.size();
    itToolBar = toolBars.constBegin();
    while (itToolBar != toolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (!defaultToolBars.contains(tb)) {
            stream << tb->objectName();
            stream << tb->windowTitle();

            stream << toolBars[tb].size();
            QListIterator<QAction *> itAction(toolBars[tb]);
            while (itAction.hasNext()) {
                QAction *action = itAction.next();

                if (action) {
                    if (action->objectName().isEmpty()) {
                        qWarning("QtToolBarManager::saveState(): 'objectName' not set for QAction "
                                    "%p '%s', using 'text' instead",
                                action, action->text().toLocal8Bit().constData());
                        stream << action->text();
                    } else {
                        stream << action->objectName();
                    }
                } else {
                    stream << QString();
                }
            }
        }
        ++itToolBar;
    }
}

bool QtFullToolBarManagerPrivate::restoreState(QDataStream &stream) const
{
    uchar tmarker;
    stream >> tmarker;
    if (tmarker != ToolBarMarker)
        return false;

    int toolBars;
    stream >> toolBars;
    for (int i = 0; i < toolBars; i++) {
        QString objectName;
        stream >> objectName;
        int actionCount;
        stream >> actionCount;
        QList<QAction *> actions;
        for (int j = 0; j < actionCount; j++) {
            QString actionName;
            stream >> actionName;

            if (actionName.isEmpty())
                actions.append(0);
            else {
                QAction *action = findAction(actionName);
                if (action)
                    actions.append(action);
            }
        }

        QToolBar *toolBar = findDefaultToolBar(objectName);
        if (toolBar)
            q_ptr->setToolBar(toolBar, actions);
    }



    uchar ctmarker;
    stream >> ctmarker;
    if (ctmarker != CustomToolBarMarker)
        return false;

    QList<QToolBar *> oldCustomToolBars = customToolBars;

    stream >> toolBars;
    for (int i = 0; i < toolBars; i++) {
        QString objectName;
        QString toolBarName;
        int actionCount;
        stream >> objectName;
        stream >> toolBarName;
        stream >> actionCount;
        QList<QAction *> actions;
        for (int j = 0; j < actionCount; j++) {
            QString actionName;
            stream >> actionName;

            if (actionName.isEmpty())
                actions.append(0);
            else {
                QAction *action = findAction(actionName);
                if (action)
                    actions.append(action);
            }
        }

        QToolBar *toolBar = toolBarByName(objectName);
        if (toolBar) {
            toolBar->setWindowTitle(toolBarName);
            oldCustomToolBars.removeAll(toolBar);
        }
        else
            toolBar = q_ptr->createToolBar(toolBarName);
        if (toolBar) {
            toolBar->setObjectName(objectName);
            q_ptr->setToolBar(toolBar, actions);
        }
    }
    QListIterator<QToolBar *> itToolBar(oldCustomToolBars);
    while (itToolBar.hasNext())
        q_ptr->deleteToolBar(itToolBar.next());
    return true;
}

QToolBar *QtFullToolBarManagerPrivate::findDefaultToolBar(const QString &objectName) const
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar =
                defaultToolBars.constBegin();
    while (itToolBar != defaultToolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (tb->objectName() == objectName)
            return tb;

        ++itToolBar;
    }

    qWarning("QtToolBarManager::restoreState(): cannot find a QToolBar named "
        "'%s', trying to match using 'windowTitle' instead.",
        objectName.toLocal8Bit().constData());

    itToolBar = defaultToolBars.constBegin();
    while (itToolBar != defaultToolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (tb->windowTitle() == objectName)
            return tb;

        ++itToolBar;
    }
    qWarning("QtToolBarManager::restoreState(): cannot find a QToolBar with "
        "matching 'windowTitle' (looking for '%s').",
        objectName.toLocal8Bit().constData());

    return 0;
}

QAction *QtFullToolBarManagerPrivate::findAction(const QString &actionName) const
{
    QSetIterator<QAction *> itAction(allActions);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();

        if (action->objectName() == actionName)
            return action;
    }
    qWarning("QtToolBarManager::restoreState(): cannot find a QAction named "
        "'%s', trying to match using 'text' instead.",
        actionName.toLocal8Bit().constData());

    itAction.toFront();
    while (itAction.hasNext()) {
        QAction *action = itAction.next();

        if (action->text() == actionName)
            return action;
    }
    qWarning("QtToolBarManager::restoreState(): cannot find a QAction with "
        "matching 'text' (looking for '%s').",
        actionName.toLocal8Bit().constData());

    return 0;
}

QToolBar *QtFullToolBarManagerPrivate::toolBarByName(const QString &toolBarName) const
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar = toolBars.constBegin();
    while (itToolBar != toolBars.constEnd()) {
        QToolBar *toolBar = itToolBar.key();
        if (toolBar->objectName() == toolBarName)
            return toolBar;

        ++itToolBar;
    }
    return 0;
}

//////////////////////////////

QtFullToolBarManager::QtFullToolBarManager(QObject *parent)
    : QObject(parent), d_ptr(new QtFullToolBarManagerPrivate)
{
    d_ptr->q_ptr = this;
}

QtFullToolBarManager::~QtFullToolBarManager()
{
}

void QtFullToolBarManager::setMainWindow(QMainWindow *mainWindow)
{
    d_ptr->theMainWindow = mainWindow;
}

QMainWindow *QtFullToolBarManager::mainWindow() const
{
    return d_ptr->theMainWindow;
}

void QtFullToolBarManager::addCategory(const QString &category)
{
    d_ptr->categoryToActions[category] = QList<QAction *>();
}

bool QtFullToolBarManager::hasCategory(const QString &category) const
{
    return d_ptr->categoryToActions.contains(category);
}

QStringList QtFullToolBarManager::categories() const
{
    return d_ptr->categoryToActions.keys();
}

QList<QAction *> QtFullToolBarManager::categoryActions(const QString &category) const
{
    QMap<QString, QList<QAction *> >::ConstIterator it =
                d_ptr->categoryToActions.find(category);
    if (it != d_ptr->categoryToActions.constEnd())
        return it.value();
    return QList<QAction *>();
}

QString QtFullToolBarManager::actionCategory(QAction *action) const
{
    QMap<QAction *, QString>::ConstIterator it = d_ptr->actionToCategory.find(action);
    if (it != d_ptr->actionToCategory.constEnd())
        return it.value();
    return QString();
}

void QtFullToolBarManager::addAction(QAction *action, const QString &category)
{
    if (!action)
        return;
    if (action->isSeparator())
        return;
    if (d_ptr->allActions.contains(action))
        return;
    if (QLatin1String(action->metaObject()->className()) ==
                QLatin1String("QToolBarWidgetAction"))
        d_ptr->widgetActions.insert(action, 0);
    else
        d_ptr->regularActions.insert(action);
    d_ptr->allActions.insert(action);
    d_ptr->categoryToActions[category].append(action);
    d_ptr->actionToCategory[action] = category;
}

void QtFullToolBarManager::removeAction(QAction *action)
{
    if (!d_ptr->allActions.contains(action))
        return;

    QList<QToolBar *> toolBars = d_ptr->actionToToolBars[action];
    QListIterator<QToolBar *> itToolBar(toolBars);
    while (itToolBar.hasNext()) {
        QToolBar *toolBar = itToolBar.next();

        d_ptr->toolBars[toolBar].removeAll(action);
        d_ptr->toolBarsWithSeparators[toolBar].removeAll(action);

        toolBar->removeAction(action);
    }

    QMap<QToolBar *, QList<QAction *> >::ConstIterator itDefault =
            d_ptr->defaultToolBars.constBegin();
    while (itDefault != d_ptr->defaultToolBars.constEnd()) {
        if (itDefault.value().contains(action))
            d_ptr->defaultToolBars[itDefault.key()].removeAll(action);

        itDefault++;
    }

    d_ptr->allActions.remove(action);
    d_ptr->widgetActions.remove(action);
    d_ptr->regularActions.remove(action);
    d_ptr->actionToToolBars.remove(action);

    QString category = d_ptr->actionToCategory.value(action);
    d_ptr->actionToCategory.remove(action);
    d_ptr->categoryToActions[category].removeAll(action);

    if (d_ptr->categoryToActions[category].isEmpty())
        d_ptr->categoryToActions.remove(category);
}

QSet<QAction *> QtFullToolBarManager::actions() const
{
    return d_ptr->allActions;
}

bool QtFullToolBarManager::isWidgetAction(QAction *action) const
{
    if (d_ptr->widgetActions.contains(action))
        return true;
    return false;
}

void QtFullToolBarManager::addDefaultToolBar(QToolBar *toolBar, const QString &category)
{
    if (!toolBar)
        return;
    if (d_ptr->toolBars.contains(toolBar))
        return;
    // could be also checked if toolBar belongs to mainwindow

    QList<QAction *> newActionsWithSeparators;
    QList<QAction *> newActions;
    QList<QAction *> actions = toolBar->actions();
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        addAction(action, category);
        if (d_ptr->widgetActions.contains(action))
            d_ptr->widgetActions.insert(action, toolBar);
        newActionsWithSeparators.append(action);
        if (action->isSeparator())
            action = 0;
        else
            d_ptr->actionToToolBars[action].append(toolBar);
        newActions.append(action);
    }
    d_ptr->defaultToolBars.insert(toolBar, newActions);
    //Below could be done by call setToolBar() if we want signal emission here.
    d_ptr->toolBars.insert(toolBar, newActions);
    d_ptr->toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

void QtFullToolBarManager::removeDefaultToolBar(QToolBar *toolBar)
{
    if (!d_ptr->defaultToolBars.contains(toolBar))
        return;

    QList<QAction *> defaultActions = d_ptr->defaultToolBars[toolBar];
    setToolBar(toolBar, QList<QAction *>());
    QListIterator<QAction *> itAction(defaultActions);
    while (itAction.hasNext())
        removeAction(itAction.next());

    d_ptr->toolBars.remove(toolBar);
    d_ptr->toolBarsWithSeparators.remove(toolBar);
    d_ptr->defaultToolBars.remove(toolBar);

    itAction.toFront();
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        if (action)
            toolBar->insertAction(0, action);
        else
            toolBar->insertSeparator(0);
    }
}

QMap<QToolBar *, QList<QAction *> > QtFullToolBarManager::defaultToolBars() const
{
    return d_ptr->defaultToolBars;
}

bool QtFullToolBarManager::isDefaultToolBar(QToolBar *toolBar) const
{
    if (d_ptr->defaultToolBars.contains(toolBar))
        return true;
    return false;
}

QToolBar *QtFullToolBarManager::createToolBar(const QString &toolBarName)
{
    if (!mainWindow())
        return 0;
    QToolBar *toolBar = new QToolBar(toolBarName, mainWindow());
    int i = 1;
    const QString prefix = QLatin1String("_Custom_Toolbar_%1");
    QString name = prefix.arg(i);
    while (d_ptr->toolBarByName(name))
        name = prefix.arg(++i);
    toolBar->setObjectName(name);
    mainWindow()->addToolBar(toolBar);
    d_ptr->customToolBars.append(toolBar);
    d_ptr->toolBars.insert(toolBar, QList<QAction *>());
    d_ptr->toolBarsWithSeparators.insert(toolBar, QList<QAction *>());
    return toolBar;
}

void QtFullToolBarManager::deleteToolBar(QToolBar *toolBar)
{
    if (!d_ptr->toolBars.contains(toolBar))
        return;
    if (d_ptr->defaultToolBars.contains(toolBar))
        return;
    setToolBar(toolBar, QList<QAction *>());
    d_ptr->customToolBars.removeAll(toolBar);
    d_ptr->toolBars.remove(toolBar);
    d_ptr->toolBarsWithSeparators.remove(toolBar);
    delete toolBar;
}

QList<QAction *> QtFullToolBarManager::actions(QToolBar *toolBar) const
{
    if (d_ptr->toolBars.contains(toolBar))
        return d_ptr->toolBars.value(toolBar);
    return QList<QAction *>();
}

void QtFullToolBarManager::setToolBars(const QMap<QToolBar *, QList<QAction *> > &actions)
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator it = actions.constBegin();
    while (it != actions.constEnd()) {
        setToolBar(it.key(), it.value());
        ++it;
    }
}

void QtFullToolBarManager::setToolBar(QToolBar *toolBar, const QList<QAction *> &actions)
{
    if (!toolBar)
        return;
    if (!d_ptr->toolBars.contains(toolBar))
        return;

    if (actions == d_ptr->toolBars[toolBar])
        return;

    QMap<QToolBar *, QList<QAction *> > toRemove;

    QList<QAction *> newActions;
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        if (!action || (!newActions.contains(action) && d_ptr->allActions.contains(action)))
            newActions.append(action);

        QToolBar *oldToolBar = d_ptr->toolBarWidgetAction(action);
        if (oldToolBar && oldToolBar != toolBar)
            toRemove[oldToolBar].append(action);
    }

    d_ptr->removeWidgetActions(toRemove);

    QList<QAction *> oldActions = d_ptr->toolBarsWithSeparators.value(toolBar);
    QListIterator<QAction *> itOldAction(oldActions);
    while (itOldAction.hasNext()) {
        QAction *action = itOldAction.next();
        /*
        When addDefaultToolBar() separator actions could be checked if they are
        inserted in other toolbars - if yes then create new one.
        */
        if (d_ptr->toolBarWidgetAction(action) == toolBar)
            d_ptr->widgetActions.insert(action, 0);
        toolBar->removeAction(action);
        if (action->isSeparator())
            delete action;
        else
            d_ptr->actionToToolBars[action].removeAll(toolBar);
    }

    QList<QAction *> newActionsWithSeparators;
    QListIterator<QAction *> itNewActions(newActions);
    while (itNewActions.hasNext()) {
        QAction *action = itNewActions.next();
        QAction *newAction = 0;
        if (!action)
            newAction = toolBar->insertSeparator(0);
        if (d_ptr->allActions.contains(action)) {
            toolBar->insertAction(0, action);
            newAction = action;
            d_ptr->actionToToolBars[action].append(toolBar);
        }
        newActionsWithSeparators.append(newAction);
    }
    d_ptr->toolBars.insert(toolBar, newActions);
    d_ptr->toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

QMap<QToolBar *, QList<QAction *> > QtFullToolBarManager::toolBarsActions() const
{
    return d_ptr->toolBars;
}

void QtFullToolBarManager::resetToolBar(QToolBar *toolBar)
{
    if (!isDefaultToolBar(toolBar))
        return;
    setToolBar(toolBar, defaultToolBars().value(toolBar));
}

void QtFullToolBarManager::resetAllToolBars()
{
    setToolBars(defaultToolBars());
    QList<QToolBar *> oldCustomToolBars = d_ptr->customToolBars;
    QListIterator<QToolBar *> itToolBar(oldCustomToolBars);
    while (itToolBar.hasNext()) {
        deleteToolBar(itToolBar.next());
    }
}

QByteArray QtFullToolBarManager::saveState(int version) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << QtFullToolBarManagerPrivate::VersionMarker;
    stream << version;
    d_ptr->saveState(stream);
    return data;
}

bool QtFullToolBarManager::restoreState(const QByteArray &state, int version)
{
    QByteArray sd = state;
    QDataStream stream(&sd, QIODevice::ReadOnly);
    int marker, v;
    stream >> marker;
    stream >> v;
    if (marker != QtFullToolBarManagerPrivate::VersionMarker || v != version)
        return false;
    return d_ptr->restoreState(stream);
}


class QtToolBarManagerPrivate
{
    class QtToolBarManager *q_ptr;
    Q_DECLARE_PUBLIC(QtToolBarManager)
public:
    QtFullToolBarManager *manager;
};

//////////////////////////////////////

/*! \class QtToolBarManager
    \internal
    \inmodule QtDesigner
    \since 4.4

    \brief The QtToolBarManager class provides toolbar management for
    main windows.

    The QtToolBarManager is typically used with a QtToolBarDialog
    which allows the user to customize the toolbars for a given main
    window. The QtToolBarDialog class's functionality is controlled by
    an instance of the QtToolBarManager class, and the main window is
    specified using the QtToolBarManager class's setMainWindow()
    function.

    The currently specified main window can be retrieved using the
    mainWindow() function.

    The toolbar manager holds lists of the given main window's actions
    and toolbars, and can add actions and toolbars to these
    lists using the addAction() and addToolBar() functions
    respectively. The actions can in addition be categorized
    acccording to the user's preferences. The toolbar manager can also
    remove custom actions and toolbars using the removeAction() and
    removeToolBar() functions.

    Finally, the QtToolBarManager is able to save the customized state
    of its toolbars using the saveState() function as well as restore
    the toolbars' saved state using restoreState() function.

    \sa QtToolBarDialog
*/

/*!
    Creates a toolbar manager with the given \a parent.
*/
QtToolBarManager::QtToolBarManager(QObject *parent)
    : QObject(parent), d_ptr(new QtToolBarManagerPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->manager = new QtFullToolBarManager(this);
}

/*!
    Destroys the toolbar manager.
*/
QtToolBarManager::~QtToolBarManager()
{
}

/*!
    Sets the main window upon which the toolbar manager operates, to
    be the given \a mainWindow.
*/
void QtToolBarManager::setMainWindow(QMainWindow *mainWindow)
{
    d_ptr->manager->setMainWindow(mainWindow);
}

/*!
    Returns the main window associated this toolbar manager.
*/
QMainWindow *QtToolBarManager::mainWindow() const
{
    return d_ptr->manager->mainWindow();
}

/*!
    Adds the given \a action to the given \a category in the manager's
    list of actions. If the \a category doesn't exist it is created.
    Only non separator actions can be added. If the action is already
    added to the list, the function doesn't do anything.

    \sa removeAction()
*/
void QtToolBarManager::addAction(QAction *action, const QString &category)
{
    d_ptr->manager->addAction(action, category);
}

/*!
    Removes the specified \a action from the manager's list of
    actions. The action is also removed from all the registered
    toolbars.  If the specified \a action is the only action in its
    category, that category is removed as well.

    \sa addAction()
*/
void QtToolBarManager::removeAction(QAction *action)
{
    d_ptr->manager->removeAction(action);
}

/*!
    Adds the given \a toolBar to the manager's toolbar list.

    All the \a toolBar's actions are automatically added to the given
    \a category in the manager's list of actions if they're not
    already there. The manager remembers which toolbar the actions
    belonged to, so, when the \a toolBar is removed, its actions will
    be removed as well.

    Custom toolbars are created with the main window returned by
    the mainWindow() function, as its parent.

    \sa removeToolBar()
*/
void QtToolBarManager::addToolBar(QToolBar *toolBar, const QString &category)
{
    d_ptr->manager->addDefaultToolBar(toolBar, category);
}

/*!
    Removes the specified \a toolBar from the manager's list. All the
    actions that existed in the specified \a toolBar when it was
    added are removed as well.

    \sa addToolBar()
*/
void QtToolBarManager::removeToolBar(QToolBar *toolBar)
{
    d_ptr->manager->removeDefaultToolBar(toolBar);
}

/*!
    Returns the manager's toolbar list.
*/
QList<QToolBar *> QtToolBarManager::toolBars() const
{
    return d_ptr->manager->toolBarsActions().keys();
}

/*
void QtToolBarManager::resetToolBar(QToolBar *toolBar)
{
    d_ptr->manager->resetToolBar(toolBar);
}

void QtToolBarManager::resetAllToolBars()
{
    d_ptr->manager->resetAllToolBars();
}
*/

/*!
    Saves the state of the toolbar manager's toolbars. The \a version
    number is stored as part of the data.

    Identifies all the QToolBar and QAction objects by their object
    name property. Ensure that this property is unique for each
    QToolBar and QAction that you add using the QtToolBarManager.

    Returns an identifier for the state which can be passed along with
    the version number to the restoreState() function to restore the
    saved state.

    \sa restoreState()
*/
QByteArray QtToolBarManager::saveState(int version) const
{
    return d_ptr->manager->saveState(version);
}

/*!
    Restores the saved state of the toolbar manager's toolbars.  The
    \a version number is compared with the version number of the
    stored \a state.

    Returns true if the version numbers are matching and the toolbar
    manager's state is restored; otherwise the toolbar manager's state
    is left unchanged and the function returns false.

    Note that the state of the toolbar manager's toolbars should be
    restored before restoring the state of the main window's toolbars
    and dockwidgets using the QMainWindow::restoreState() function. In
    that way the restoreState() function can create the custom
    toolbars before the QMainWindow::restoreState() function restores
    the custom toolbars' positions.

    \sa saveState()
*/
bool QtToolBarManager::restoreState(const QByteArray &state, int version)
{
    return d_ptr->manager->restoreState(state, version);
}

//////////////////////

class ToolBarItem {
public:
    ToolBarItem() : tb(0) {}
    ToolBarItem(QToolBar *toolBar) : tb(toolBar) {}
    ToolBarItem(QToolBar *toolBar, const QString &toolBarName)
            : tb(toolBar), tbName(toolBarName) {}
    ToolBarItem(const QString &toolBarName) : tb(0), tbName(toolBarName) {}
    QToolBar *toolBar() const
        { return tb; }
    void setToolBar(QToolBar *toolBar)
        { tb = toolBar; }
    QString toolBarName() const
        { return tbName; }
    void setToolBarName(const QString &toolBarName)
        { tbName = toolBarName; }
private:
    QToolBar *tb;
    QString tbName;
};

class QtToolBarDialogPrivate {
    QtToolBarDialog *q_ptr;
    Q_DECLARE_PUBLIC(QtToolBarDialog)
public:
    QtToolBarDialogPrivate()
        : toolBarManager(0),
          currentAction(0),
          currentToolBar(0)
          { }

    ToolBarItem *createItem(QToolBar *toolBar);
    ToolBarItem *createItem(const QString &toolBarName);
    void deleteItem(ToolBarItem *item);

    void newClicked();
    void removeClicked();
    void defaultClicked();
    void okClicked();
    void applyClicked();
    void cancelClicked();
    void upClicked();
    void downClicked();
    void leftClicked();
    void rightClicked();
    void renameClicked();
    void toolBarRenamed(QListWidgetItem *item);
    void currentActionChanged(QTreeWidgetItem *current);
    void currentToolBarChanged(QListWidgetItem *current);
    void currentToolBarActionChanged(QListWidgetItem *current);

    void removeToolBar(ToolBarItem *item);
    bool isDefaultToolBar(ToolBarItem *item) const;
    void setButtons();
    void clearOld();
    void fillNew();
    QtFullToolBarManager *toolBarManager;
    QMap<ToolBarItem *, QList<QAction *> > currentState;
    QMap<QToolBar *, ToolBarItem *> toolBarItems;
    QSet<ToolBarItem *> createdItems;
    QSet<ToolBarItem *> removedItems;

    QSet<ToolBarItem *> allToolBarItems;

    // static
    QTreeWidgetItem *currentAction;
    QMap<QAction *, QTreeWidgetItem *> actionToItem;
    QMap<QTreeWidgetItem *, QAction *> itemToAction;

    // dynamic
    ToolBarItem *currentToolBar;
    QMap<ToolBarItem *, QListWidgetItem *> toolBarToItem;
    QMap<QListWidgetItem *, ToolBarItem *> itemToToolBar;

    // dynamic
    QMap<QAction *, QListWidgetItem *> actionToCurrentItem;
    QMap<QListWidgetItem *, QAction *> currentItemToAction;

    QMap<QAction *, ToolBarItem *> widgetActionToToolBar;
    QMap<ToolBarItem *, QSet<QAction *> > toolBarToWidgetActions;

    QString separatorText;
    Ui::QtToolBarDialog ui;
};

ToolBarItem *QtToolBarDialogPrivate::createItem(QToolBar *toolBar)
{
    if (!toolBar)
        return 0;
    ToolBarItem *item = new ToolBarItem(toolBar, toolBar->windowTitle());
    allToolBarItems.insert(item);
    return item;
}

ToolBarItem *QtToolBarDialogPrivate::createItem(const QString &toolBarName)
{
    ToolBarItem *item = new ToolBarItem(toolBarName);
    allToolBarItems.insert(item);
    return item;
}

void QtToolBarDialogPrivate::deleteItem(ToolBarItem *item)
{
    if (!allToolBarItems.contains(item))
        return;
    allToolBarItems.remove(item);
    delete item;
}

void QtToolBarDialogPrivate::clearOld()
{
    ui.actionTree->clear();
    ui.toolBarList->clear();
    ui.currentToolBarList->clear();
    ui.removeButton->setEnabled(false);
    ui.newButton->setEnabled(false);
    ui.upButton->setEnabled(false);
    ui.downButton->setEnabled(false);
    ui.leftButton->setEnabled(false);
    ui.rightButton->setEnabled(false);

    actionToItem.clear();
    itemToAction.clear();
    toolBarToItem.clear();
    itemToToolBar.clear();
    actionToCurrentItem.clear();
    currentItemToAction.clear();
    widgetActionToToolBar.clear();
    toolBarToWidgetActions.clear();

    toolBarItems.clear();
    currentState.clear();
    createdItems.clear();
    removedItems.clear();
    QSetIterator<ToolBarItem *> itItem(allToolBarItems);
    while (itItem.hasNext())
        delete itItem.next();
    allToolBarItems.clear();

    currentToolBar = 0;
    currentAction = 0;
}

void QtToolBarDialogPrivate::fillNew()
{
    if (!toolBarManager)
        return;

    QTreeWidgetItem *item = new QTreeWidgetItem(ui.actionTree);
    item->setText(0, separatorText);
    ui.actionTree->setCurrentItem(item);
    currentAction = item;
    actionToItem.insert(0, item);
    itemToAction.insert(item, 0);
    QStringList categories = toolBarManager->categories();
    QStringListIterator itCategory(categories);
    while (itCategory.hasNext()) {
        QString category = itCategory.next();
        QTreeWidgetItem *categoryItem = new QTreeWidgetItem(ui.actionTree);
        categoryItem->setText(0, category);
        QList<QAction *> actions = toolBarManager->categoryActions(category);
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            item = new QTreeWidgetItem(categoryItem);
            item->setText(0, action->text());
            item->setIcon(0, action->icon());
            item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic);
            actionToItem.insert(action, item);
            itemToAction.insert(item, action);
            if (toolBarManager->isWidgetAction(action)) {
                item->setData(0, Qt::TextColorRole, QColor(Qt::blue));
                widgetActionToToolBar.insert(action, 0);
            }
            item->setFlags(item->flags() | Qt::ItemIsDragEnabled);
        }
        ui.actionTree->setItemExpanded(categoryItem, true);
    }
    //ui.actionTree->sortItems(0, Qt::AscendingOrder);

    QMap<QToolBar *, QList<QAction *> > toolBars = toolBarManager->toolBarsActions();
    QMap<QToolBar *, QList<QAction *> >::ConstIterator it = toolBars.constBegin();
    while (it != toolBars.constEnd()) {
        QToolBar *toolBar = it.key();
        ToolBarItem *tbItem = createItem(toolBar);
        toolBarItems.insert(toolBar, tbItem);
        QListWidgetItem *item = new QListWidgetItem(toolBar->windowTitle(),
                ui.toolBarList);
        toolBarToItem.insert(tbItem, item);
        itemToToolBar.insert(item, tbItem);
        QList<QAction *> actions = it.value();
        QListIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            if (toolBarManager->isWidgetAction(action)) {
                widgetActionToToolBar.insert(action, tbItem);
                toolBarToWidgetActions[tbItem].insert(action);
            }
        }
        currentState.insert(tbItem, actions);
        if (it == toolBars.constBegin())
            ui.toolBarList->setCurrentItem(item);
        if (isDefaultToolBar(tbItem))
            item->setData(Qt::TextColorRole, QColor(Qt::darkGreen));
        else
            item->setFlags(item->flags() | Qt::ItemIsEditable);

        ++it;
    }
    ui.toolBarList->sortItems();
    setButtons();
}

bool QtToolBarDialogPrivate::isDefaultToolBar(ToolBarItem *item) const
{
    if (!item)
        return false;
    if (!item->toolBar())
        return false;
    return toolBarManager->isDefaultToolBar(item->toolBar());
}

void QtToolBarDialogPrivate::setButtons()
{
    bool newEnabled = false;
    bool removeEnabled = false;
    bool renameEnabled = false;
    bool upEnabled = false;
    bool downEnabled = false;
    bool leftEnabled = false;
    bool rightEnabled = false;

    if (toolBarManager) {
        newEnabled = true;
        removeEnabled = !isDefaultToolBar(currentToolBar);
        renameEnabled = removeEnabled;
        QListWidgetItem *currentToolBarAction = ui.currentToolBarList->currentItem();
        if (currentToolBarAction) {
            int row = ui.currentToolBarList->row(currentToolBarAction);
            upEnabled = row > 0;
            downEnabled = row < ui.currentToolBarList->count() - 1;
            leftEnabled = true;
        }
        if (currentAction && currentToolBar)
            rightEnabled = true;
    }
    ui.newButton->setEnabled(newEnabled);
    ui.removeButton->setEnabled(removeEnabled);
    ui.renameButton->setEnabled(renameEnabled);
    ui.upButton->setEnabled(upEnabled);
    ui.downButton->setEnabled(downEnabled);
    ui.leftButton->setEnabled(leftEnabled);
    ui.rightButton->setEnabled(rightEnabled);
}

void QtToolBarDialogPrivate::newClicked()
{
    QString toolBarName = QtToolBarDialog::tr("Custom Toolbar"); // = QInputDialog::getString();
    // produce unique name
    ToolBarItem *item = createItem(toolBarName);
    currentState.insert(item, QList<QAction *>());
    createdItems.insert(item);
    QListWidgetItem *i = new QListWidgetItem(toolBarName, ui.toolBarList);
    i->setFlags(i->flags() | Qt::ItemIsEditable);
    ui.toolBarList->setCurrentItem(i);
    itemToToolBar.insert(i, item);
    toolBarToItem.insert(item, i);
    ui.toolBarList->sortItems();
    ui.toolBarList->setCurrentItem(i);
    currentToolBarChanged(i);
    renameClicked();
}

void QtToolBarDialogPrivate::removeToolBar(ToolBarItem *item)
{
    if (!item)
        return;
    if (item->toolBar() && toolBarManager->isDefaultToolBar(item->toolBar()))
        return;
    if (!toolBarToItem.contains(item))
        return;
    QListWidgetItem *i = toolBarToItem.value(item);
    bool wasCurrent = false;
    if (i == ui.toolBarList->currentItem())
        wasCurrent = true;
    int row = ui.toolBarList->row(i);
    QMap<ToolBarItem *, QSet<QAction *> >::ConstIterator itToolBar =
            toolBarToWidgetActions.find(item);
    if (itToolBar != toolBarToWidgetActions.constEnd()) {
        QSet<QAction *> actions = itToolBar.value();
        QSetIterator<QAction *> itAction(actions);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            widgetActionToToolBar.insert(action, 0);
        }
        toolBarToWidgetActions.remove(item);
    }

    currentState.remove(item);
    createdItems.remove(item);
    toolBarToItem.remove(item);
    itemToToolBar.remove(i);
    delete i;
    if (item->toolBar())
        removedItems.insert(item);
    else
        deleteItem(item);
    if (wasCurrent) {
        if (row == ui.toolBarList->count())
            row--;
        if (row < 0)
            ;
        else
            ui.toolBarList->setCurrentRow(row);
    }
    setButtons();
}

void QtToolBarDialogPrivate::removeClicked()
{
    QListWidgetItem *i = ui.toolBarList->currentItem();
    if (!i)
        return;
    ToolBarItem *item = itemToToolBar.value(i);
    removeToolBar(item);
}

void QtToolBarDialogPrivate::defaultClicked()
{
    QMap<QToolBar *, QList<QAction *> > defaultToolBars = toolBarManager->defaultToolBars();
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar = defaultToolBars.constBegin();
    while (itToolBar != defaultToolBars.constEnd()) {
        QToolBar *toolBar = itToolBar.key();
        ToolBarItem *toolBarItem = toolBarItems.value(toolBar);

        if (toolBarToWidgetActions.contains(toolBarItem)) {
            QSetIterator<QAction *> itAction(toolBarToWidgetActions.value(toolBarItem));
            while (itAction.hasNext())
                widgetActionToToolBar.insert(itAction.next(), 0);
            toolBarToWidgetActions.remove(toolBarItem);
        }

        currentState.remove(toolBarItem);

        QListIterator<QAction *> itAction(itToolBar.value());
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            if (toolBarManager->isWidgetAction(action)) {
                ToolBarItem *otherToolBar = widgetActionToToolBar.value(action);
                if (otherToolBar) {
                    toolBarToWidgetActions[otherToolBar].remove(action);
                    currentState[otherToolBar].removeAll(action);
                }
                widgetActionToToolBar.insert(action, toolBarItem);
                toolBarToWidgetActions[toolBarItem].insert(action);
            }
        }
        currentState.insert(toolBarItem, itToolBar.value());

        ++itToolBar;
    }
    currentToolBarChanged(toolBarToItem.value(currentToolBar));

    QList<ToolBarItem *> toolBars = currentState.keys();
    QListIterator<ToolBarItem *> itTb(toolBars);
    while (itTb.hasNext())
        removeToolBar(itTb.next());
}

void QtToolBarDialogPrivate::okClicked()
{
    applyClicked();
    q_ptr->accept();
}

void QtToolBarDialogPrivate::applyClicked()
{
    QMap<ToolBarItem *, QList<QAction *> > toolBars = currentState;
    QMap<ToolBarItem *, QList<QAction *> >::ConstIterator itToolBar = toolBars.constBegin();
    while (itToolBar != toolBars.constEnd()) {
        ToolBarItem *item = itToolBar.key();
        QToolBar *toolBar = item->toolBar();
        if (toolBar) {
            toolBarManager->setToolBar(toolBar, itToolBar.value());
            toolBar->setWindowTitle(item->toolBarName());
        }

        ++itToolBar;
    }

    QSet<ToolBarItem *> toRemove = removedItems;
    QSetIterator<ToolBarItem *> itRemove(toRemove);
    while (itRemove.hasNext()) {
        ToolBarItem *item = itRemove.next();
        QToolBar *toolBar = item->toolBar();
        removedItems.remove(item);
        currentState.remove(item);
        deleteItem(item);
        if (toolBar)
            toolBarManager->deleteToolBar(toolBar);
    }

    QSet<ToolBarItem *> toCreate = createdItems;
    QSetIterator<ToolBarItem *> itCreate(toCreate);
    while (itCreate.hasNext()) {
        ToolBarItem *item = itCreate.next();
        QString toolBarName = item->toolBarName();
        createdItems.remove(item);
        QList<QAction *> actions = currentState.value(item);
        QToolBar *toolBar = toolBarManager->createToolBar(toolBarName);
        item->setToolBar(toolBar);
        toolBarManager->setToolBar(toolBar, actions);
    }
}

void QtToolBarDialogPrivate::upClicked()
{
    QListWidgetItem *currentToolBarAction = ui.currentToolBarList->currentItem();
    if (!currentToolBarAction)
        return;
    int row = ui.currentToolBarList->row(currentToolBarAction);
    if (row == 0)
        return;
    ui.currentToolBarList->takeItem(row);
    int newRow = row - 1;
    ui.currentToolBarList->insertItem(newRow, currentToolBarAction);
    QList<QAction *> actions = currentState.value(currentToolBar);
    QAction *action = actions.at(row);
    actions.removeAt(row);
    actions.insert(newRow, action);
    currentState.insert(currentToolBar, actions);
    ui.currentToolBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

void QtToolBarDialogPrivate::downClicked()
{
    QListWidgetItem *currentToolBarAction = ui.currentToolBarList->currentItem();
    if (!currentToolBarAction)
        return;
    int row = ui.currentToolBarList->row(currentToolBarAction);
    if (row == ui.currentToolBarList->count() - 1)
        return;
    ui.currentToolBarList->takeItem(row);
    int newRow = row + 1;
    ui.currentToolBarList->insertItem(newRow, currentToolBarAction);
    QList<QAction *> actions = currentState.value(currentToolBar);
    QAction *action = actions.at(row);
    actions.removeAt(row);
    actions.insert(newRow, action);
    currentState.insert(currentToolBar, actions);
    ui.currentToolBarList->setCurrentItem(currentToolBarAction);
    setButtons();
}

void QtToolBarDialogPrivate::leftClicked()
{
    QListWidgetItem *currentToolBarAction = ui.currentToolBarList->currentItem();
    if (!currentToolBarAction)
        return;
    int row = ui.currentToolBarList->row(currentToolBarAction);
    currentState[currentToolBar].removeAt(row);
    QAction *action = currentItemToAction.value(currentToolBarAction);
    if (widgetActionToToolBar.contains(action)) {
        ToolBarItem *item = widgetActionToToolBar.value(action);
        if (item == currentToolBar) { // have to be
            toolBarToWidgetActions[item].remove(action);
            if (toolBarToWidgetActions[item].empty())
                toolBarToWidgetActions.remove(item);
        }
        widgetActionToToolBar.insert(action, 0);
    }
    if (action)
        actionToCurrentItem.remove(action);
    currentItemToAction.remove(currentToolBarAction);
    delete currentToolBarAction;
    if (row == ui.currentToolBarList->count())
        row--;
    if (row >= 0) {
        QListWidgetItem *item = ui.currentToolBarList->item(row);
        ui.currentToolBarList->setCurrentItem(item);
    }
    setButtons();
}

void QtToolBarDialogPrivate::rightClicked()
{
    if (!currentAction)
        return;
    if (!currentToolBar)
        return;
    QListWidgetItem *currentToolBarAction = ui.currentToolBarList->currentItem();

    QAction *action = itemToAction.value(currentAction);
    QListWidgetItem *item = 0;
    if (action) {
        if (currentState[currentToolBar].contains(action)) {
            item = actionToCurrentItem.value(action);
            if (item == currentToolBarAction)
                return;
            int row = ui.currentToolBarList->row(item);
            ui.currentToolBarList->takeItem(row);
            currentState[currentToolBar].removeAt(row);
            // only reorder here
        } else {
            item = new QListWidgetItem(action->text());
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic);
            currentItemToAction.insert(item, action);
            actionToCurrentItem.insert(action, item);
            if (widgetActionToToolBar.contains(action)) {
                item->setData(Qt::TextColorRole, QColor(Qt::blue));
                ToolBarItem *toolBar = widgetActionToToolBar.value(action);
                if (toolBar) {
                    currentState[toolBar].removeAll(action);
                    toolBarToWidgetActions[toolBar].remove(action);
                    if (toolBarToWidgetActions[toolBar].empty())
                        toolBarToWidgetActions.remove(toolBar);
                }
                widgetActionToToolBar.insert(action, currentToolBar);
                toolBarToWidgetActions[currentToolBar].insert(action);
            }
        }
    } else {
        item = new QListWidgetItem(separatorText);
        currentItemToAction.insert(item, 0);
    }

    int row = ui.currentToolBarList->count();
    if (currentToolBarAction) {
        row = ui.currentToolBarList->row(currentToolBarAction) + 1;
    }
    ui.currentToolBarList->insertItem(row, item);
    currentState[currentToolBar].insert(row, action);
    ui.currentToolBarList->setCurrentItem(item);

    setButtons();
}

void QtToolBarDialogPrivate::renameClicked()
{
    if (!currentToolBar)
        return;

    QListWidgetItem *item = toolBarToItem.value(currentToolBar);
    ui.toolBarList->editItem(item);
}

void QtToolBarDialogPrivate::toolBarRenamed(QListWidgetItem *item)
{
    if (!currentToolBar)
        return;

    ToolBarItem *tbItem = itemToToolBar.value(item);
    if (!tbItem)
        return;
    tbItem->setToolBarName(item->text());
    //ui.toolBarList->sortItems();
}

void QtToolBarDialogPrivate::currentActionChanged(QTreeWidgetItem *current)
{
    if (itemToAction.contains(current))
        currentAction = current;
    else
        currentAction = NULL;
    setButtons();
}

void QtToolBarDialogPrivate::currentToolBarChanged(QListWidgetItem *current)
{
    currentToolBar = itemToToolBar.value(current);
    ui.currentToolBarList->clear();
    actionToCurrentItem.clear();
    currentItemToAction.clear();
    setButtons();
    if (!currentToolBar) {
        return;
    }
    QList<QAction *> actions = currentState.value(currentToolBar);
    QListIterator<QAction *> itAction(actions);
    QListWidgetItem *first = 0;
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        QString actionName = separatorText;
        if (action)
            actionName = action->text();
        QListWidgetItem *item = new QListWidgetItem(actionName, ui.currentToolBarList);
        if (action) {
            item->setIcon(action->icon());
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic);
            actionToCurrentItem.insert(action, item);
            if (widgetActionToToolBar.contains(action))
                item->setData(Qt::TextColorRole, QColor(Qt::blue));
        }
        currentItemToAction.insert(item, action);
        if (!first)
            first = item;
    }
    if (first)
        ui.currentToolBarList->setCurrentItem(first);
}

void QtToolBarDialogPrivate::currentToolBarActionChanged(QListWidgetItem *)
{
    setButtons();
}

void QtToolBarDialogPrivate::cancelClicked()
{
    // just nothing
    q_ptr->reject();
}

//////////////////////
/*
class FeedbackItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    FeedbackItemDelegate(QObject *parent = 0) : QItemDelegate(parent) { }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex & index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

void FeedbackItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    if ()
    painter->save();
    QRect r = option.rect;
    float yCentral = r.height() / 2.0;
    float margin = 2.0;
    float arrowWidth = 5.0;
    float width = 20;
    qDebug("rect: x %d, y %d, w %d, h %d", r.x(), r.y(), r.width(), r.height());
    QLineF lineBase(0.0 + margin, r.y() + yCentral, width - margin, r.y() + yCentral);
    QLineF lineArrowLeft(width - margin - arrowWidth, r.y() + yCentral - arrowWidth,
                    width - margin, r.y() + yCentral);
    QLineF lineArrowRight(width - margin - arrowWidth, r.y() + yCentral + arrowWidth,
                    width - margin, r.y() + yCentral);
    painter->drawLine(lineBase);
    painter->drawLine(lineArrowLeft);
    painter->drawLine(lineArrowRight);
    painter->translate(QPoint(width, 0));
    QItemDelegate::paint(painter, option, index);
    painter->restore();
}

QSize FeedbackItemDelegate::sizeHint(const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    //return QItemDelegate::sizeHint(option, index);
    QSize s = QItemDelegate::sizeHint(option, index);
    s.setWidth(s.width() - 20);
    return s;
}

class QtToolBarListWidget : public QListWidget
{
    Q_OBJECT
public:
    QtToolBarListWidget(QWidget *parent) : QListWidget(parent), actionDrag(false) {}

protected:
    void startDrag(Qt::DropActions supportedActions);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *event);

    void setDragAction(const QString *action) { actionName = action; }
private:
    QPersistentModelIndex lastDropIndicator;
    QString actionName;
    bool actionDrag;
};

void QtToolBarListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (item) {
        actionName = QString();
        emit aboutToDrag(item);
        if (!actionName.isEmpty()) {
            QDrag *drag = new QDrag(this);
            QMimeData *data = new QMimeData;
            data->setData("action", actionName.toLocal8Bit().constData());
            drag->setMimeData(data);
            drag->start(supportedActions);
        }
    }
}

void QtToolBarListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mime = event->mimeData();
    actionDrag = mime->hasFormat("action");
    if (actionDrag)
        event->accept();
    else
        event->ignore();
}

void QtToolBarListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->ignore();
    if (actionDrag) {
        QPoint p = event->pos();
        QListWidgetItem *item = itemAt(p);
        Indicator indic = QtToolBarListWidget::None;
        if (item) {
            QRect rect = visualItemRect(item);
            if (p.y() - rect.top() < rect.height() / 2)
                indic = QtToolBarListWidget::Above;
            else
                indic = QtToolBarListWidget::Below;
        }
        setIndicator(item, indic);
        event->accept();
    }
}

void QtToolBarListWidget::dragLeaveEvent(QDragLeaveEvent *)
{
    if (actionDrag) {
        actionDrag = false;
        setIndicator(item, QtToolBarListWidget::None);
    }
}

void QtToolBarListWidget::dropEvent(QDropEvent *event)
{
    if (actionDrag) {
        QListWidgetItem *item = indicatorItem();
        Indicator indic = indicator();
        QByteArray array = event->mimeData()->data("action");
        QDataStream stream(&array, QIODevice::ReadOnly);
        QString action;
        stream >> action;
        emit actionDropped(action, item, );

        actionDrag = false;
        setIndicator(item, QtToolBarListWidget::None);
    }
}
*/

/*! \class QtToolBarDialog
    \internal
    \inmodule QtDesigner
    \since 4.4

    \brief The QtToolBarDialog class provides a dialog for customizing
    toolbars.

    QtToolBarDialog allows the user to customize the toolbars for a
    given main window.

    \image qttoolbardialog.png

    The dialog lets the users add, rename and remove custom toolbars.
    Note that built-in toolbars are marked with a green color, and
    cannot be removed or renamed.

    The users can also add and remove actions from the toolbars. An
    action can be added to many toolbars, but a toolbar can only
    contain one instance of each action. Actions that contains a
    widget are marked with a blue color in the list of actions, and
    can only be added to one single toolbar.

    Finally, the users can add separators to the toolbars.

    The original toolbars can be restored by clicking the \gui
    {Restore all} button. All custom toolbars will then be removed,
    and all built-in toolbars will be restored to their original state.

    The QtToolBarDialog class's functionality is controlled by an
    instance of the QtToolBarManager class, and the main window is
    specified using the QtToolBarManager::setMainWindow() function.

    All you need to do to use QtToolBarDialog is to specify an
    QtToolBarManager instance and call the QDialog::exec() slot:

    \snippet doc/src/snippets/code/tools_shared_qttoolbardialog_qttoolbardialog.cpp 0

    \sa QtToolBarManager
*/

/*!
    Creates a toolbar dialog with the given \a parent and the specified
    window \a flags.
*/
QtToolBarDialog::QtToolBarDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags), d_ptr(new QtToolBarDialogPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->ui.setupUi(this);
    d_ptr->separatorText = tr("< S E P A R A T O R >");

    d_ptr->ui.actionTree->setColumnCount(1);
    d_ptr->ui.actionTree->setRootIsDecorated(false);
    d_ptr->ui.actionTree->header()->hide();

    d_ptr->ui.upButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/up.png")));
    d_ptr->ui.downButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/down.png")));
    d_ptr->ui.leftButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/back.png")));
    d_ptr->ui.rightButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/forward.png")));
    d_ptr->ui.newButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/plus.png")));
    d_ptr->ui.removeButton->setIcon(QIcon(QLatin1String(":/trolltech/qttoolbardialog/images/minus.png")));

    connect(d_ptr->ui.newButton, SIGNAL(clicked()), this, SLOT(newClicked()));
    connect(d_ptr->ui.removeButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
    connect(d_ptr->ui.renameButton, SIGNAL(clicked()), this, SLOT(renameClicked()));
    connect(d_ptr->ui.upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
    connect(d_ptr->ui.downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
    connect(d_ptr->ui.leftButton, SIGNAL(clicked()), this, SLOT(leftClicked()));
    connect(d_ptr->ui.rightButton, SIGNAL(clicked()), this, SLOT(rightClicked()));

    connect(d_ptr->ui.buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(defaultClicked()));
    connect(d_ptr->ui.buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(d_ptr->ui.buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(applyClicked()));
    connect(d_ptr->ui.buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(cancelClicked()));

    connect(d_ptr->ui.actionTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                    this, SLOT(currentActionChanged(QTreeWidgetItem*)));
    connect(d_ptr->ui.toolBarList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                    this, SLOT(currentToolBarChanged(QListWidgetItem*)));
    connect(d_ptr->ui.currentToolBarList,
                    SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                    this, SLOT(currentToolBarActionChanged(QListWidgetItem*)));

    connect(d_ptr->ui.actionTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
                    this, SLOT(rightClicked()));
    connect(d_ptr->ui.currentToolBarList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                    this, SLOT(leftClicked()));
    connect(d_ptr->ui.toolBarList, SIGNAL(itemChanged(QListWidgetItem*)),
                    this, SLOT(toolBarRenamed(QListWidgetItem*)));
}

/*!
    Destroys the toolbar dialog.
*/
QtToolBarDialog::~QtToolBarDialog()
{
    d_ptr->clearOld();
}

/*!
    Connects the toolbar dialog to the given \a toolBarManager. Then,
    when exec() is called, the toolbar dialog will operate using the
    given \a toolBarManager.
*/
void QtToolBarDialog::setToolBarManager(QtToolBarManager *toolBarManager)
{
    if (d_ptr->toolBarManager == toolBarManager->d_ptr->manager)
        return;
    if (isVisible())
        d_ptr->clearOld();
    d_ptr->toolBarManager = toolBarManager->d_ptr->manager;
    if (isVisible())
        d_ptr->fillNew();
}

/*!
    \reimp
*/
void QtToolBarDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
        d_ptr->fillNew();
}

/*!
    \reimp
*/
void QtToolBarDialog::hideEvent(QHideEvent *event)
{
    if (!event->spontaneous())
        d_ptr->clearOld();
}

QT_END_NAMESPACE

#include "moc_qttoolbardialog.cpp"
#include "qttoolbardialog.moc"
