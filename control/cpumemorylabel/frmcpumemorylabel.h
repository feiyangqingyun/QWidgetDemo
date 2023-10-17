#ifndef FRMCPUMEMORYLABEL_H
#define FRMCPUMEMORYLABEL_H

#include <QWidget>

namespace Ui {
class frmCpuMemoryLabel;
}

class frmCpuMemoryLabel : public QWidget
{
    Q_OBJECT

public:
    explicit frmCpuMemoryLabel(QWidget *parent = 0);
    ~frmCpuMemoryLabel();

private:
    Ui::frmCpuMemoryLabel *ui;

private slots:
    void initForm();
    //cpu和内存占用情况数值改变信号
    void valueChanged(quint64 cpuPercent, quint64 memoryPercent, quint64 memoryAll, quint64 memoryUse, quint64 memoryFree);
};

#endif // FRMCPUMEMORYLABEL_H
