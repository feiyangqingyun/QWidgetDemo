#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	GraphicsView(QGraphicsScene *scene);
	~GraphicsView();

protected:
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;	
};

#endif // GRAPHICSVIEW_H
