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

#include "resourcebuilder_p.h"
#include "ui4_p.h"
#include <QtCore/QVariant>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE

#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

QResourceBuilder::QResourceBuilder()
{

}

QResourceBuilder::~QResourceBuilder()
{

}

int QResourceBuilder::iconStateFlags(const DomResourceIcon *dpi)
{
    int rc = 0;
    if (dpi->hasElementNormalOff())
        rc |= NormalOff;
    if (dpi->hasElementNormalOn())
        rc |= NormalOn;
    if (dpi->hasElementDisabledOff())
        rc |= DisabledOff;
    if (dpi->hasElementDisabledOn())
        rc |= DisabledOn;
    if (dpi->hasElementActiveOff())
        rc |= ActiveOff;
    if (dpi->hasElementActiveOn())
        rc |= ActiveOn;
    if (dpi->hasElementSelectedOff())
        rc |= SelectedOff;
    if (dpi->hasElementSelectedOn())
        rc |= SelectedOn;
    return rc;
}

QVariant QResourceBuilder::loadResource(const QDir &workingDirectory, const DomProperty *property) const
{
    switch (property->kind()) {
        case DomProperty::Pixmap: {
            const DomResourcePixmap *dpx = property->elementPixmap();
            QPixmap pixmap(QFileInfo(workingDirectory, dpx->text()).absoluteFilePath());
            return qVariantFromValue(pixmap);
        }
        case DomProperty::IconSet: {
            const DomResourceIcon *dpi = property->elementIconSet();
            if (const int flags = iconStateFlags(dpi)) { // new, post 4.4 format
                QIcon icon;
                if (flags & NormalOff)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementNormalOff()->text()).absoluteFilePath(), QSize(), QIcon::Normal, QIcon::Off);
                if (flags & NormalOn)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementNormalOn()->text()).absoluteFilePath(), QSize(), QIcon::Normal, QIcon::On);
                if (flags & DisabledOff)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementDisabledOff()->text()).absoluteFilePath(), QSize(), QIcon::Disabled, QIcon::Off);
                if (flags & DisabledOn)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementDisabledOn()->text()).absoluteFilePath(), QSize(), QIcon::Disabled, QIcon::On);
                if (flags & ActiveOff)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementActiveOff()->text()).absoluteFilePath(), QSize(), QIcon::Active, QIcon::Off);
                if (flags & ActiveOn)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementActiveOn()->text()).absoluteFilePath(), QSize(), QIcon::Active, QIcon::On);
                if (flags & SelectedOff)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementSelectedOff()->text()).absoluteFilePath(), QSize(), QIcon::Selected, QIcon::Off);
                if (flags & SelectedOn)
                    icon.addFile(QFileInfo(workingDirectory, dpi->elementSelectedOn()->text()).absoluteFilePath(), QSize(), QIcon::Selected, QIcon::On);
                return qVariantFromValue(icon);
            } else { // 4.3 legacy
                const QIcon icon(QFileInfo(workingDirectory, dpi->text()).absoluteFilePath());
                return qVariantFromValue(icon);
            }
        }
            break;
        default:
            break;
    }
    return QVariant();
}

QVariant QResourceBuilder::toNativeValue(const QVariant &value) const
{
    return value;
}

DomProperty *QResourceBuilder::saveResource(const QDir &workingDirectory, const QVariant &value) const
{
    Q_UNUSED(workingDirectory)
    Q_UNUSED(value)
    return 0;
}

bool QResourceBuilder::isResourceProperty(const DomProperty *p) const
{
    switch (p->kind()) {
        case DomProperty::Pixmap:
        case DomProperty::IconSet:
            return true;
        default:
            break;
    }
    return false;
}

bool QResourceBuilder::isResourceType(const QVariant &value) const
{
    switch (value.type()) {
        case QVariant::Pixmap:
        case QVariant::Icon:
            return true;
        default:
            break;
    }
    return false;
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif

QT_END_NAMESPACE
