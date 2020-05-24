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

#include "abstractintrospection_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDesignerMetaEnumInterface
    \internal
    \since 4.4

    \brief QDesignerMetaEnumInterface is part of \QD's introspection interface and represents an enumeration.

    \inmodule QtDesigner

    The QDesignerMetaEnumInterface class provides meta-data about an enumerator.

    \sa QDesignerMetaObjectInterface
*/

/*!
    Constructs a QDesignerMetaEnumInterface object.
*/

QDesignerMetaEnumInterface::QDesignerMetaEnumInterface()
{
}

/*!
    Destroys the QDesignerMetaEnumInterface object.
*/
QDesignerMetaEnumInterface::~QDesignerMetaEnumInterface()
{
}

/*!
    \fn bool QDesignerMetaEnumInterface::isFlag() const

    Returns true if this enumerator is used as a flag.
*/

/*!
    \fn QString QDesignerMetaEnumInterface::key(int index) const

    Returns the key with the given \a index.
*/

/*!
    \fn int QDesignerMetaEnumInterface::keyCount() const

    Returns the number of keys.
*/

/*!
    \fn int QDesignerMetaEnumInterface::keyToValue(const QString &key) const

    Returns the integer value of the given enumeration \a key, or -1 if \a key is not defined.
*/

/*!
    \fn int QDesignerMetaEnumInterface::keysToValue(const QString &keys) const

    Returns the value derived from combining together the values of the \a keys using the OR operator, or -1 if keys is not defined. Note that the strings in \a keys must be '|'-separated.
*/

/*!
    \fn QString QDesignerMetaEnumInterface::name() const

    Returns the name of the enumerator (without the scope).
*/

/*!
    \fn QString QDesignerMetaEnumInterface::scope() const

    Returns the scope this enumerator was declared in.
*/

/*!
    \fn QString QDesignerMetaEnumInterface::separator() const

    Returns the separator to be used when building enumeration names.
*/

/*!
    \fn int QDesignerMetaEnumInterface::value(int index) const

    Returns the value with the given \a index; or returns -1 if there is no such value.
*/

/*!
    \fn QString QDesignerMetaEnumInterface::valueToKey(int value) const

    Returns the string that is used as the name of the given enumeration \a value, or QString::null if value is not defined.
*/

/*!
    \fn QString QDesignerMetaEnumInterface::valueToKeys(int value) const

    Returns a byte array of '|'-separated keys that represents the given \a value.
*/

/*!
    \class QDesignerMetaPropertyInterface
    \internal
    \since 4.4

    \brief QDesignerMetaPropertyInterface is part of \QD's introspection interface and represents a property.

    \inmodule QtDesigner

    The QDesignerMetaPropertyInterface class provides meta-data about a property.

    \sa QDesignerMetaObjectInterface
*/

/*!
    Constructs a QDesignerMetaPropertyInterface object.
*/

QDesignerMetaPropertyInterface::QDesignerMetaPropertyInterface()
{
}

/*!
    Destroys the QDesignerMetaPropertyInterface object.
*/

QDesignerMetaPropertyInterface::~QDesignerMetaPropertyInterface()
{
}

/*!
    \enum QDesignerMetaPropertyInterface::Kind

   This enum indicates whether the property is of a special type.

    \value EnumKind      The property is of an enumeration type
    \value FlagKind      The property is of an flag type
    \value OtherKind     The property is of another type
 */

/*!
    \enum QDesignerMetaPropertyInterface::AccessFlag

    These flags specify the access the property provides.

    \value  ReadAccess   Property can be read
    \value  WriteAccess  Property can be written
    \value  ResetAccess  Property can be reset to a default value
 */

/*!
    \enum QDesignerMetaPropertyInterface::Attribute

    Various attributes of the  property.

    \value DesignableAttribute  Property is designable (visible  in \QD)
    \value ScriptableAttribute  Property is scriptable
    \value StoredAttribute      Property is stored, that is, not calculated
    \value UserAttribute        Property is the property that the user can edit for the QObject
 */

/*!
    \fn const QDesignerMetaEnumInterface *QDesignerMetaPropertyInterface::enumerator() const

     Returns the enumerator if this property's type is an enumerator type;
*/

/*!
    \fn Kind QDesignerMetaPropertyInterface::kind() const

     Returns the type of the property.
*/

/*!
    \fn AccessFlags QDesignerMetaPropertyInterface::accessFlags() const

     Returns a combination of access flags.
*/

/*!
    \fn Attributes QDesignerMetaPropertyInterface::attributes(const QObject *object) const

     Returns the attributes of the property for the gives \a object.
*/

/*!
    \fn QVariant::Type QDesignerMetaPropertyInterface::type() const

    Returns the type of the property.
*/

/*!
    \fn QString QDesignerMetaPropertyInterface::name() const

    Returns the name of the property.
*/

/*!
    \fn QString QDesignerMetaPropertyInterface::typeName() const

    Returns the name of this property's type.
*/


/*!
    \fn int QDesignerMetaPropertyInterface::userType() const

    Returns this property's user type.
*/

/*!
    \fn bool QDesignerMetaPropertyInterface::hasSetter() const

    Returns whether getter and setter methods exist for this property.
*/

/*!
    \fn QVariant QDesignerMetaPropertyInterface::read(const QObject *object) const

    Reads the property's value from the given \a object. Returns the value if it was able to read it; otherwise returns an invalid variant.
*/

/*!
    \fn bool QDesignerMetaPropertyInterface::reset(QObject *object) const

    Resets the property for the given \a object with a reset method. Returns true if the reset worked; otherwise returns false.
*/

/*!
    \fn bool QDesignerMetaPropertyInterface::write(QObject *object, const QVariant &value) const

    Writes \a value as the property's value to the given \a object. Returns true if the write succeeded; otherwise returns false.
*/

/*!
    \class QDesignerMetaMethodInterface
    \internal
    \since 4.4

    \brief QDesignerMetaMethodInterface is part of \QD's introspection interface and represents a member function.

    \inmodule QtDesigner

    The QDesignerMetaMethodInterface class provides meta-data about a member function.

    \sa QDesignerMetaObjectInterface
*/

/*!
    Constructs a QDesignerMetaMethodInterface object.
*/

QDesignerMetaMethodInterface::QDesignerMetaMethodInterface()
{
}

/*!
    Destroys the QDesignerMetaMethodInterface object.
*/

QDesignerMetaMethodInterface::~QDesignerMetaMethodInterface()
{
}

/*!
    \enum QDesignerMetaMethodInterface::MethodType

    This enum specifies the type of the method

    \value Method  The function is a plain member function.
    \value Signal  The function is a signal.
    \value Slot    The function is a slot.
    \value Constructor The function is a constructor.

*/

/*!
    \enum QDesignerMetaMethodInterface::Access

    This enum represents the access specification of the method

    \value Private   A private member function
    \value Protected A protected member function
    \value Public    A public member function
*/

/*!
    \fn  QDesignerMetaMethodInterface::Access  QDesignerMetaMethodInterface::access() const

     Returns the access specification of this method.
*/


/*!
    \fn  QDesignerMetaMethodInterface::MethodType  QDesignerMetaMethodInterface::methodType() const

    Returns the type of this method.
*/

/*!
    \fn QStringList QDesignerMetaMethodInterface::parameterNames() const

    Returns a list of parameter names.
*/

/*!
    \fn QStringList QDesignerMetaMethodInterface::parameterTypes() const

    Returns a list of parameter types.
*/

/*!
    \fn QString QDesignerMetaMethodInterface::signature() const

    Returns the signature of this method.
*/

/*!
    \fn QString QDesignerMetaMethodInterface::normalizedSignature() const

     Returns the normalized signature of this method (suitable as signal/slot specification).
*/


/*!
    \fn QString QDesignerMetaMethodInterface::tag() const

    Returns the tag associated with this method.
*/

/*!
    \fn QString QDesignerMetaMethodInterface::typeName() const

    Returns the return type of this method, or an empty string if the return type is void.
*/

/*!
    \class QDesignerMetaObjectInterface
    \internal
    \since 4.4

    \brief QDesignerMetaObjectInterface is part of \QD's introspection interface and provides meta-information about Qt objects

    \inmodule QtDesigner

    The QDesignerMetaObjectInterface class provides meta-data about Qt objects. For a given object, it can be obtained
    by querying QDesignerIntrospectionInterface.

    \sa QDesignerIntrospectionInterface
*/

/*!
    Constructs a QDesignerMetaObjectInterface object.
*/

QDesignerMetaObjectInterface::QDesignerMetaObjectInterface()
{
}

/*!
    Destroys the QDesignerMetaObjectInterface object.
*/

QDesignerMetaObjectInterface::~QDesignerMetaObjectInterface()
{
}

/*!
    \fn QString QDesignerMetaObjectInterface::className() const

    Returns the class name.
*/

/*!
    \fn const QDesignerMetaEnumInterface *QDesignerMetaObjectInterface::enumerator(int index) const

    Returns the meta-data for the enumerator with the given \a index.
*/

/*!
    \fn int QDesignerMetaObjectInterface::enumeratorCount() const

    Returns the number of enumerators in this class.
*/

/*!
    \fn int QDesignerMetaObjectInterface::enumeratorOffset() const

    Returns the enumerator offset for this class; i.e. the index position of this class's first enumerator.
*/

/*!
    \fn int QDesignerMetaObjectInterface::indexOfEnumerator(const QString &name) const

    Finds enumerator \a name and returns its index; otherwise returns -1.
*/

/*!
    \fn int QDesignerMetaObjectInterface::indexOfMethod(const QString &method) const

    Finds \a method and returns its index; otherwise returns -1.
*/

/*!
    \fn int QDesignerMetaObjectInterface::indexOfProperty(const QString &name) const

    Finds property \a name and returns its index; otherwise returns -1.
*/

/*!
    \fn int QDesignerMetaObjectInterface::indexOfSignal(const QString &signal) const

    Finds \a signal and returns its index; otherwise returns -1.
*/

/*!
    \fn int QDesignerMetaObjectInterface::indexOfSlot(const QString &slot) const

    Finds \a slot and returns its index; otherwise returns -1.
*/

/*!
    \fn const QDesignerMetaMethodInterface *QDesignerMetaObjectInterface::method(int index) const

    Returns the meta-data for the method with the given \a index.
*/

/*!
    \fn int QDesignerMetaObjectInterface::methodCount() const

    Returns the number of methods in this class. These include ordinary methods, signals, and slots.
*/

/*!
    \fn int QDesignerMetaObjectInterface::methodOffset() const

    Returns the method offset for this class; i.e. the index position of this class's first member function.
*/

/*!
    \fn const  QDesignerMetaPropertyInterface *QDesignerMetaObjectInterface::property(int index) const

    Returns the meta-data for the property with the given \a index.
*/
/*!
    \fn int QDesignerMetaObjectInterface::propertyCount() const

    Returns the number of properties in this class.
*/
/*!
    \fn int QDesignerMetaObjectInterface::propertyOffset() const

    Returns the property offset for this class; i.e. the index position of this class's first property.
*/

/*!
    \fn const QDesignerMetaObjectInterface *QDesignerMetaObjectInterface::superClass() const

    Returns the meta-object of the superclass, or 0 if there is no such object.
*/

/*!
    \fn const QDesignerMetaPropertyInterface *QDesignerMetaObjectInterface::userProperty() const

    Returns the property that has the USER flag set to true.
*/

/*!
    \class QDesignerIntrospectionInterface
    \internal
    \since 4.4

    \brief QDesignerIntrospectionInterface provides access to a QDesignerMetaObjectInterface for a given Qt object.

    \inmodule QtDesigner

    QDesignerIntrospectionInterface is the main class of \QD's introspection interface. These
    interfaces provide a layer of abstraction around QMetaObject and related classes to allow for the integration
    of other programming languages.

    An instance of QDesignerIntrospectionInterface can be obtained from the core.

    \sa QDesignerMetaObjectInterface
*/

/*!
    Constructs a QDesignerIntrospectionInterface object.
*/

QDesignerIntrospectionInterface::QDesignerIntrospectionInterface()
{
}

/*!
    Destroys the QDesignerIntrospectionInterface object.
*/

QDesignerIntrospectionInterface::~QDesignerIntrospectionInterface()
{
}

/*!
   \fn const QDesignerMetaObjectInterface* QDesignerIntrospectionInterface::metaObject(const QObject *object) const

   Returns the meta object of this \a object.
*/

QT_END_NAMESPACE
