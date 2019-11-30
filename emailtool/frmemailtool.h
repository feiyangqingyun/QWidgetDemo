#ifndef FRMEMAILTOOL_H
#define FRMEMAILTOOL_H

#include <QWidget>

namespace Ui
{
class frmEmailTool;
}

class frmEmailTool : public QWidget
{
	Q_OBJECT

public:
    explicit frmEmailTool(QWidget *parent = 0);
    ~frmEmailTool();

private:
    Ui::frmEmailTool *ui;

private:
	bool check();

private slots:
    void initForm();
	void receiveEmailResult(QString result);

private slots:
	void on_btnSend_clicked();
	void on_btnSelect_clicked();
	void on_cboxServer_currentIndexChanged(int index);
};

#endif // FRMEMAILTOOL_H
