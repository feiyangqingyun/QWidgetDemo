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

#ifndef QDESIGNER_WIDGETBOX_H
#define QDESIGNER_WIDGETBOX_H

#include "shared_global_p.h"
#include <QtDesigner/QDesignerWidgetBoxInterface>

QT_BEGIN_NAMESPACE

class DomUI;

namespace qdesigner_internal {

// A widget box with a load mode that allows for updating custom widgets.

class QDESIGNER_SHARED_EXPORT QDesignerWidgetBox : public QDesignerWidgetBoxInterface
{
    Q_OBJECT
public:
    enum LoadMode { LoadMerge, LoadReplace, LoadCustomWidgetsOnly };

    explicit QDesignerWidgetBox(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    LoadMode loadMode() const;
    void setLoadMode(LoadMode lm);

    virtual bool loadContents(const QString &contents) = 0;

    // Convenience to access the widget box icon of a widget. Empty category
    // matches all
    virtual QIcon iconForWidget(const QString &className,
                                const QString &category = QString()) const = 0;

    // Convenience to find a widget by class name. Empty category matches all
    static bool findWidget(const QDesignerWidgetBoxInterface *wbox,
                           const QString &className,
                           const QString &category /* = QString()  */,
                           Widget *widgetData);
    // Convenience functions to create a DomWidget from widget box xml.
    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel, QString *errorMessage);
    static DomUI *xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel);

private:
    LoadMode m_loadMode;
};
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_WIDGETBOX_H
