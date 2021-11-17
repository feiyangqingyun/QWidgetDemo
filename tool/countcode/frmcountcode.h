#ifndef FRMCOUNTCODE_H
#define FRMCOUNTCODE_H

#include <QWidget>

namespace Ui {
class frmCountCode;
}

class frmCountCode : public QWidget
{
    Q_OBJECT

public:
    explicit frmCountCode(QWidget *parent = 0);
    ~frmCountCode();

private:
    Ui::frmCountCode *ui;
    QStringList listFile;

private:
    void initForm();
    bool checkFile(const QString &fileName);
    void countCode(const QString &filePath);
    void countCode(const QStringList &files);
    void countCode(const QString &fileName, int &lineCode, int &lineBlank, int &lineNotes);

private slots:
    void on_btnOpenFile_clicked();
    void on_btnOpenPath_clicked();
    void on_btnClear_clicked();
};

#endif // FRMCOUNTCODE_H
