#include "puzzlewidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QTime>
#include <QTimer>

const int squarewidth = 46;
const int squareradius = 20;
PuzzleWidget::PuzzleWidget(QWidget *parent)
	: QWidget(parent)
{
	m_value = 0;
	m_offsetPoint = QPoint(0, 0);
    srand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
}

PuzzleWidget::~PuzzleWidget()
{

}

void PuzzleWidget::setPixmap(const QString& pixmap)
{
	m_pixmap = pixmap;
	QTimer::singleShot(10, this, SLOT(onUpdatePixmap()));
}

void PuzzleWidget::onUpdatePixmap()
{
    m_offsetPoint.rx() = qBound(0, rand() % this->width() + squarewidth + squareradius, this->width() - squarewidth - squareradius);
    m_offsetPoint.ry() = qBound(0, rand() % this->height() + squarewidth + squareradius, this->height() - squarewidth - squareradius);
	update();
}

void PuzzleWidget::setValue(int value)
{
	m_value = qBound(0, value, this->width() - squarewidth - squareradius + m_offsetPoint.x());
	update();
}

void PuzzleWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	QPainterPath clippath;
	clippath.addRoundedRect(this->rect(), 4, 4);
	painter.setClipPath(clippath);
	const QPixmap& pixmap = QPixmap(m_pixmap).scaled(this->width(), this->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);

	QPainterPath cutoutpath;
	cutoutpath.setFillRule(Qt::WindingFill);
	QRect rect(m_offsetPoint, QSize(squarewidth, squarewidth));
	cutoutpath.addRoundedRect(rect, 2, 2);
	cutoutpath.addEllipse(rect.center().x() - squareradius / 2, rect.top() - squareradius + 6, squareradius, squareradius);
	QPainterPath subellipseparh;
	subellipseparh.addEllipse(rect.right() - squareradius + 6, rect.center().y() - squareradius / 2, squareradius, squareradius);
	cutoutpath -= subellipseparh;

	painter.setPen(QPen(QColor(80, 80, 80), 1));
	painter.setBrush(QColor(100, 100, 100, 220));
	painter.drawPath(cutoutpath);

	QPixmap puzzlePixmap(this->size());
	puzzlePixmap.fill(Qt::transparent);
	QPainter puzzlePainter(&puzzlePixmap);
	puzzlePainter.setRenderHints(QPainter::Antialiasing);
	puzzlePainter.setClipPath(cutoutpath);
	puzzlePainter.setPen(QPen(QColor(80, 80, 80), 2));
	puzzlePainter.setBrush(QColor(200, 200, 200, 100));
	puzzlePainter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
	puzzlePainter.drawPath(cutoutpath);

	painter.drawPixmap(-m_offsetPoint.x() + m_value, 0, this->width(), this->height(), puzzlePixmap);
}

bool PuzzleWidget::isOverlap()
{
	return qAbs(-m_offsetPoint.x() + m_value) < 5;
}
