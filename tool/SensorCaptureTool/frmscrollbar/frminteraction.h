#ifndef FRMINTERACTION_H
#define FRMINTERACTION_H

#include <QWidget>
#include <QInputDialog>
#include "qcustomplot.h"

namespace Ui {
class frmInterAction;
}

class frmInterAction : public QWidget
{
    Q_OBJECT

public:
    explicit frmInterAction(QWidget *parent = 0);
    ~frmInterAction();

private:
    Ui::frmInterAction *ui;

private slots:
    void initForm();
    void titleDoubleClick(QMouseEvent *event);
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void addRandomGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
};

#endif // FRMINTERACTION_H
