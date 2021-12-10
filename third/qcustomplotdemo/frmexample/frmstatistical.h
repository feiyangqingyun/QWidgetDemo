#ifndef FRMSTATISTICAL_H
#define FRMSTATISTICAL_H

#include <QWidget>

namespace Ui {
class frmStatistical;
}

class frmStatistical : public QWidget
{
    Q_OBJECT

public:
    explicit frmStatistical(QWidget *parent = 0);
    ~frmStatistical();

private:
    Ui::frmStatistical *ui;

private slots:
    void initForm();
};

#endif // FRMSTATISTICAL_H
