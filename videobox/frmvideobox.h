#ifndef FRMVIDEOBOX_H
#define FRMVIDEOBOX_H

#include <QWidget>
class QMenu;
class VideoBox;

namespace Ui {
class frmVideoBox;
}

class frmVideoBox : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoBox(QWidget *parent = 0);
    ~frmVideoBox();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmVideoBox *ui;

    bool max;
    QMenu *menu;
    VideoBox *box;

private slots:
    void initForm();
    void doAction();

    //画面布局切换信号
    void changeVideo(int type, const QString &videoType, bool videoMax);
};

#endif // FRMVIDEOBOX_H
