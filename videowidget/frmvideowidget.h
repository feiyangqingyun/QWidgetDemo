#ifndef FRMVIDEOWIDGET_H
#define FRMVIDEOWIDGET_H

#include <QWidget>

namespace Ui {
class frmVideoWidget;
}

class frmVideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoWidget(QWidget *parent = 0);
    ~frmVideoWidget();

private:
    Ui::frmVideoWidget *ui;

private slots:
    void initForm();
    void btnClicked(const QString &objName);
};

#endif // FRMVIDEOWIDGET_H
