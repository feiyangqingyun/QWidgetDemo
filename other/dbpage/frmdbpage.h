#ifndef FRMDBPAGE_H
#define FRMDBPAGE_H

#include <QWidget>

class DbPage;

namespace Ui {
class frmDbPage;
}

class frmDbPage : public QWidget
{
    Q_OBJECT

public:
    explicit frmDbPage(QWidget *parent = 0);
    ~frmDbPage();

private:
    Ui::frmDbPage *ui;

    QList<QString> columnNames; //字段名集合
    QList<int> columnWidths;    //字段宽度集合
    DbPage *dbPage;             //数据库翻页类

    QString tableName;          //表名称
    QString countName;          //统计行数字段名称

private slots:
    void initForm();

private slots:
    void on_btnSelect_clicked();
};

#endif // FRMDBPAGE_H
