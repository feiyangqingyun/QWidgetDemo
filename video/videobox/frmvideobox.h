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

private:
    Ui::frmVideoBox *ui;

    QMenu *menu;
    VideoBox *box;

private slots:
    void initForm();
    void doAction();
    void layoutChanged(int type, const QString &layoutType, bool isMax);
};

#endif // FRMVIDEOBOX_H
