#ifndef IMAGEVIEWWINDOW_H
#define IMAGEVIEWWINDOW_H

#include <QtWidgets/QWidget>
#include "ui_imageviewwindow.h"

class QTimer;
class QPropertyAnimation;
class GraphicsPixmap;
class QGraphicsScene;
class QParallelAnimationGroup;
class ImageViewWindow : public QWidget
{
	Q_OBJECT

public:
	ImageViewWindow(QWidget *parent = 0);
	~ImageViewWindow();

private:
	void initControl();
	void nextPlay();
	void lastPlay();
	void play();

private slots:
	void onLeftBtnClicked();
	void onRightBtnClicked();

private:
	Ui::ImageViewWindowClass ui;
	QGraphicsScene* m_scene;
    QList<QMap<QString, QString> > m_imgMapInfolst;
	QList<GraphicsPixmap *> m_items;
	QParallelAnimationGroup *m_group;
	QMap<GraphicsPixmap *, QPropertyAnimation *> m_animationMap;
	QMap<GraphicsPixmap *, QPropertyAnimation *> m_animationsMap;
	QTimer* m_timer;
	bool m_isStart;
};

#endif // IMAGEVIEWWINDOW_H
