#ifndef FRMSLIDEPUZZLEWIDGET_H
#define FRMSLIDEPUZZLEWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_frmslidepuzzlewidget.h"

class FrmSlidePuzzleWidget : public QWidget
{
	Q_OBJECT

public:
	FrmSlidePuzzleWidget(QWidget *parent = 0);
	~FrmSlidePuzzleWidget();

private:
	void initForm();

private:
	Ui::FrmSlidePuzzleWidgetClass ui;
};

#endif // FRMSLIDEPUZZLEWIDGET_H
