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

#include "qdesigner_membersheet_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <abstractintrospection_p.h>

#include <QtGui/QWidget>

namespace {

class Qt3Members
  {
  public:
      static Qt3Members *instance();
      QMap<QString, QStringList> getSignals() const { return m_classNameToSignals; }
      QMap<QString, QStringList> getSlots() const { return m_classNameToSlots; }
  private:
      Qt3Members();
      static Qt3Members *m_instance;
      QMap<QString, QStringList> m_classNameToSignals;
      QMap<QString, QStringList> m_classNameToSlots;
  };

Qt3Members *Qt3Members::m_instance = 0;

Qt3Members::Qt3Members()
{
    m_classNameToSignals[QLatin1String("QTextEdit")].append(QLatin1String("currentFontChanged(QFont)"));
    m_classNameToSignals[QLatin1String("QTextEdit")].append(QLatin1String("currentColorChanged(QColor)"));
    m_classNameToSignals[QLatin1String("QTabWidget")].append(QLatin1String("currentChanged(QWidget*)"));
    m_classNameToSignals[QLatin1String("QTabWidget")].append(QLatin1String("selected(QString)"));
    m_classNameToSignals[QLatin1String("QTabBar")].append(QLatin1String("selected(int)"));
    m_classNameToSignals[QLatin1String("QMenuBar")].append(QLatin1String("activated(int)"));
    m_classNameToSignals[QLatin1String("QMenuBar")].append(QLatin1String("highlighted(int)"));
    m_classNameToSignals[QLatin1String("QMenu")].append(QLatin1String("activated(int)"));
    m_classNameToSignals[QLatin1String("QMenu")].append(QLatin1String("highlighted(int)"));
    m_classNameToSignals[QLatin1String("QLineEdit")].append(QLatin1String("lostFocus()"));
    m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialPressed()"));
    m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialMoved(int)"));
    m_classNameToSignals[QLatin1String("QDial")].append(QLatin1String("dialReleased()"));
    m_classNameToSignals[QLatin1String("QComboBox")].append(QLatin1String("textChanged(QString)"));
    m_classNameToSignals[QLatin1String("QActionGroup")].append(QLatin1String("selected(QAction*)"));
    m_classNameToSignals[QLatin1String("QAction")].append(QLatin1String("activated(int)"));
    m_classNameToSignals[QLatin1String("QAbstractSocket")].append(QLatin1String("connectionClosed()"));
    m_classNameToSignals[QLatin1String("QAbstractSocket")].append(QLatin1String("delayedCloseFinished()"));

    m_classNameToSlots[QLatin1String("QWidget")].append(QLatin1String("setShown(bool)"));
    m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setTextPosition(QToolButton::TextPosition)"));
    m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setUsesBigPixmap(bool)"));
    m_classNameToSlots[QLatin1String("QToolButton")].append(QLatin1String("setUsesTextLabel(bool)"));
    m_classNameToSlots[QLatin1String("QTextEdit")].append(QLatin1String("setModified(bool)"));
    m_classNameToSlots[QLatin1String("QTextEdit")].append(QLatin1String("setColor(QColor)"));
    m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("setCurrentPage(int)"));
    m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("showPage(QWidget*)"));
    m_classNameToSlots[QLatin1String("QTabWidget")].append(QLatin1String("removePage(QWidget*)"));
    m_classNameToSlots[QLatin1String("QTabBar")].append(QLatin1String("setCurrentTab(int)"));
    m_classNameToSlots[QLatin1String("QStatusBar")].append(QLatin1String("message(QString,int)"));
    m_classNameToSlots[QLatin1String("QStatusBar")].append(QLatin1String("clear()"));
    m_classNameToSlots[QLatin1String("QSplashScreen")].append(QLatin1String("message(QString,int)"));
    m_classNameToSlots[QLatin1String("QSplashScreen")].append(QLatin1String("clear()"));
    m_classNameToSlots[QLatin1String("QSlider")].append(QLatin1String("addStep()"));
    m_classNameToSlots[QLatin1String("QSlider")].append(QLatin1String("subtractStep()"));
    m_classNameToSlots[QLatin1String("QAbstractButton")].append(QLatin1String("setOn(bool)"));
    m_classNameToSlots[QLatin1String("QAction")].append(QLatin1String("setOn(bool)"));
    m_classNameToSlots[QLatin1String("QErrorMessage")].append(QLatin1String("message(QString)"));
    m_classNameToSlots[QLatin1String("QTimer")].append(QLatin1String("changeInterval(int)"));
    m_classNameToSlots[QLatin1String("QTimer")].append(QLatin1String("start(int,bool)"));
}

Qt3Members *Qt3Members::instance()
{
    if (!m_instance)
        m_instance = new Qt3Members();
    return m_instance;
}
}

QT_BEGIN_NAMESPACE

static QList<QByteArray> stringListToByteArray(const QStringList &l)
{
    if (l.empty())
        return QList<QByteArray>();
    QList<QByteArray> rc;
    const QStringList::const_iterator cend = l.constEnd();
    for (QStringList::const_iterator it = l.constBegin(); it != cend; ++it)
        rc += it->toUtf8();
    return rc;
}

// Find the form editor in the hierarchy.
// We know that the parent of the sheet is the extension manager
// whose parent is the core.

static QDesignerFormEditorInterface *formEditorForObject(QObject *o) {
    do {
        if (QDesignerFormEditorInterface* core = qobject_cast<QDesignerFormEditorInterface*>(o))
            return core;
        o = o->parent();
    } while(o);
    Q_ASSERT(o);
    return 0;
}

// ------------ QDesignerMemberSheetPrivate
class QDesignerMemberSheetPrivate {
public:
    explicit QDesignerMemberSheetPrivate(QObject *object, QObject *sheetParent);

    QDesignerFormEditorInterface *m_core;
    const QDesignerMetaObjectInterface *m_meta;

    class Info {
    public:
        inline Info() : visible(true) {}

        QString group;
        bool visible;
    };

    typedef QHash<int, Info> InfoHash;

    Info &ensureInfo(int index);

    InfoHash m_info;
};

QDesignerMemberSheetPrivate::QDesignerMemberSheetPrivate(QObject *object, QObject *sheetParent) :
    m_core(formEditorForObject(sheetParent)),
    m_meta(m_core->introspection()->metaObject(object))
{
}

QDesignerMemberSheetPrivate::Info &QDesignerMemberSheetPrivate::ensureInfo(int index)
{
    InfoHash::iterator it = m_info.find(index);
    if (it == m_info.end()) {
        it = m_info.insert(index, Info());
    }
    return it.value();
}

// --------- QDesignerMemberSheet

QDesignerMemberSheet::QDesignerMemberSheet(QObject *object, QObject *parent) :
    QObject(parent),
    d(new QDesignerMemberSheetPrivate(object, parent))
{
}

QDesignerMemberSheet::~QDesignerMemberSheet()
{
    delete d;
}

int QDesignerMemberSheet::count() const
{
    return d->m_meta->methodCount();
}

int QDesignerMemberSheet::indexOf(const QString &name) const
{
    return d->m_meta->indexOfMethod(name);
}

QString QDesignerMemberSheet::memberName(int index) const
{
    return d->m_meta->method(index)->tag();
}

QString QDesignerMemberSheet::declaredInClass(int index) const
{
    const QString member = d->m_meta->method(index)->signature();

    // Find class whose superclass does not contain the method.
    const QDesignerMetaObjectInterface *meta_obj = d->m_meta;

    for (;;) {
        const QDesignerMetaObjectInterface *tmp = meta_obj->superClass();
        if (tmp == 0)
            break;
        if (tmp->indexOfMethod(member) == -1)
            break;
        meta_obj = tmp;
    }
    return meta_obj->className();
}

QString QDesignerMemberSheet::memberGroup(int index) const
{
    return d->m_info.value(index).group;
}

void QDesignerMemberSheet::setMemberGroup(int index, const QString &group)
{
    d->ensureInfo(index).group = group;
}

QString QDesignerMemberSheet::signature(int index) const
{
    return d->m_meta->method(index)->normalizedSignature();
}

bool QDesignerMemberSheet::isVisible(int index) const
{
    typedef QDesignerMemberSheetPrivate::InfoHash InfoHash;
    const InfoHash::const_iterator it = d->m_info.constFind(index);
    if (it != d->m_info.constEnd())
        return it.value().visible;

   return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Signal
           || d->m_meta->method(index)->access() == QDesignerMetaMethodInterface::Public;
}

void QDesignerMemberSheet::setVisible(int index, bool visible)
{
    d->ensureInfo(index).visible = visible;
}

bool QDesignerMemberSheet::isSignal(int index) const
{
    return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Signal;
}

bool QDesignerMemberSheet::isSlot(int index) const
{
    return d->m_meta->method(index)->methodType() == QDesignerMetaMethodInterface::Slot;
}

bool QDesignerMemberSheet::inheritedFromWidget(int index) const
{
    const QString name = d->m_meta->method(index)->signature();
    return declaredInClass(index) == QLatin1String("QWidget") || declaredInClass(index) == QLatin1String("QObject");
}


QList<QByteArray> QDesignerMemberSheet::parameterTypes(int index) const
{
    return stringListToByteArray(d->m_meta->method(index)->parameterTypes());
}

QList<QByteArray> QDesignerMemberSheet::parameterNames(int index) const
{
    return stringListToByteArray(d->m_meta->method(index)->parameterNames());
}

bool QDesignerMemberSheet::signalMatchesSlot(const QString &signal, const QString &slot)
{
    bool result = true;

    do {
        int signal_idx = signal.indexOf(QLatin1Char('('));
        int slot_idx = slot.indexOf(QLatin1Char('('));
        if (signal_idx == -1 || slot_idx == -1)
            break;

        ++signal_idx; ++slot_idx;

        if (slot.at(slot_idx) == QLatin1Char(')'))
            break;

        while (signal_idx < signal.size() && slot_idx < slot.size()) {
            const QChar signal_c = signal.at(signal_idx);
            const QChar slot_c = slot.at(slot_idx);

            if (signal_c == QLatin1Char(',') && slot_c == QLatin1Char(')'))
                break;

            if (signal_c == QLatin1Char(')') && slot_c == QLatin1Char(')'))
                break;

            if (signal_c != slot_c) {
                result = false;
                break;
            }

            ++signal_idx; ++slot_idx;
        }
    } while (false);

    return result;
}

bool QDesignerMemberSheet::isQt3Signal(int index) const
{
    if (!isSignal(index))
        return false;

    const QString className = declaredInClass(index);
    const QString signalSignature = signature(index);

    QMap<QString, QStringList> qt3signals = Qt3Members::instance()->getSignals();
    QMap<QString, QStringList>::const_iterator it = qt3signals.constFind(className);
    if (it != qt3signals.constEnd() && (*it).contains(signalSignature))
        return true;

    return false;
}

bool QDesignerMemberSheet::isQt3Slot(int index) const
{
    if (!isSlot(index))
        return false;

    const QString className = declaredInClass(index);
    const QString slotSignature = signature(index);

    QMap<QString, QStringList> qt3slots = Qt3Members::instance()->getSlots();
    QMap<QString, QStringList>::const_iterator it = qt3slots.constFind(className);
    if (it != qt3slots.constEnd() && (*it).contains(slotSignature))
        return true;
    return false;
}

// ------------ QDesignerMemberSheetFactory

QDesignerMemberSheetFactory::QDesignerMemberSheetFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *QDesignerMemberSheetFactory::createExtension(QObject *object, const QString &iid, QObject *parent) const
{
    if (iid == Q_TYPEID(QDesignerMemberSheetExtension)) {
        return new QDesignerMemberSheet(object, parent);
    }

    return 0;
}

QT_END_NAMESPACE
