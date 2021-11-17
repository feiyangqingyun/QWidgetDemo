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


#ifndef ICONSELECTOR_H
#define ICONSELECTOR_H

#include "shared_global_p.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QtResourceModel;
class QDesignerFormEditorInterface;
class QDesignerDialogGuiInterface;
class QDesignerResourceBrowserInterface;

namespace qdesigner_internal {

class DesignerIconCache;
class DesignerPixmapCache;
class PropertySheetIconValue;

// Resource Dialog that embeds the language-dependent resource widget as returned by the language extension
class QDESIGNER_SHARED_EXPORT LanguageResourceDialog : public QDialog
{
    Q_OBJECT

    explicit LanguageResourceDialog(QDesignerResourceBrowserInterface *rb, QWidget *parent = 0);

public:
    virtual ~LanguageResourceDialog();
    // Factory: Returns 0 if the language extension does not provide a resource browser.
    static LanguageResourceDialog* create(QDesignerFormEditorInterface *core, QWidget *parent);

    void setCurrentPath(const QString &filePath);
    QString currentPath() const;

private:
    QScopedPointer<class LanguageResourceDialogPrivate> d_ptr;
    Q_DECLARE_PRIVATE(LanguageResourceDialog)
    Q_DISABLE_COPY(LanguageResourceDialog)
    Q_PRIVATE_SLOT(d_func(), void slotAccepted())
    Q_PRIVATE_SLOT(d_func(), void slotPathChanged(QString))

};

class QDESIGNER_SHARED_EXPORT IconSelector: public QWidget
{
    Q_OBJECT
public:
    IconSelector(QWidget *parent = 0);
    virtual ~IconSelector();

    void setFormEditor(QDesignerFormEditorInterface *core); // required for dialog gui.
    void setIconCache(DesignerIconCache *iconCache);
    void setPixmapCache(DesignerPixmapCache *pixmapCache);

    void setIcon(const PropertySheetIconValue &icon);
    PropertySheetIconValue icon() const;

    // Check whether a pixmap may be read
    enum CheckMode { CheckFast, CheckFully };
    static bool checkPixmap(const QString &fileName, CheckMode cm = CheckFully, QString *errorMessage = 0);
    // Choose a pixmap from file
    static QString choosePixmapFile(const QString &directory, QDesignerDialogGuiInterface *dlgGui, QWidget *parent);
    // Choose a pixmap from resource; use language-dependent resource browser if present
    static QString choosePixmapResource(QDesignerFormEditorInterface *core, QtResourceModel *resourceModel, const QString &oldPath, QWidget *parent);

signals:
    void iconChanged(const PropertySheetIconValue &icon);
private:
    QScopedPointer<class IconSelectorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(IconSelector)
    Q_DISABLE_COPY(IconSelector)

    Q_PRIVATE_SLOT(d_func(), void slotStateActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetResourceActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetFileActivated())
    Q_PRIVATE_SLOT(d_func(), void slotResetActivated())
    Q_PRIVATE_SLOT(d_func(), void slotResetAllActivated())
    Q_PRIVATE_SLOT(d_func(), void slotUpdate())
};


} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // ICONSELECTOR_H

