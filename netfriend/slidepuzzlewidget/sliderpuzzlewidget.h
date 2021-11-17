#ifndef SLIDERPUZZLEWIDGET_H
#define SLIDERPUZZLEWIDGET_H

#include <QWidget>
#include "ui_sliderpuzzlewidget.h"

class SliderPuzzleWidget : public QWidget
{
	Q_OBJECT

public:
	SliderPuzzleWidget(QWidget *parent = 0);
	~SliderPuzzleWidget();

private:
	void initForm();

private slots:
	void onUpdateWidget();
	void onSliderValueChanged(int value);
	void onSliderReleased();

private:
	Ui::SliderPuzzleWidget ui;
};

#endif // SLIDERPUZZLEWIDGET_H
