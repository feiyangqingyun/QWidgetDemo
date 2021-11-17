#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "miniblink.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    miniblink *webView;

private slots:
    void initForm();
    //网页载入完成
    void loadFinished(bool ok);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);

private slots:
    void on_btnLoadUrl_clicked();
    void on_btnLoadFile_clicked();
    void on_btnLoadHtml_clicked();
    void on_btnRunJs_clicked();
    void on_horizontalSlider_valueChanged(int value);
};

#endif // WIDGET_H
