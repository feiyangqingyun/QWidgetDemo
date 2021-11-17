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

#include "layout_p.h"
#include "qdesigner_utils_p.h"
#include "qlayout_widget_p.h"
#include "spacer_widget_p.h"
#include "layoutdecoration.h"
#include "widgetfactory_p.h"
#include "qdesigner_widgetitem_p.h"

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include <QtCore/qdebug.h>
#include <QtCore/QVector>

#include <QtGui/qevent.h>
#include <QtGui/QGridLayout>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QSplitter>
#include <QtGui/QMainWindow>
#include <QtGui/QApplication>
#include <QtGui/QScrollArea>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QWizardPage>
#include <QtGui/QWizard>
#include <QtCore/QDebug>
#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

enum { FormLayoutColumns = 2 };

namespace qdesigner_internal {

/* The wizard has a policy of setting a size policy of its external children
 * according to the page being expanding or not (in the latter case, the
 * page will be pushed to the top). When setting/breaking layouts, this needs
 * to be updated, which happens via a fake style change event. */

void updateWizardLayout(QWidget *layoutBase);

class FriendlyWizardPage : public  QWizardPage {
    friend void updateWizardLayout(QWidget *);
};

void updateWizardLayout(QWidget *layoutBase)
{
    if (QWizardPage *wizardPage = qobject_cast<QWizardPage*>(layoutBase))
        if (QWizard *wizard = static_cast<FriendlyWizardPage*>(wizardPage)->wizard()) {
            QEvent event(QEvent::StyleChange);
            QApplication::sendEvent(wizard, &event);
        }
}

/*!
  \class Layout layout.h
  \brief Baseclass for layouting widgets in the Designer (Helper for Layout commands)
  \internal

  Classes derived from this abstract base class are used for layouting
  operations in the Designer (creating/breaking layouts).

  Instances live in the Layout/BreakLayout commands.
*/

/*!  \a p specifies the parent of the layoutBase \a lb. The parent
  might be changed in setup(). If the layoutBase is a
  container, the parent and the layoutBase are the same. Also they
  always have to be a widget known to the designer (e.g. in the case
  of the tabwidget parent and layoutBase are the tabwidget and not the
  page which actually gets laid out. For actual usage the correct
  widget is found later by Layout.)
 */

Layout::Layout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb, LayoutInfo::Type layoutType) :
    m_widgets(wl),
    m_parentWidget(p),
    m_layoutBase(lb),
    m_formWindow(fw),
    m_layoutType(layoutType),
    m_reparentLayoutWidget(true),
    m_isBreak(false)
{
    if (m_layoutBase)
        m_oldGeometry = m_layoutBase->geometry();
}

Layout::~Layout()
{
}

/*!  The widget list we got in the constructor might contain too much
  widgets (like widgets with different parents, already laid out
  widgets, etc.). Here we set up the list and so the only the "best"
  widgets get laid out.
*/

void Layout::setup()
{
    m_startPoint = QPoint(32767, 32767);

    // Go through all widgets of the list we got. As we can only
    // layout widgets which have the same parent, we first do some
    // sorting which means create a list for each parent containing
    // its child here. After that we keep working on the list of
    // children which has the most entries.
    // Widgets which are already laid out are thrown away here too

    QMultiMap<QWidget*, QWidget*> lists;
    foreach (QWidget *w, m_widgets) {
        QWidget *p = w->parentWidget();

        if (p && LayoutInfo::layoutType(m_formWindow->core(), p) != LayoutInfo::NoLayout
                && m_formWindow->core()->metaDataBase()->item(p->layout()) != 0)
            continue;

        lists.insert(p, w);
    }

    QWidgetList lastList;
    QWidgetList parents = lists.keys();
    foreach (QWidget *p, parents) {
        QWidgetList children = lists.values(p);

        if (children.count() > lastList.count())
            lastList = children;
    }


    // If we found no list (because no widget did fit at all) or the
    // best list has only one entry and we do not layout a container,
    // we leave here.
    QDesignerWidgetDataBaseInterface *widgetDataBase = m_formWindow->core()->widgetDataBase();
    if (lastList.count() < 2 &&
                        (!m_layoutBase ||
                          (!widgetDataBase->isContainer(m_layoutBase, false) &&
                            m_layoutBase != m_formWindow->mainContainer()))
                       ) {
        m_widgets.clear();
        m_startPoint = QPoint(0, 0);
        return;
    }

    // Now we have a new and clean widget list, which makes sense
    // to layout
    m_widgets = lastList;
    // Also use the only correct parent later, so store it

    Q_ASSERT(m_widgets.isEmpty() == false);

    m_parentWidget = m_formWindow->core()->widgetFactory()->widgetOfContainer(m_widgets.first()->parentWidget());
    // Now calculate the position where the layout-meta-widget should
    // be placed and connect to widgetDestroyed() signals of the
    // widgets to get informed if one gets deleted to be able to
    // handle that and do not crash in this case
    foreach (QWidget *w, m_widgets) {
        connect(w, SIGNAL(destroyed()), this, SLOT(widgetDestroyed()));
        m_startPoint = QPoint(qMin(m_startPoint.x(), w->x()), qMin(m_startPoint.y(), w->y()));
        const QRect rc(w->geometry());

        m_geometries.insert(w, rc);
        // Change the Z-order, as saving/loading uses the Z-order for
        // writing/creating widgets and this has to be the same as in
        // the layout. Else saving + loading will give different results
        w->raise();
    }

    sort();
}

void Layout::widgetDestroyed()
{
    if (QWidget *w = qobject_cast<QWidget *>(sender())) {
        m_widgets.removeAt(m_widgets.indexOf(w));
        m_geometries.remove(w);
    }
}

bool Layout::prepareLayout(bool &needMove, bool &needReparent)
{
    foreach (QWidget *widget, m_widgets) {
        widget->raise();
    }

    needMove = !m_layoutBase;
    needReparent = needMove || (m_reparentLayoutWidget && qobject_cast<QLayoutWidget*>(m_layoutBase)) || qobject_cast<QSplitter*>(m_layoutBase);

    QDesignerWidgetFactoryInterface *widgetFactory = m_formWindow->core()->widgetFactory();
    QDesignerMetaDataBaseInterface *metaDataBase = m_formWindow->core()->metaDataBase();

    if (m_layoutBase == 0) {
        const bool useSplitter = m_layoutType == LayoutInfo::HSplitter || m_layoutType == LayoutInfo::VSplitter;
        const QString baseWidgetClassName = useSplitter ? QLatin1String("QSplitter") : QLatin1String("QLayoutWidget");
        m_layoutBase = widgetFactory->createWidget(baseWidgetClassName, widgetFactory->containerOfWidget(m_parentWidget));
        if (useSplitter) {
            m_layoutBase->setObjectName(QLatin1String("splitter"));
            m_formWindow->ensureUniqueObjectName(m_layoutBase);
        }
    } else {
        LayoutInfo::deleteLayout(m_formWindow->core(), m_layoutBase);
    }

    metaDataBase->add(m_layoutBase);

    Q_ASSERT(m_layoutBase->layout() == 0 || metaDataBase->item(m_layoutBase->layout()) == 0);

    return true;
}

static bool isMainContainer(QDesignerFormWindowInterface *fw, const QWidget *w)
{
    return w && (w == fw || w == fw->mainContainer());
}

static bool isPageOfContainerWidget(QDesignerFormWindowInterface *fw, QWidget *widget)
{
    QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(
            fw->core()->extensionManager(), widget->parentWidget());

    if (c != 0) {
        for (int i = 0; i<c->count(); ++i) {
            if (widget == c->widget(i))
                return true;
        }
    }

    return false;
}
void Layout::finishLayout(bool needMove, QLayout *layout)
{
    if (m_parentWidget == m_layoutBase) {
        QWidget *widget = m_layoutBase;
        m_oldGeometry = widget->geometry();

        bool done = false;
        while (!isMainContainer(m_formWindow, widget) && !done) {
            if (!m_formWindow->isManaged(widget)) {
                widget = widget->parentWidget();
                continue;
            } else if (LayoutInfo::isWidgetLaidout(m_formWindow->core(), widget)) {
                widget = widget->parentWidget();
                continue;
            } else if (isPageOfContainerWidget(m_formWindow, widget)) {
                widget = widget->parentWidget();
                continue;
            } else if (widget->parentWidget()) {
                QScrollArea *area = qobject_cast<QScrollArea*>(widget->parentWidget()->parentWidget());
                if (area && area->widget() == widget) {
                    widget = area;
                    continue;
                }
            }

            done = true;
        }
        updateWizardLayout(m_layoutBase);
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        // We don't want to resize the form window
        if (!Utils::isCentralWidget(m_formWindow, widget))
            widget->adjustSize();

        return;
    }

    if (needMove)
        m_layoutBase->move(m_startPoint);

    const QRect g(m_layoutBase->pos(), m_layoutBase->size());

    if (LayoutInfo::layoutType(m_formWindow->core(), m_layoutBase->parentWidget()) == LayoutInfo::NoLayout && !m_isBreak)
        m_layoutBase->adjustSize();
    else if (m_isBreak)
        m_layoutBase->setGeometry(m_oldGeometry);

    m_oldGeometry = g;
    if (layout)
        layout->invalidate();
    m_layoutBase->show();

    if (qobject_cast<QLayoutWidget*>(m_layoutBase) || qobject_cast<QSplitter*>(m_layoutBase)) {
        m_formWindow->clearSelection(false);
        m_formWindow->manageWidget(m_layoutBase);
        m_formWindow->selectWidget(m_layoutBase);
    }
}

void Layout::undoLayout()
{
    if (!m_widgets.count())
        return;

    m_formWindow->selectWidget(m_layoutBase, false);

    QDesignerWidgetFactoryInterface *widgetFactory = m_formWindow->core()->widgetFactory();
    QHashIterator<QWidget *, QRect> it(m_geometries);
    while (it.hasNext()) {
        it.next();

        if (!it.key())
            continue;

        QWidget* w = it.key();
        const QRect rc = it.value();

        const bool showIt = w->isVisibleTo(m_formWindow);
        QWidget *container = widgetFactory->containerOfWidget(m_parentWidget);

        // ### remove widget here
        QWidget *parentWidget = w->parentWidget();
        QDesignerFormEditorInterface *core = m_formWindow->core();
        QDesignerLayoutDecorationExtension *deco = qt_extension<QDesignerLayoutDecorationExtension*>(core->extensionManager(), parentWidget);

        if (deco)
            deco->removeWidget(w);

        w->setParent(container);
        w->setGeometry(rc);

        if (showIt)
            w->show();
    }

    LayoutInfo::deleteLayout(m_formWindow->core(), m_layoutBase);

    if (m_parentWidget != m_layoutBase && !qobject_cast<QMainWindow*>(m_layoutBase)) {
        m_formWindow->unmanageWidget(m_layoutBase);
        m_layoutBase->hide();
    } else {
        QMainWindow *mw = qobject_cast<QMainWindow*>(m_formWindow->mainContainer());
        if (m_layoutBase != m_formWindow->mainContainer() &&
                    (!mw || mw->centralWidget() != m_layoutBase))
            m_layoutBase->setGeometry(m_oldGeometry);
    }
}

void Layout::breakLayout()
{
    typedef QMap<QWidget *, QRect> WidgetRectMap;
    WidgetRectMap rects;
    /* Store the geometry of the widgets. The idea is to give the user space
     * to rearrange them, so, we do a adjustSize() on them, unless they want
     * to grow (expanding widgets like QTextEdit), in which the geometry is
     * preserved. Note that historically, geometries were re-applied
     * only after breaking splitters. */
    foreach (QWidget *w, m_widgets) {
        const QRect geom = w->geometry();
        const QSize sizeHint = w->sizeHint();
        const bool restoreGeometry = sizeHint.isEmpty() || sizeHint.width() > geom.width() || sizeHint.height() > geom.height();
        rects.insert(w, restoreGeometry ? w->geometry() : QRect(geom.topLeft(), QSize()));
    }
    const QPoint m_layoutBasePos = m_layoutBase->pos();
    QDesignerWidgetDataBaseInterface *widgetDataBase = m_formWindow->core()->widgetDataBase();

    LayoutInfo::deleteLayout(m_formWindow->core(), m_layoutBase);

    const bool needReparent = (m_reparentLayoutWidget && qobject_cast<QLayoutWidget*>(m_layoutBase)) ||
                        qobject_cast<QSplitter*>(m_layoutBase)     ||
                        (!widgetDataBase->isContainer(m_layoutBase, false) &&
                          m_layoutBase != m_formWindow->mainContainer());
    const bool add = m_geometries.isEmpty();

    QMapIterator<QWidget*, QRect> it(rects);
    while (it.hasNext()) {
        it.next();

        QWidget *w = it.key();
        if (needReparent) {
            w->setParent(m_layoutBase->parentWidget(), 0);
            w->move(m_layoutBasePos + it.value().topLeft());
            w->show();
        }

        const QRect oldGeometry = it.value();
        if (oldGeometry.isEmpty()) {
            w->adjustSize();
        } else {
            w->resize(oldGeometry.size());
        }

        if (add)
            m_geometries.insert(w, QRect(w->pos(), w->size()));
    }

    if (needReparent) {
        m_layoutBase->hide();
        m_parentWidget = m_layoutBase->parentWidget();
        m_formWindow->unmanageWidget(m_layoutBase);
    } else {
        m_parentWidget = m_layoutBase;
    }
    updateWizardLayout(m_layoutBase);

    if (!m_widgets.isEmpty() && m_widgets.first() && m_widgets.first()->isVisibleTo(m_formWindow))
        m_formWindow->selectWidget(m_widgets.first());
    else
        m_formWindow->selectWidget(m_formWindow);
}

static QString suggestLayoutName(const char *className)
{
    // Legacy
    if (!qstrcmp(className, "QHBoxLayout"))
        return QLatin1String("horizontalLayout");
    if (!qstrcmp(className, "QVBoxLayout"))
        return QLatin1String("verticalLayout");
    if (!qstrcmp(className, "QGridLayout"))
        return QLatin1String("gridLayout");

    return qtify(QString::fromUtf8(className));
}
QLayout *Layout::createLayout(int type)
{
    Q_ASSERT(m_layoutType != LayoutInfo::HSplitter && m_layoutType != LayoutInfo::VSplitter);
    QLayout *layout = m_formWindow->core()->widgetFactory()->createLayout(m_layoutBase, 0, type);
    // set a name
    layout->setObjectName(suggestLayoutName(layout->metaObject()->className()));
    m_formWindow->ensureUniqueObjectName(layout);
    // QLayoutWidget
    QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(m_formWindow->core()->extensionManager(), layout);
    if (sheet && qobject_cast<QLayoutWidget*>(m_layoutBase)) {
        sheet->setProperty(sheet->indexOf(QLatin1String("leftMargin")), 0);
        sheet->setProperty(sheet->indexOf(QLatin1String("topMargin")), 0);
        sheet->setProperty(sheet->indexOf(QLatin1String("rightMargin")), 0);
        sheet->setProperty(sheet->indexOf(QLatin1String("bottomMargin")), 0);
    }
    return layout;
}

void Layout::reparentToLayoutBase(QWidget *w)
{
    if (w->parent() != m_layoutBase) {
        w->setParent(m_layoutBase, 0);
        w->move(QPoint(0,0));
    }
}

namespace { // within qdesigner_internal

// ----- PositionSortPredicate: Predicate to be usable as LessThan function to sort widgets by position
class PositionSortPredicate {
public:
    PositionSortPredicate(Qt::Orientation orientation) : m_orientation(orientation) {}
    bool operator()(const QWidget* w1, const QWidget* w2) {
        return m_orientation == Qt::Horizontal ? w1->x() < w2->x() : w1->y() < w2->y();
    }
    private:
    const Qt::Orientation m_orientation;
};

// -------- BoxLayout
class BoxLayout : public Layout
{
public:
    BoxLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb,
              Qt::Orientation orientation);

    virtual void doLayout();
    virtual void sort();

private:
    const Qt::Orientation m_orientation;
};

BoxLayout::BoxLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb,
                     Qt::Orientation orientation)  :
    Layout(wl, p, fw, lb, orientation == Qt::Horizontal ? LayoutInfo::HBox : LayoutInfo::VBox),
    m_orientation(orientation)
{
}

void BoxLayout::sort()
{
    QWidgetList wl = widgets();
    qStableSort(wl.begin(), wl.end(), PositionSortPredicate(m_orientation));
    setWidgets(wl);
}

void BoxLayout::doLayout()
{
    bool needMove, needReparent;
    if (!prepareLayout(needMove, needReparent))
        return;

    QBoxLayout *layout = static_cast<QBoxLayout *>(createLayout(m_orientation == Qt::Horizontal ? LayoutInfo::HBox : LayoutInfo::VBox));

    QDesignerWidgetItemInstaller wii; // Make sure we use QDesignerWidgetItem.

    const  QWidgetList::const_iterator cend = widgets().constEnd();
    for (QWidgetList::const_iterator it =  widgets().constBegin(); it != cend; ++it) {
        QWidget *w = *it;
        if (needReparent)
            reparentToLayoutBase(w);

        if (const Spacer *spacer = qobject_cast<const Spacer*>(w))
            layout->addWidget(w, 0, spacer->alignment());
        else
            layout->addWidget(w);
        w->show();
    }
    finishLayout(needMove, layout);
}

// --------  SplitterLayout
class SplitterLayout : public Layout
{
public:
    SplitterLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb,
                   Qt::Orientation orientation);

    virtual void doLayout();
    virtual void sort();

private:
    const Qt::Orientation m_orientation;
};

SplitterLayout::SplitterLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb,
                               Qt::Orientation orientation) :
    Layout(wl, p, fw, lb, orientation == Qt::Horizontal ? LayoutInfo::HSplitter : LayoutInfo::VSplitter),
    m_orientation(orientation)
{
}

void SplitterLayout::sort()
{
    QWidgetList wl = widgets();
    qStableSort(wl.begin(), wl.end(), PositionSortPredicate(m_orientation));
    setWidgets(wl);
}

void SplitterLayout::doLayout()
{
    bool needMove, needReparent;
    if (!prepareLayout(needMove, needReparent))
        return;

    QSplitter *splitter = qobject_cast<QSplitter*>(layoutBaseWidget());
    Q_ASSERT(splitter != 0);


    const  QWidgetList::const_iterator cend = widgets().constEnd();
    for (QWidgetList::const_iterator it =  widgets().constBegin(); it != cend; ++it) {
        QWidget *w = *it;
        if (needReparent)
            reparentToLayoutBase(w);
        splitter->addWidget(w);
        w->show();
    }

    splitter->setOrientation(m_orientation);
    finishLayout(needMove);
}

//  ---------- Grid: Helper for laying out grids

class Grid
{
public:
    enum Mode {
        GridLayout, // Arbitrary size/supports span
        FormLayout  // 2-column/no span
    };

    Grid(Mode mode);
    void resize(int nrows, int ncols);

    ~Grid();

    QWidget* cell(int row, int col) const { return m_cells[ row * m_ncols + col]; }

    void setCells(const QRect &c, QWidget* w);

    bool empty() const  { return m_nrows * m_ncols; }
    int numRows() const { return m_nrows; }
    int numCols() const { return m_ncols; }

    void simplify();
    bool locateWidget(QWidget* w, int& row, int& col, int& rowspan, int& colspan) const;

    QDebug debug(QDebug str) const;

private:
    void setCell(int row, int col, QWidget* w) { m_cells[ row * m_ncols + col] = w; }
    void swapCells(int r1, int c1, int r2, int c2);
    void shrink();
    void reallocFormLayout();
    int countRow(int r, int c) const;
    int countCol(int r, int c) const;
    void setRow(int r, int c, QWidget* w, int count);
    void setCol(int r, int c, QWidget* w, int count);
    bool isWidgetStartCol(int c) const;
    bool isWidgetEndCol(int c) const;
    bool isWidgetStartRow(int r) const;
    bool isWidgetEndRow(int r) const;
    bool isWidgetTopLeft(int r, int c) const;
    void extendLeft();
    void extendRight();
    void extendUp();
    void extendDown();
    bool shrinkFormLayoutSpans();

    const Mode m_mode;
    int m_nrows;
    int m_ncols;

    QWidget** m_cells; // widget matrix w11, w12, w21...
};

Grid::Grid(Mode mode) :
    m_mode(mode),
    m_nrows(0),
    m_ncols(0),
    m_cells(0)
{
}

Grid::~Grid()
{
    delete [] m_cells;
}

void Grid::resize(int nrows, int ncols)
{
    delete [] m_cells;
    m_cells = 0;
    m_nrows = nrows;
    m_ncols = ncols;
    if (const int allocSize = m_nrows * m_ncols) {
        m_cells = new QWidget*[allocSize];
        qFill(m_cells, m_cells + allocSize, static_cast<QWidget *>(0));
    }
}

QDebug Grid::debug(QDebug str) const
{
    str << m_nrows << 'x' << m_ncols << '\n';
    QSet<QWidget *> widgets;
    const int cellCount = m_nrows * m_ncols;
    int row, col, rowspan, colspan;
    for (int c = 0; c < cellCount; c++)
        if (QWidget *w = m_cells[c])
            if (!widgets.contains(w)) {
                widgets.insert(w);
                locateWidget(w, row, col, rowspan, colspan);
                str << w << " at " << row <<  col << rowspan << 'x' << colspan << '\n';
            }
    for (int r = 0; r < m_nrows; r++)
        for (int c = 0; c < m_ncols; c++)
            str << "At " << r << c << cell(r, c) << '\n';

    return str;
}

static inline QDebug operator<<(QDebug str, const Grid &g) { return g.debug(str); }

void Grid::setCells(const QRect &c, QWidget* w)
{
    const int bottom = c.top() + c.height();
    const int width =  c.width();

    for (int r = c.top(); r < bottom; r++) {
        QWidget **pos = m_cells + r * m_ncols + c.left();
        qFill(pos, pos + width, w);
    }
}


void Grid::swapCells(int r1, int c1, int r2, int c2)
{
    QWidget *w1 = cell(r1, c1);
    setCell(r1, c1, cell(r2, c2));
    setCell(r2, c2, w1);
}

int Grid::countRow(int r, int c) const
{
    QWidget* w = cell(r, c);
    int i = c + 1;
    while (i < m_ncols && cell(r, i) == w)
        i++;
    return i - c;
}

int Grid::countCol(int r, int c) const
{
    QWidget* w = cell(r, c);
    int i = r + 1;
    while (i < m_nrows && cell(i, c) == w)
        i++;
    return i - r;
}

void Grid::setCol(int r, int c, QWidget* w, int count)
{
    for (int i = 0; i < count; i++)
        setCell(r + i, c, w);
}

void Grid::setRow(int r, int c, QWidget* w, int count)
{
    for (int i = 0; i < count; i++)
        setCell(r, c + i, w);
}

bool Grid::isWidgetStartCol(int c) const
{
    for (int r = 0; r < m_nrows; r++) {
        if (cell(r, c) && ((c==0) || (cell(r, c)  != cell(r, c-1)))) {
            return true;
        }
    }
    return false;
}

bool Grid::isWidgetEndCol(int c) const
{
    for (int r = 0; r < m_nrows; r++) {
        if (cell(r, c) && ((c == m_ncols-1) || (cell(r, c) != cell(r, c+1))))
            return true;
    }
    return false;
}

bool Grid::isWidgetStartRow(int r) const
{
    for ( int c = 0; c < m_ncols; c++) {
        if (cell(r, c) && ((r==0) || (cell(r, c) != cell(r-1, c))))
            return true;
    }
    return false;
}

bool Grid::isWidgetEndRow(int r) const
{
    for (int c = 0; c < m_ncols; c++) {
        if (cell(r, c) && ((r == m_nrows-1) || (cell(r, c) != cell(r+1, c))))
            return true;
    }
    return false;
}


bool Grid::isWidgetTopLeft(int r, int c) const
{
    QWidget* w = cell(r, c);
    if (!w)
        return false;
    return (!r || cell(r-1, c) != w) && (!c || cell(r, c-1) != w);
}

void Grid::extendLeft()
{
    for (int c = 1; c < m_ncols; c++) {
        for (int r = 0; r < m_nrows; r++) {
            QWidget* w = cell(r, c);
            if (!w)
                continue;

            const int cc = countCol(r, c);
            int stretch = 0;
            for (int i = c-1; i >= 0; i--) {
                if (cell(r, i))
                    break;
                if (countCol(r, i) < cc)
                    break;
                if (isWidgetEndCol(i))
                    break;
                if (isWidgetStartCol(i)) {
                    stretch = c - i;
                    break;
                }
            }
            if (stretch) {
                for (int i = 0; i < stretch; i++)
                    setCol(r, c-i-1, w, cc);
            }
        }
    }
}


void Grid::extendRight()
{
    for (int c = m_ncols - 2; c >= 0; c--) {
        for (int r = 0; r < m_nrows; r++) {
            QWidget* w = cell(r, c);
            if (!w)
                continue;
            const int cc = countCol(r, c);
            int stretch = 0;
            for (int i = c+1; i < m_ncols; i++) {
                if (cell(r, i))
                    break;
                if (countCol(r, i) < cc)
                    break;
                if (isWidgetStartCol(i))
                    break;
                if (isWidgetEndCol(i)) {
                    stretch = i - c;
                    break;
                }
            }
            if (stretch) {
                for (int i = 0; i < stretch; i++)
                    setCol(r, c+i+1, w, cc);
            }
        }
    }

}

void Grid::extendUp()
{
    for (int r = 1; r < m_nrows; r++) {
        for (int c = 0; c < m_ncols; c++) {
            QWidget* w = cell(r, c);
            if (!w)
                continue;
            const int cr = countRow(r, c);
            int stretch = 0;
            for (int i = r-1; i >= 0; i--) {
                if (cell(i, c))
                    break;
                if (countRow(i, c) < cr)
                    break;
                if (isWidgetEndRow(i))
                    break;
                if (isWidgetStartRow(i)) {
                    stretch = r - i;
                    break;
                }
            }
            if (stretch) {
                for (int i = 0; i < stretch; i++)
                    setRow(r-i-1, c, w, cr);
            }
        }
    }
}

void Grid::extendDown()
{
    for (int r = m_nrows - 2; r >= 0; r--) {
        for (int c = 0; c < m_ncols; c++) {
            QWidget* w = cell(r, c);
            if (!w)
                continue;
            const int cr = countRow(r, c);
            int stretch = 0;
            for (int i = r+1; i < m_nrows; i++) {
                if (cell(i, c))
                    break;
                if (countRow(i, c) < cr)
                    break;
                if (isWidgetStartRow(i))
                    break;
                if (isWidgetEndRow(i)) {
                    stretch = i - r;
                    break;
                }
            }
            if (stretch) {
                for (int i = 0; i < stretch; i++)
                    setRow(r+i+1, c, w, cr);
            }
        }
    }
}

void Grid::simplify()
{
    switch (m_mode) {
    case GridLayout:
        // Grid: Extend all widgets to occupy most space and delete
        // rows/columns that are not bordering on a widget
        extendLeft();
        extendRight();
        extendUp();
        extendDown();
        shrink();
        break;
    case FormLayout:
        // Form: First treat it as a grid to get the same behaviour
        // regarding spanning and shrinking. Then restrict the span to
        // the horizontal span possible in the form, simplify again
        // and spread the widgets over a 2-column layout
        extendLeft();
        extendRight();
        extendUp();
        extendDown();
        shrink();
        if (shrinkFormLayoutSpans())
            shrink();
        reallocFormLayout();
        break;
    }

}

void Grid::shrink()
{
    //  tick off the occupied cols/rows (bordering on widget edges)
    QVector<bool> columns(m_ncols, false);
    QVector<bool> rows(m_nrows, false);

    for (int c = 0; c < m_ncols; c++)
        for (int r = 0; r < m_nrows; r++)
            if (isWidgetTopLeft(r, c))
                rows[r] = columns[c] = true;

    // remove empty cols/rows
    const int simplifiedNCols = columns.count(true);
    const int simplifiedNRows = rows.count(true);
    if (simplifiedNCols ==  m_ncols && simplifiedNRows == m_nrows)
        return;
    // reallocate and copy omitting the empty cells
    QWidget **simplifiedCells = new QWidget*[simplifiedNCols * simplifiedNRows];
    qFill(simplifiedCells, simplifiedCells + simplifiedNCols * simplifiedNRows, static_cast<QWidget *>(0));
    QWidget **simplifiedPtr = simplifiedCells;

    for (int r = 0; r < m_nrows; r++)
        if (rows[r])
            for (int c = 0; c < m_ncols; c++)
                if (columns[c]) {
                    if (QWidget *w = cell(r, c))
                        *simplifiedPtr = w;
                    simplifiedPtr++;
                }
    Q_ASSERT(simplifiedPtr == simplifiedCells + simplifiedNCols * simplifiedNRows);
    delete [] m_cells;
    m_cells = simplifiedCells;
    m_nrows = simplifiedNRows;
    m_ncols = simplifiedNCols;
}

bool Grid::shrinkFormLayoutSpans()
{
    bool shrunk = false;
    typedef  QSet<QWidget*> WidgetSet;
    // Determine unique set of widgets
    WidgetSet widgets;
    QWidget **end =  m_cells + m_ncols * m_nrows;
    for (QWidget **wptr = m_cells; wptr < end; wptr++)
        if (QWidget *w = *wptr)
            widgets.insert(w);
    // Restrict the widget span: max horizontal span at column 0: 2, anything else: 1
    const int maxRowSpan = 1;
    const WidgetSet::const_iterator cend = widgets.constEnd();
    for (WidgetSet::const_iterator it = widgets.constBegin(); it != cend ; ++it) {
        QWidget *w = *it;
        int row, col,  rowspan, colspan;
        locateWidget(w, row, col, rowspan, colspan);
        const int maxColSpan = col == 0 ? 2 : 1;
        const int newColSpan = qMin(colspan, maxColSpan);
        const int newRowSpan = qMin(rowspan, maxRowSpan);
        if (newColSpan != colspan || newRowSpan != rowspan) {
            setCells(QRect(col, row, colspan, rowspan), 0);
            setCells(QRect(col, row, newColSpan, newRowSpan), w);
            shrunk = true;
        }
    }
    return shrunk;
}

void Grid::reallocFormLayout()
{
    // Columns matching? -> happy!
    if (m_ncols == FormLayoutColumns)
        return;

    // If there are offset columns (starting past the field column),
    // move them to the left and squeeze them. This also prevents the
    // following reallocation from creating empty form rows.
    int pastRightWidgetCount = 0;
    if (m_ncols > FormLayoutColumns) {
        for (int r = 0; r < m_nrows; r++) {
            // Try to find a column where the form columns are empty and
            // there are widgets further to the right.
            if (cell(r, 0) == 0 && cell(r, 1) == 0) {
                int sourceCol = FormLayoutColumns;
                QWidget *firstWidget = 0;
                for ( ; sourceCol < m_ncols; sourceCol++)
                    if (QWidget *w = cell(r, sourceCol)) {
                        firstWidget = w;
                        break;
                    }
                if (firstWidget) {
                    // Move/squeeze. Copy to beginning of column if it is a label, else field
                    int targetCol = qobject_cast<QLabel*>(firstWidget) ? 0 : 1;
                    for ( ; sourceCol < m_ncols; sourceCol++)
                        if (QWidget *w = cell(r, sourceCol))
                            setCell(r,  targetCol++, w);
                    // Pad with zero
                    for ( ; targetCol < m_ncols; targetCol++)
                        setCell(r, targetCol, 0);
                }
            }
            // Any protruding widgets left on that row?
            for (int c = FormLayoutColumns; c < m_ncols; c++)
                if (cell(r, c))
                    pastRightWidgetCount++;
        }
    }
    // Reallocate with 2 columns. Just insert the protruding ones as fields.
    const int formNRows = m_nrows + pastRightWidgetCount;
    QWidget **formCells = new QWidget*[FormLayoutColumns * formNRows];
    qFill(formCells, formCells + FormLayoutColumns * formNRows, static_cast<QWidget *>(0));
    QWidget **formPtr = formCells;
    const int matchingColumns = qMin(m_ncols, static_cast<int>(FormLayoutColumns));
    for (int r = 0; r < m_nrows; r++) {
        int c = 0;
        for ( ; c < matchingColumns; c++)               // Just copy over matching columns
             *formPtr++ = cell(r, c);
        formPtr += FormLayoutColumns - matchingColumns; // In case old format was 1 column
        // protruding widgets: Insert as single-field rows
        for ( ; c < m_ncols; c++)
            if (QWidget *w = cell(r, c)) {
                formPtr++;
                *formPtr++ = w;
            }
    }
    Q_ASSERT(formPtr == formCells + FormLayoutColumns * formNRows);
    delete [] m_cells;
    m_cells = formCells;
    m_nrows = formNRows;
    m_ncols = FormLayoutColumns;
}

bool Grid::locateWidget(QWidget *w, int &row, int &col, int &rowspan, int &colspan) const
{
    const int end = m_nrows * m_ncols;
    const int startIndex = qFind(m_cells, m_cells + end, w) - m_cells;
    if (startIndex == end)
        return false;

    row = startIndex / m_ncols;
    col = startIndex % m_ncols;
    for (rowspan = 1; row + rowspan < m_nrows && cell(row + rowspan, col) == w; rowspan++) {}
    for (colspan = 1; col + colspan < m_ncols && cell(row, col + colspan) == w; colspan++) {}
    return true;
}

// QGridLayout/QFormLayout Helpers: get item position/add item (overloads to make templates work)

void getGridItemPosition(QGridLayout *gridLayout, int index, int *row, int *column, int *rowspan, int *colspan)
{
    gridLayout->getItemPosition(index, row, column, rowspan, colspan);
}

void addWidgetToGrid(QGridLayout *lt, QWidget * widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
    lt->addWidget(widget, row, column, rowSpan, columnSpan, alignment);
}

inline void getGridItemPosition(QFormLayout *formLayout, int index, int *row, int *column, int *rowspan, int *colspan)
{
    getFormLayoutItemPosition(formLayout, index, row, column, rowspan, colspan);
}

inline void addWidgetToGrid(QFormLayout *lt, QWidget * widget, int row, int column, int, int columnSpan, Qt::Alignment)
{
    formLayoutAddWidget(lt, widget, QRect(column, row,  columnSpan, 1), false);
}

// ----------- Base template for grid like layouts
template <class GridLikeLayout, int LayoutType, int GridMode>
class GridLayout : public Layout
{
public:
    GridLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb);

    virtual void doLayout();
    virtual void sort()                  { setWidgets(buildGrid(widgets())); }

protected:
    QWidget *widgetAt(GridLikeLayout *layout, int row, int column) const;

protected:
    QWidgetList buildGrid(const QWidgetList &);
    Grid m_grid;
};

template <class GridLikeLayout, int LayoutType, int GridMode>
GridLayout<GridLikeLayout, LayoutType, GridMode>::GridLayout(const QWidgetList &wl, QWidget *p, QDesignerFormWindowInterface *fw, QWidget *lb) :
    Layout(wl, p, fw, lb, LayoutInfo::Grid),
    m_grid(static_cast<Grid::Mode>(GridMode))
{
}

template <class GridLikeLayout, int LayoutType, int GridMode>
QWidget *GridLayout<GridLikeLayout, LayoutType, GridMode>::widgetAt(GridLikeLayout *layout, int row, int column) const
{
    int index = 0;
    while (QLayoutItem *item = layout->itemAt(index)) {
        if (item->widget()) {
            int r, c, rowspan, colspan;
            getGridItemPosition(layout, index, &r, &c, &rowspan, &colspan);
            if (row == r && column == c)
                return item->widget();
        }
        ++index;
    }
    return 0;
}

template <class GridLikeLayout, int LayoutType, int GridMode>
void GridLayout<GridLikeLayout, LayoutType, GridMode>::doLayout()
{
    bool needMove, needReparent;
    if (!prepareLayout(needMove, needReparent))
        return;

    GridLikeLayout *layout =  static_cast<GridLikeLayout *>(createLayout(LayoutType));

    if (m_grid.empty())
        sort();

    QDesignerWidgetItemInstaller wii; // Make sure we use QDesignerWidgetItem.

    const  QWidgetList::const_iterator cend = widgets().constEnd();
    for (QWidgetList::const_iterator it =  widgets().constBegin(); it != cend; ++it) {
        QWidget *w = *it;
        int r = 0, c = 0, rs = 0, cs = 0;

        if (m_grid.locateWidget(w, r, c, rs, cs)) {
            if (needReparent)
                reparentToLayoutBase(w);

            Qt::Alignment alignment = Qt::Alignment(0);
            if (const Spacer *spacer = qobject_cast<const Spacer*>(w))
                alignment = spacer->alignment();

            if (rs * cs == 1) {
                addWidgetToGrid(layout, w, r, c, 1, 1, alignment);
            } else {
                addWidgetToGrid(layout, w, r, c, rs, cs, alignment);
            }

            w->show();
        } else {
            qDebug("ooops, widget '%s' does not fit in layout", w->objectName().toUtf8().constData());
        }
    }

    QLayoutSupport::createEmptyCells(layout);

    finishLayout(needMove, layout);
}

// Remove duplicate entries (Remove next, if equal to current)
void removeIntVecDuplicates(QVector<int> &v)
{
    if (v.size() < 2)
        return;

    for (QVector<int>::iterator current = v.begin() ; (current != v.end()) && ((current+1) != v.end()) ; )
        if ( (*current == *(current+1)) )
            v.erase(current+1);
        else
            ++current;
}

// Ensure a non-zero size for a widget geometry (squeezed spacers)
inline QRect expandGeometry(const QRect &rect)
{
    return rect.isEmpty() ? QRect(rect.topLeft(), rect.size().expandedTo(QSize(1, 1))) : rect;
}

template <class GridLikeLayout, int LayoutType, int GridMode>
QWidgetList GridLayout<GridLikeLayout, LayoutType, GridMode>::buildGrid(const QWidgetList &widgetList)
{
    if (widgetList.empty())
        return QWidgetList();

    // Pixel to cell conversion:
    // By keeping a list of start'n'stop values (x & y) for each widget,
    // it is possible to create a very small grid of cells to represent
    // the widget layout.
    // -----------------------------------------------------------------

    // We need a list of both start and stop values for x- & y-axis
    const int widgetCount = widgetList.size();
    QVector<int> x( widgetCount * 2 );
    QVector<int> y( widgetCount * 2 );

    // Using push_back would look nicer, but operator[] is much faster
    int index  = 0;
    for (int i = 0; i < widgetCount; ++i) {
        const QRect widgetPos = expandGeometry(widgetList.at(i)->geometry());
        x[index]   = widgetPos.left();
        x[index+1] = widgetPos.right();
        y[index]   = widgetPos.top();
        y[index+1] = widgetPos.bottom();
        index += 2;
    }

    qSort(x);
    qSort(y);

    // Remove duplicate x entries (Remove next, if equal to current)
    removeIntVecDuplicates(x);
    removeIntVecDuplicates(y);

    // Note that left == right and top == bottom for size 1 items; reserve
    // enough space
    m_grid.resize(y.size(), x.size());

    const  QWidgetList::const_iterator cend = widgetList.constEnd();
    for (QWidgetList::const_iterator it = widgetList.constBegin(); it != cend; ++it) {
        QWidget *w = *it;
        // Mark the cells in the grid that contains a widget
        const QRect widgetPos = expandGeometry(w->geometry());
        QRect c(0, 0, 0, 0); // rect of columns/rows

        // From left til right (not including)
        const int leftIdx = x.indexOf(widgetPos.left());
        Q_ASSERT(leftIdx != -1);
        c.setLeft(leftIdx);
        c.setRight(leftIdx);
        for (int cw=leftIdx; cw<x.size(); cw++)
            if (x[cw] <  widgetPos.right())
                c.setRight(cw);
            else
                break;
        // From top til bottom (not including)
        const int topIdx = y.indexOf(widgetPos.top());
        Q_ASSERT(topIdx != -1);
        c.setTop(topIdx);
        c.setBottom(topIdx);
        for (int ch=topIdx; ch<y.size(); ch++)
            if (y[ch] <  widgetPos.bottom())
                c.setBottom(ch);
            else
                break;
        m_grid.setCells(c, w); // Mark cellblock
    }

    m_grid.simplify();

    QWidgetList ordered;
    for (int i = 0; i < m_grid.numRows(); i++)
        for (int j = 0; j < m_grid.numCols(); j++) {
            QWidget *w = m_grid.cell(i, j);
            if (w && !ordered.contains(w))
                ordered.append(w);
        }
    return ordered;
}
} // anonymous

Layout* Layout::createLayout(const QWidgetList &widgets,  QWidget *parentWidget,
                             QDesignerFormWindowInterface *fw,
                             QWidget *layoutBase, LayoutInfo::Type layoutType)
{
    switch (layoutType) {
    case LayoutInfo::Grid:
        return new GridLayout<QGridLayout, LayoutInfo::Grid, Grid::GridLayout>(widgets, parentWidget, fw, layoutBase);
    case LayoutInfo::HBox:
    case LayoutInfo::VBox: {
        const Qt::Orientation orientation = layoutType == LayoutInfo::HBox ? Qt::Horizontal : Qt::Vertical;
        return new BoxLayout(widgets, parentWidget, fw, layoutBase, orientation);
    }
    case LayoutInfo::HSplitter:
    case LayoutInfo::VSplitter: {
        const Qt::Orientation orientation = layoutType == LayoutInfo::HSplitter ? Qt::Horizontal : Qt::Vertical;
        return new SplitterLayout(widgets, parentWidget, fw, layoutBase, orientation);
    }
    case LayoutInfo::Form:
        return new GridLayout<QFormLayout, LayoutInfo::Form, Grid::FormLayout>(widgets, parentWidget, fw, layoutBase);
    default:
        break;
    }
    Q_ASSERT(0);
    return 0;
}

} // namespace qdesigner_internal

QT_END_NAMESPACE
