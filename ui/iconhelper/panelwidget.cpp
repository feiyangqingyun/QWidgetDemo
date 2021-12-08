#pragma execution_character_set("utf-8")

#include "panelwidget.h"
#include "qscrollarea.h"
#include "qframe.h"
#include "qboxlayout.h"
#include "qdebug.h"

PanelWidget::PanelWidget(QWidget *parent) : QWidget(parent)
{
    scrollArea = new QScrollArea(this);
    scrollArea->setObjectName("scrollAreaMain");
    scrollArea->setWidgetResizable(true);

    scrollAreaContents = new QWidget();
    scrollAreaContents->setGeometry(QRect(0, 0, 100, 100));

    verticalLayout = new QVBoxLayout(scrollAreaContents);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    frame = new QFrame(scrollAreaContents);
    frame->setObjectName("frameMain");

    gridLayout = new QGridLayout(frame);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    verticalLayout->addWidget(frame);
    scrollArea->setWidget(scrollAreaContents);
    frame->setStyleSheet("QFrame#frameMain{border-width:0px;}");

    margin = 0;
    space = 0;
    autoWidth = false;
    autoHeight = false;
}

void PanelWidget::resizeEvent(QResizeEvent *)
{
    scrollArea->resize(this->size());
}

int PanelWidget::getMargin() const
{
    return this->margin;
}

int PanelWidget::getSpace() const
{
    return this->space;
}

bool PanelWidget::getAutoWidth() const
{
    return this->autoWidth;
}

bool PanelWidget::getAutoHeight() const
{
    return this->autoHeight;
}

int PanelWidget::getColumnCount() const
{
    return this->columnCount;
}

QList<QWidget *> PanelWidget::getWidgets() const
{
    return this->widgets;
}

QSize PanelWidget::sizeHint() const
{
    return QSize(300, 200);
}

QSize PanelWidget::minimumSizeHint() const
{
    return QSize(20, 20);
}

void PanelWidget::setMargin(int left, int top, int right, int bottom)
{
    gridLayout->setContentsMargins(left, top, right, bottom);
}

void PanelWidget::setMargin(int margin)
{
    if (this->margin != margin) {
        setMargin(margin, margin, margin, margin);
    }
}

void PanelWidget::setSpace(int space)
{
    if (this->space != space) {
        gridLayout->setSpacing(space);
    }
}

void PanelWidget::setAutoWidth(bool autoWidth)
{
    if (this->autoWidth != autoWidth) {
        this->autoWidth = autoWidth;
    }
}

void PanelWidget::setAutoHeight(bool autoHeight)
{
    if (this->autoHeight != autoHeight) {
        this->autoHeight = autoHeight;
    }
}

void PanelWidget::setColumnCount(int columnCount)
{
    if (this->columnCount != columnCount) {
        this->columnCount = columnCount;
    }
}

void PanelWidget::setWidgets(QList<QWidget *> widgets)
{
    this->widgets = widgets;
    this->loadWidgets();
}

void PanelWidget::loadWidgets()
{
    int row = 0;
    int column = 0;
    int index = 0;

    //先把之前的所有移除并不可见
    foreach (QWidget *widget, widgets) {
        gridLayout->removeWidget(widget);
        widget->setVisible(false);
    }

    //重新添加到布局中并可见
    foreach (QWidget *widget, widgets) {
        gridLayout->addWidget(widget, row, column);
        widget->setVisible(true);

        column++;
        index++;
        if (index % columnCount == 0) {
            row++;
            column = 0;
        }
    }

    row++;

    //设置右边弹簧
    if (!autoWidth) {
        QSpacerItem *hSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
        gridLayout->addItem(hSpacer, 0, gridLayout->columnCount());
    }

    //设置底边弹簧
    if (!autoHeight) {
        QSpacerItem *vSpacer = new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
        gridLayout->addItem(vSpacer, row, 0);
    }
}

void PanelWidget::insertWidget(int index, QWidget *widget)
{
    this->widgets.insert(index, widget);
    this->loadWidgets();
}

void PanelWidget::removeWidget(QWidget *widget)
{
    this->widgets.removeOne(widget);
    this->loadWidgets();
}

void PanelWidget::clearWidgets()
{
    qDeleteAll(this->widgets);
    this->widgets.clear();
}
