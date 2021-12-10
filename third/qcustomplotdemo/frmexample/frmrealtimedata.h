#ifndef FRMREALTIMEDATA_H
#define FRMREALTIMEDATA_H

#include <QWidget>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class frmRealtimeData;
}

class frmRealtimeData : public QWidget
{
    Q_OBJECT

public:
    explicit frmRealtimeData(QWidget *parent = 0);
    ~frmRealtimeData();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::frmRealtimeData *ui;
    QTimer dataTimer;
    QTime timeStart;

private slots:
    void initForm();
    void realtimeDataSlot();
};

#endif // FRMREALTIMEDATA_H
