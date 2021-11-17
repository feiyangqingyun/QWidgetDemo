#ifndef FRMLUNARCALENDARWIDGET_H
#define FRMLUNARCALENDARWIDGET_H

#include <QWidget>

namespace Ui {
class frmLunarCalendarWidget;
}

class frmLunarCalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmLunarCalendarWidget(QWidget *parent = 0);
    ~frmLunarCalendarWidget();

private:
    Ui::frmLunarCalendarWidget *ui;

private slots:
    void initForm();
    void on_cboxCalendarStyle_currentIndexChanged(int index);
    void on_cboxSelectType_currentIndexChanged(int index);
    void on_cboxWeekNameFormat_currentIndexChanged(int index);
    void on_ckShowLunar_stateChanged(int arg1);
};

#endif // FRMLUNARCALENDARWIDGET_H
