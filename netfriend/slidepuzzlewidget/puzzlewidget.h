#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QWidget>

class PuzzleWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString pixmap READ getPixmap WRITE setPixmap)

public:
	PuzzleWidget(QWidget *parent);
	~PuzzleWidget();

public:
	QString getPixmap() const { return m_pixmap; };
	void setPixmap(const QString& pixmap);

	void setValue(int value);
	bool isOverlap();

private slots:
	void onUpdatePixmap();

protected:
	void paintEvent(QPaintEvent *event);

private:
	int m_value;
	QString m_pixmap;
	QPoint m_offsetPoint;
};

#endif // PUZZLEWIDGET_H
