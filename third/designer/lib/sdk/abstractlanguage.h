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

#ifndef QDESIGNER_ABTRACT_LANGUAGE_H
#define QDESIGNER_ABTRACT_LANGUAGE_H

#include <QtDesigner/extension.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDialog;
class QWidget;
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class QDesignerResourceBrowserInterface;

class QDesignerLanguageExtension
{
public:
    virtual ~QDesignerLanguageExtension() {}

    virtual QDialog *createFormWindowSettingsDialog(QDesignerFormWindowInterface *formWindow, QWidget *parentWidget) = 0;
    virtual QDesignerResourceBrowserInterface *createResourceBrowser(QWidget *parentWidget) = 0;

    virtual QDialog *createPromotionDialog(QDesignerFormEditorInterface *formEditor, QWidget *parentWidget = 0) = 0;

    virtual QDialog *createPromotionDialog(QDesignerFormEditorInterface *formEditor,
                                           const QString &promotableWidgetClassName,
                                           QString *promoteToClassName,
                                           QWidget *parentWidget = 0) = 0;

    virtual bool isLanguageResource(const QString &path) const = 0;

    virtual QString classNameOf(QObject *object) const = 0;

    virtual bool signalMatchesSlot(const QString &signal, const QString &slot) const = 0;

    virtual QString widgetBoxContents() const = 0;

    virtual QString uiExtension() const = 0;
};

Q_DECLARE_EXTENSION_INTERFACE(QDesignerLanguageExtension, "com.trolltech.Qt.Designer.Language.3")

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDESIGNER_ABTRACT_LANGUAGE_H
