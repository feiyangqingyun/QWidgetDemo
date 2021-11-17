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

#include "qdesigner_widgetitem_p.h"
#include "qdesigner_widget_p.h"
#include "widgetfactory_p.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QApplication>

#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <private/qlayout_p.h>

QT_BEGIN_NAMESPACE

enum { DebugWidgetItem = 0 };
enum { MinimumLength = 10 };

// Widget item creation function to be registered as factory method with
// QLayoutPrivate
static QWidgetItem *createDesignerWidgetItem(const QLayout *layout, QWidget *widget)
{
    Qt::Orientations orientations;
    if (qdesigner_internal::QDesignerWidgetItem::check(layout, widget, &orientations)) {
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItem: Creating on " << layout << widget << orientations;
        return new qdesigner_internal::QDesignerWidgetItem(layout, widget, orientations);
    }
    if (DebugWidgetItem)
        qDebug() << "QDesignerWidgetItem: Noncontainer: " << layout << widget;

    return 0;
}

static QString sizePolicyToString(const QSizePolicy &p)
{
    QString rc; {
        QTextStream str(&rc);
        str << "Control=" << p.controlType() << " expdirs=" << p.expandingDirections()
            << " hasHeightForWidth=" << p.hasHeightForWidth()
            << " H: Policy=" << p.horizontalPolicy()
            << " stretch=" << p.horizontalStretch()
            << " V: Policy=" << p.verticalPolicy()
            << " stretch=" << p.verticalStretch();
    }
    return rc;
}

// Find the layout the item is contained in, recursing over
// child layouts
static const QLayout *findLayoutOfItem(const QLayout *haystack, const QLayoutItem *needle)
{
    const int count = haystack->count();
    for (int i = 0; i < count; i++) {
        QLayoutItem *item = haystack->itemAt(i);
        if (item == needle)
            return haystack;
        if (QLayout *childLayout =  item->layout())
            if (const QLayout *containing = findLayoutOfItem(childLayout, needle))
                return containing;
    }
    return 0;
}


namespace qdesigner_internal {

// ------------------ QDesignerWidgetItem
QDesignerWidgetItem::QDesignerWidgetItem(const QLayout *containingLayout, QWidget *w, Qt::Orientations o) :
    QWidgetItemV2(w),
    m_orientations(o),
    m_nonLaidOutMinSize(w->minimumSizeHint()),
    m_nonLaidOutSizeHint(w->sizeHint()),
    m_cachedContainingLayout(containingLayout)
{
    // Initialize the minimum size to prevent nonlaid-out frames/widgets
    // from being slammed to zero
    const QSize minimumSize = w->minimumSize();
    if (!minimumSize.isEmpty())
        m_nonLaidOutMinSize = minimumSize;
    expand(&m_nonLaidOutMinSize);
    expand(&m_nonLaidOutSizeHint);
    w->installEventFilter(this);
    connect(containingLayout, SIGNAL(destroyed()), this, SLOT(layoutChanged()));
    if (DebugWidgetItem )
        qDebug() << "QDesignerWidgetItem"  << w <<  sizePolicyToString(w->sizePolicy()) << m_nonLaidOutMinSize << m_nonLaidOutSizeHint;
}

void QDesignerWidgetItem::expand(QSize *s) const
{
    // Expand the size if its too small
    if (m_orientations & Qt::Horizontal && s->width() <= 0)
        s->setWidth(MinimumLength);
    if (m_orientations & Qt::Vertical && s->height() <= 0)
        s->setHeight(MinimumLength);
}

QSize QDesignerWidgetItem::minimumSize() const
{
    // Just track the size in case we are laid-out or stretched.
    const QSize baseMinSize = QWidgetItemV2::minimumSize();
    QWidget * w = constWidget();
    if (w->layout() || subjectToStretch(containingLayout(), w)) {
        m_nonLaidOutMinSize = baseMinSize;
        return baseMinSize;
    }
    // Nonlaid out: Maintain last laid-out size
    const QSize rc = baseMinSize.expandedTo(m_nonLaidOutMinSize);
    if (DebugWidgetItem > 1)
         qDebug() << "minimumSize" << constWidget() <<  baseMinSize << rc;
    return rc;
}

QSize QDesignerWidgetItem::sizeHint()    const
{
    // Just track the size in case we are laid-out or stretched.
    const QSize baseSizeHint = QWidgetItemV2::sizeHint();
    QWidget * w = constWidget();
    if (w->layout() || subjectToStretch(containingLayout(), w)) {
        m_nonLaidOutSizeHint = baseSizeHint;
        return baseSizeHint;
    }
    // Nonlaid out: Maintain last laid-out size
    const QSize rc = baseSizeHint.expandedTo(m_nonLaidOutSizeHint);
    if (DebugWidgetItem > 1)
        qDebug() << "sizeHint" << constWidget() << baseSizeHint << rc;
    return rc;
}

bool QDesignerWidgetItem::subjectToStretch(const QLayout *layout, QWidget *w)
{
    if (!layout)
        return false;
    // Are we under some stretch factor?
    if (const QBoxLayout *bl = qobject_cast<const QBoxLayout *>(layout)) {
        const int index = bl->indexOf(w);
        Q_ASSERT(index != -1);
        return bl->stretch(index) != 0;
    }
    if (const QGridLayout *cgl = qobject_cast<const QGridLayout *>(layout)) {
        QGridLayout *gl = const_cast<QGridLayout *>(cgl);
        const int index = cgl->indexOf(w);
        Q_ASSERT(index != -1);
        int row, column, rowSpan, columnSpan;
        gl->getItemPosition (index, &row, &column, &rowSpan, &columnSpan);
        const int rend = row + rowSpan;
        const int cend = column + columnSpan;
        for (int r = row; r < rend; r++)
            if (cgl->rowStretch(r) != 0)
                return true;
        for (int c = column; c < cend; c++)
            if (cgl->columnStretch(c) != 0)
                return true;
    }
    return false;
}

/* Return the orientations mask for a layout, specifying
 * in which directions squeezing should be prevented. */
static Qt::Orientations layoutOrientation(const QLayout *layout)
{
    if (const QBoxLayout *bl = qobject_cast<const QBoxLayout *>(layout)) {
        const QBoxLayout::Direction direction = bl->direction();
        return direction == QBoxLayout::LeftToRight || direction == QBoxLayout::RightToLeft ? Qt::Horizontal : Qt::Vertical;
    }
    if (qobject_cast<const QFormLayout*>(layout))
        return  Qt::Vertical;
    return Qt::Horizontal|Qt::Vertical;
}

// Check for a non-container extension container
bool  QDesignerWidgetItem::isContainer(const QDesignerFormEditorInterface *core, QWidget *w)
{
    if (!WidgetFactory::isFormEditorObject(w))
        return false;
    const QDesignerWidgetDataBaseInterface *wdb = core->widgetDataBase();
    const int widx = wdb->indexOfObject(w);
    if (widx == -1 || !wdb->item(widx)->isContainer())
        return false;
    if (qt_extension<QDesignerContainerExtension*>(core->extensionManager(), w))
        return false;
    return true;
}

bool QDesignerWidgetItem::check(const QLayout *layout, QWidget *w, Qt::Orientations *ptrToOrientations)
{
    // Check for form-editor non-containerextension-containers (QFrame, etc)
    // within laid-out form editor widgets. No check for managed() here as we
    // want container pages and widgets in the process of being morphed as
    // well. Avoid nested layouts (as the effective stretch cannot be easily
    // computed and may mess things up). Won't work for Q3 Group boxes.
    if (ptrToOrientations)
        *ptrToOrientations = 0;

    const QObject *layoutParent = layout->parent();
    if (!layoutParent || !layoutParent->isWidgetType() || !WidgetFactory::isFormEditorObject(layoutParent))
        return false;

    QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(w);
    if (!fw || !isContainer(fw->core(), w))
        return false;

    // If it is a box, restrict to its orientation
    if (ptrToOrientations)
        *ptrToOrientations = layoutOrientation(layout);

    return true;
}

QSize QDesignerWidgetItem::nonLaidOutMinSize() const
{
    return m_nonLaidOutMinSize;
}

void QDesignerWidgetItem::setNonLaidOutMinSize(const QSize &s)
{
    if (DebugWidgetItem > 1)
        qDebug() << "setNonLaidOutMinSize" << constWidget() << s;
    m_nonLaidOutMinSize = s;
}

QSize QDesignerWidgetItem::nonLaidOutSizeHint() const
{
    return m_nonLaidOutSizeHint;
}

void QDesignerWidgetItem::setNonLaidOutSizeHint(const QSize &s)
{
    if (DebugWidgetItem > 1)
        qDebug() << "setNonLaidOutSizeHint" << constWidget() << s;
    m_nonLaidOutSizeHint = s;
}

void QDesignerWidgetItem::install()
{
    QLayoutPrivate::widgetItemFactoryMethod = createDesignerWidgetItem;
}

void QDesignerWidgetItem::deinstall()
{
    QLayoutPrivate::widgetItemFactoryMethod = 0;
}

const QLayout *QDesignerWidgetItem::containingLayout() const
{
    if (!m_cachedContainingLayout) {
        if (QWidget *parentWidget = constWidget()->parentWidget())
            if (QLayout *parentLayout = parentWidget->layout()) {
                m_cachedContainingLayout = findLayoutOfItem(parentLayout, this);
                if (m_cachedContainingLayout)
                    connect(m_cachedContainingLayout, SIGNAL(destroyed()), this, SLOT(layoutChanged()));
            }
        if (DebugWidgetItem)
            qDebug() << Q_FUNC_INFO << " found " << m_cachedContainingLayout << " after reparenting " << constWidget();
    }
    return m_cachedContainingLayout;
}

void QDesignerWidgetItem::layoutChanged()        
{
    if (DebugWidgetItem)
        qDebug() << Q_FUNC_INFO;
    m_cachedContainingLayout = 0;
}

bool QDesignerWidgetItem::eventFilter(QObject * /* watched */, QEvent *event)
{    
    if (event->type() == QEvent::ParentChange)
        layoutChanged();
    return false;
}

// ------------------   QDesignerWidgetItemInstaller

int QDesignerWidgetItemInstaller::m_instanceCount = 0;

QDesignerWidgetItemInstaller::QDesignerWidgetItemInstaller()
{
    if (m_instanceCount++ == 0) {
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItemInstaller: installing";
        QDesignerWidgetItem::install();
    }
}

QDesignerWidgetItemInstaller::~QDesignerWidgetItemInstaller()
{
    if (--m_instanceCount == 0) {
        if (DebugWidgetItem)
            qDebug() << "QDesignerWidgetItemInstaller: deinstalling";
        QDesignerWidgetItem::deinstall();
    }
}

}

QT_END_NAMESPACE
