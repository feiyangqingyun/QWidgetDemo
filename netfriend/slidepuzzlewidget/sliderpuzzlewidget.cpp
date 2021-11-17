#pragma execution_character_set("utf-8")
#include "sliderpuzzlewidget.h"

#include <QTimer>
#include <QMessageBox>

SliderPuzzleWidget::SliderPuzzleWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->initForm();
}

SliderPuzzleWidget::~SliderPuzzleWidget()
{

}

void SliderPuzzleWidget::initForm()
{
	QTimer::singleShot(10, this, SLOT(onUpdateWidget()));
	connect(ui.horizontalSlider, &QSlider::valueChanged, this, &SliderPuzzleWidget::onSliderValueChanged);
	connect(ui.horizontalSlider, &QSlider::sliderReleased, this, &SliderPuzzleWidget::onSliderReleased);
	ui.puzzlewidget->setPixmap(":/FrmSlidePuzzleWidget/Resources/back1.png");
}

void SliderPuzzleWidget::onUpdateWidget()
{
	ui.horizontalSlider->setRange(0, this->width());
}

void SliderPuzzleWidget::onSliderValueChanged(int value)
{
	ui.puzzlewidget->setValue(value);
}

void SliderPuzzleWidget::onSliderReleased()
{
	QString content = ui.puzzlewidget->isOverlap() ? "验证成功！" : "验证失败！";
	QMessageBox msgBox;
	msgBox.setWindowTitle("滑块图片验证码");
	msgBox.setText(content);
	msgBox.exec();

	static int testIndex = 1;
	testIndex = testIndex + 1 > 4 ? 1 : testIndex + 1;
	ui.horizontalSlider->setValue(0);
	ui.puzzlewidget->setPixmap(QString(":/FrmSlidePuzzleWidget/Resources/back%1.png").arg(testIndex));
}