#ifndef _PLOT_H
#define _PLOT_H

#include <qwt_plot.h>
#include "shapefactory.h"

class QColor;
class QSizeF;
class QPointF;
class Editor;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    Plot( QWidget *parent = NULL );

public Q_SLOTS:
    void exportPlot();
    void setMode( int );
    
private:
    void populate();

    void addShape( const QString &title,
        ShapeFactory::Shape, const QColor &,
        const QPointF &, const QSizeF & );

    Editor *d_editor;
};

#endif
