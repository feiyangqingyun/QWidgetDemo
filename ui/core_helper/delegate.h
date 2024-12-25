#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>

class DelegateComboBox : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DelegateComboBox(const QStringList &delegateValue, QObject *parent = 0);

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    QStringList delegateValue;
};

#endif // DELEGATE_H
