#include "astackwidget.h"

#include <QDebug>
#include <QPropertyAnimation>

AStackWidget::AStackWidget(QWidget *parent)
	: QWidget(parent)
{
	m_offset = 0;
	m_curIndex = 0;
    m_lastIndex = 0;
	m_duration = 500;
	m_moveAnimation = new QPropertyAnimation(this, "");
	m_moveAnimation->setDuration(m_duration);
	connect(m_moveAnimation, &QPropertyAnimation::valueChanged, this, &AStackWidget::onValueChanged);
	connect(m_moveAnimation, &QPropertyAnimation::finished, this, &AStackWidget::onMoveFinished);
}

AStackWidget::~AStackWidget()
{

}

int AStackWidget::count() const
{
	return m_widgetLst.size();
}

int AStackWidget::currentIndex() const
{
	return m_curIndex;
}

void AStackWidget::setDuration(int duration)
{
	m_duration = duration;
}

int AStackWidget::addWidget(QWidget * widget)
{
	int index = indexOf(widget);
	if (index >= 0){
		return index;
	}
	widget->setParent(this);
	m_widgetLst.append(widget);
	return count() - 1;
}

int AStackWidget::indexOf(QWidget * widget) const
{
	return m_widgetLst.indexOf(widget);
}

int AStackWidget::insertWidget(int index, QWidget * widget)
{
	int curindex = indexOf(widget);
	if (curindex >= 0) {
		return curindex;
	}
	widget->setParent(this);
	m_widgetLst.insert(index, widget);
	return index;
}

QWidget * AStackWidget::currentWidget() const
{
	if (m_curIndex >= 0 && m_curIndex < count()){
		return m_widgetLst.at(m_curIndex);
	}
    return 0;
}

QWidget * AStackWidget::widget(int index) const
{
	if (index >= 0 && index < count()) {
		return m_widgetLst.at(index);
	}
    return 0;
}

void AStackWidget::removeWidget(QWidget * widget)
{
	int index = indexOf(widget);
	if (index >= 0) {
		m_widgetLst.removeAll(widget);
		emit widgetRemoved(index);
	}
}

void AStackWidget::setCurrentWidget(QWidget * widget)
{
	int index = indexOf(widget);
	if (index >= 0 && m_curIndex != index) {
		setCurrentIndex(index);
	}
}

void AStackWidget::setCurrentIndex(int index)
{
	if (index >= 0 && m_curIndex != index) {
		m_lastIndex = m_curIndex;
		m_curIndex = index;	
		moveAnimationStart();
		emit currentChanged(index);
	}
}

void AStackWidget::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);

	int size = count();
	for (int i = 0; i < size; i++) {
		m_widgetLst.at(i)->resize(this->width(), this->height());
	}

	if (m_moveAnimation->state() == QAbstractAnimation::Running) {
		moveAnimationStart();
	}
	else {
		setWidgetsVisible();
        onValueChanged(0);
	}
}

void AStackWidget::onValueChanged(const QVariant &value)
{
	m_offset = value.toInt();
	m_widgetLst.at(m_curIndex)->move(m_offset, 0);
	if (m_curIndex > m_lastIndex) {
		m_widgetLst.at(m_lastIndex)->move(m_offset - this->width(), 0);
    } else if (m_curIndex < m_lastIndex){
		m_widgetLst.at(m_lastIndex)->move(this->width() + m_offset, 0);
	}
}

void AStackWidget::moveAnimationStart()
{
	m_moveAnimation->stop();
	setWidgetsVisible();
	int startOffset = m_offset;
	if (m_curIndex > m_lastIndex) {
		if (startOffset == 0) startOffset = this->width();
		else startOffset = this->width() - qAbs(startOffset);
	}
	else {
		if (startOffset == 0) startOffset = -this->width();
		else startOffset = qAbs(startOffset) - this->width();
	}
	m_moveAnimation->setDuration(qAbs(startOffset) * m_duration / this->width());
	m_moveAnimation->setStartValue(startOffset);
	m_moveAnimation->setEndValue(0);
	m_moveAnimation->start();
}

void AStackWidget::setWidgetsVisible()
{
	int size = count();
	for (int i = 0; i < size; i++) {
		if (m_lastIndex == i || m_curIndex == i)
			m_widgetLst.at(i)->setVisible(true);
		else {
			m_widgetLst.at(i)->setVisible(false);
		}
	}
}

void AStackWidget::onMoveFinished()
{

}
