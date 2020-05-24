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

#ifndef QDESIGNER_APPEARANCEOPTIONS_H
#define QDESIGNER_APPEARANCEOPTIONS_H

#include "designer_enums.h"
#include "qdesigner_toolwindow.h"

#include <QtDesigner/private/abstractoptionspage_p.h>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerSettings;

namespace Ui {
    class AppearanceOptionsWidget;
}

/* AppearanceOptions data */
struct AppearanceOptions {
    AppearanceOptions();
    bool equals(const AppearanceOptions&) const;
    void toSettings(QDesignerSettings &) const;
    void fromSettings(const QDesignerSettings &);

    UIMode uiMode;
    ToolWindowFontSettings toolWindowFontSettings;
};

inline bool operator==(const AppearanceOptions &ao1, const AppearanceOptions &ao2)
{
    return ao1.equals(ao2);
}

inline bool operator!=(const AppearanceOptions &ao1, const AppearanceOptions &ao2)
{
    return !ao1.equals(ao2);
}

/* QDesignerAppearanceOptionsWidget: Let the user edit AppearanceOptions */
class QDesignerAppearanceOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDesignerAppearanceOptionsWidget(QWidget *parent = 0);
    ~QDesignerAppearanceOptionsWidget();

    AppearanceOptions appearanceOptions() const;
    void setAppearanceOptions(const AppearanceOptions &ao);

signals:
    void uiModeChanged(bool modified);

private slots:
    void slotUiModeComboChanged();

private:
    UIMode uiMode() const;

    Ui::AppearanceOptionsWidget *m_ui;
    UIMode m_initialUIMode;
};

/* The options page for appearance options. Emits a Timer-0 delayed changed
 * signal to allow the preferences dialog to close (and be deleted) before a
 * possible switch from docked mode to top-level mode happens. (The switch
 * would delete the main window, which the preference dialog is a child of
 * -> BOOM) */

class QDesignerAppearanceOptionsPage : public QObject, public QDesignerOptionsPageInterface
{
    Q_OBJECT

public:
    QDesignerAppearanceOptionsPage(QDesignerFormEditorInterface *core);

    QString name() const;
    QWidget *createPage(QWidget *parent);
    virtual void apply();
    virtual void finish();

signals:
    void settingsChangedDelayed();

private:
    QDesignerFormEditorInterface *m_core;
    QPointer<QDesignerAppearanceOptionsWidget> m_widget;
    AppearanceOptions m_initialOptions;
};

QT_END_NAMESPACE

#endif // QDESIGNER_APPEARANCEOPTIONS_H
