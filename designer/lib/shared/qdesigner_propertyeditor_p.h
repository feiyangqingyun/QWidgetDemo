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


#ifndef DESIGNERPROPERTYEDITOR_H
#define DESIGNERPROPERTYEDITOR_H

#include "shared_global_p.h"
#include "shared_enums_p.h"
#include <QtDesigner/QDesignerPropertyEditorInterface>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

// Extends the QDesignerPropertyEditorInterface by property comment handling and
// a signal for resetproperty.

class QDESIGNER_SHARED_EXPORT QDesignerPropertyEditor: public QDesignerPropertyEditorInterface
{
    Q_OBJECT
public:
    explicit QDesignerPropertyEditor(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    // A pair <ValidationMode, bool isTranslatable>.
    typedef QPair<TextPropertyValidationMode, bool> StringPropertyParameters;

    // Return a pair of validation mode and flag indicating whether property is translatable
    // for textual properties.
    static StringPropertyParameters textPropertyValidationMode(QDesignerFormEditorInterface *core,
                const QObject *object, const QString &propertyName, bool isMainContainer);

Q_SIGNALS:
    void propertyValueChanged(const QString &name, const QVariant &value, bool enableSubPropertyHandling);
    void resetProperty(const QString &name);
    void addDynamicProperty(const QString &name, const QVariant &value);
    void removeDynamicProperty(const QString &name);
    void editorOpened();
    void editorClosed();

public Q_SLOTS:
    /* Quick update that assumes the actual count of properties has not changed
     * (as opposed to setObject()). N/A when for example executing a
     * layout command and margin properties appear. */
    virtual void updatePropertySheet() = 0;
    virtual void reloadResourceProperties() = 0;

private Q_SLOTS:
    void slotPropertyChanged(const QString &name, const QVariant &value);

protected:
    void emitPropertyValueChanged(const QString &name, const QVariant &value, bool enableSubPropertyHandling);

private:
    bool m_propertyChangedForwardingBlocked;

};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // DESIGNERPROPERTYEDITOR_H
