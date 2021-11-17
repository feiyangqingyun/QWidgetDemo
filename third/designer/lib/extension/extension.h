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

#ifndef EXTENSION_H
#define EXTENSION_H

#include <QtCore/QString>
#include <QtCore/QObject>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_TYPEID(IFace) QLatin1String(IFace##_iid)

class QAbstractExtensionFactory
{
public:
    virtual ~QAbstractExtensionFactory() {}

    virtual QObject *extension(QObject *object, const QString &iid) const = 0;
};
Q_DECLARE_INTERFACE(QAbstractExtensionFactory, "com.trolltech.Qt.QAbstractExtensionFactory")

class QAbstractExtensionManager
{
public:
    virtual ~QAbstractExtensionManager() {}

    virtual void registerExtensions(QAbstractExtensionFactory *factory, const QString &iid) = 0;
    virtual void unregisterExtensions(QAbstractExtensionFactory *factory, const QString &iid) = 0;

    virtual QObject *extension(QObject *object, const QString &iid) const = 0;
};
Q_DECLARE_INTERFACE(QAbstractExtensionManager, "com.trolltech.Qt.QAbstractExtensionManager")

#if defined(Q_CC_MSVC) && (_MSC_VER < 1300)

template <class T>
inline T qt_extension_helper(QAbstractExtensionManager *, QObject *, T)
{ return 0; }

template <class T>
inline T qt_extension(QAbstractExtensionManager* manager, QObject *object)
{ return qt_extension_helper(manager, object, T(0)); }

#define Q_DECLARE_EXTENSION_INTERFACE(IFace, IId) \
const char * const IFace##_iid = IId; \
Q_DECLARE_INTERFACE(IFace, IId) \
template <> inline IFace *qt_extension_helper<IFace *>(QAbstractExtensionManager *manager, QObject *object, IFace *) \
{ QObject *extension = manager->extension(object, Q_TYPEID(IFace)); return (IFace *)(extension ? extension->qt_metacast(IFace##_iid) : 0); }

#else

template <class T>
inline T qt_extension(QAbstractExtensionManager* manager, QObject *object)
{ return 0; }

#define Q_DECLARE_EXTENSION_INTERFACE(IFace, IId) \
const char * const IFace##_iid = IId; \
Q_DECLARE_INTERFACE(IFace, IId) \
template <> inline IFace *qt_extension<IFace *>(QAbstractExtensionManager *manager, QObject *object) \
{ QObject *extension = manager->extension(object, Q_TYPEID(IFace)); return extension ? static_cast<IFace *>(extension->qt_metacast(IFace##_iid)) : static_cast<IFace *>(0); }

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // EXTENSION_H
