#ifndef QUIDATESELECT_H
#define QUIDATESELECT_H

#include "quihead.h"

class QUIDateSelect : public QDialog
{
    Q_OBJECT

public:
    static QUIDateSelect *Instance();
    explicit QUIDateSelect(QWidget *parent = 0);
    ~QUIDateSelect();

protected:
    void showEvent(QShowEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    static QScopedPointer<QUIDateSelect> self;

    QVBoxLayout *verticalLayout;
    QWidget *widgetTitle;
    QHBoxLayout *horizontalLayout1;
    QLabel *labIco;
    QLabel *labTitle;
    QWidget *widgetMenu;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnMenu_Close;
    QWidget *widgetMain;
    QVBoxLayout *verticalLayout1;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *labStart;
    QPushButton *btnOk;
    QLabel *labEnd;
    QPushButton *btnClose;
    QDateTimeEdit *dateStart;
    QDateTimeEdit *dateEnd;

private:
    QString startDateTime;  //开始时间
    QString endDateTime;    //结束时间
    QString format;         //日期时间格式

private slots:
    void initControl();     //初始化控件
    void initForm();        //初始化窗体

private slots:
    void on_btnOk_clicked();
    void on_btnMenu_Close_clicked();

public:
    //获取当前选择的开始时间和结束时间
    QString getStartDateTime()  const;
    QString getEndDateTime()    const;

public Q_SLOTS:
    void setIconMain(const QChar &icon, quint32 size = 12);
    void setFormat(const QString &format);
};

#endif // QUIDATESELECT_H
