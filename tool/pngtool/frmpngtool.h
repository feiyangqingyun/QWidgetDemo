#ifndef FRMPNGTOOL_H
#define FRMPNGTOOL_H

#include <QWidget>

namespace Ui {
class frmPngTool;
}

class frmPngTool : public QWidget
{
    Q_OBJECT

public:
    explicit frmPngTool(QWidget *parent = 0);
    ~frmPngTool();

private slots:
    void on_btnFile_clicked();
    void on_btnDir_clicked();
    void on_btnOk_clicked();

private:
    Ui::frmPngTool *ui;
    QStringList files;
};

#endif // FRMPNGTOOL_H
