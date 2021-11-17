#include <QWidget>
#include "ui_FrmAStackWidget.h"

class FrmAStackWidget : public QWidget
{
    Q_OBJECT

public:
    FrmAStackWidget(QWidget *parent = Q_NULLPTR);

private:
	Ui::FrmAStackWidgetClass ui;
};
