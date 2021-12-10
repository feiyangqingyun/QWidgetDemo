#ifndef FRMITEM_H
#define FRMITEM_H

#include <QWidget>
#include <QTimer>
class QCPItemTracer;

namespace Ui {
class frmItem;
}

class frmItem : public QWidget
{
    Q_OBJECT

public:
    explicit frmItem(QWidget *parent = 0);
    ~frmItem();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::frmItem *ui;
    QTimer dataTimer;
    QCPItemTracer *phaseTracer;

private slots:
    void initForm();
    void bracketDataSlot();
};

#endif // FRMITEM_H
