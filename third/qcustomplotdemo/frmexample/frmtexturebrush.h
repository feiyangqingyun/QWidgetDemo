#ifndef FRMTEXTUREBRUSH_H
#define FRMTEXTUREBRUSH_H

#include <QWidget>

namespace Ui {
class frmTextureBrush;
}

class frmTextureBrush : public QWidget
{
    Q_OBJECT

public:
    explicit frmTextureBrush(QWidget *parent = 0);
    ~frmTextureBrush();

private:
    Ui::frmTextureBrush *ui;

private slots:
    void initForm();
};

#endif // FRMTEXTUREBRUSH_H
