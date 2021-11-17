#ifndef GRAPHICSPIXMAP_H
#define GRAPHICSPIXMAP_H

#include <QObject>
#include <QGraphicsObject> 
#include <QPixmap>

class GraphicsPixmap : public QGraphicsObject
{
	Q_OBJECT
	Q_PROPERTY(QPointF itemoffset READ itemoffset WRITE setItemOffset)
	Q_PROPERTY(QSize itemsize READ pixsize WRITE setPixmapSize)

public:
	GraphicsPixmap();

public:
	QRectF boundingRect() const Q_DECL_OVERRIDE;
	void setItemOffset(QPointF ponit);
	QPointF itemoffset();
	QSize pixsize();
	void setPixmap(const QPixmap& pixmap);
	void setPixmapSize(QSize size);

signals:
	void clicked();

private:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) Q_DECL_OVERRIDE;

private:
	QPixmap pix;
	QPointF offset;
	QSize pixSize;
};

#endif // GRAPHICSPIXMAP_H
