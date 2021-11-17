#include <QWidget>

class QPropertyAnimation;
class AStackWidget : public QWidget
{
	Q_OBJECT

public:
	AStackWidget(QWidget *parent);
	~AStackWidget();

signals:
	void currentChanged(int index);
	void widgetRemoved(int index);

public:
	int count() const;
	int currentIndex() const;
	int addWidget(QWidget *widget);
	int indexOf(QWidget *widget) const;
	int insertWidget(int index, QWidget *widget);
	
	QWidget *currentWidget() const;	
	QWidget *widget(int index) const;

	void removeWidget(QWidget *widget);
	void setDuration(int duration);

public slots:
	void setCurrentIndex(int index);
	void setCurrentWidget(QWidget *widget);

private slots:
	void onValueChanged(const QVariant &);
	void onMoveFinished();

private:
	void moveAnimationStart();
	void setWidgetsVisible();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	int m_offset;
	int m_curIndex;
	int m_lastIndex;

	int m_duration;
	QPropertyAnimation *m_moveAnimation;
	QList<QWidget *> m_widgetLst;
};
