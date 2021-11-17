#include "graphicspixmap.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QCursor>
#include <QDebug>

GraphicsPixmap::GraphicsPixmap() : QGraphicsObject()
{
	setCacheMode(DeviceCoordinateCache);
}

void GraphicsPixmap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsObject::mousePressEvent(event);
	if (event->button() == Qt::LeftButton)
	{
		emit clicked();
	}
}

void GraphicsPixmap::setItemOffset(QPointF ponit)
{
	prepareGeometryChange();
	offset = ponit;
	update();
}

QPointF GraphicsPixmap::itemoffset()
{
	return offset;
}

void GraphicsPixmap::setPixmap(const QPixmap& pixmap)
{
	pixSize = pixmap.size();
	pix = pixmap;
}

void GraphicsPixmap::setPixmapSize(QSize size)
{
	pixSize = size;
}

QSize GraphicsPixmap::pixsize()
{
	return pixSize;
}

QRectF GraphicsPixmap::boundingRect() const
{
	return QRectF(offset, pix.size() / pix.devicePixelRatio());
}

void GraphicsPixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter->drawPixmap(offset, pix.scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}