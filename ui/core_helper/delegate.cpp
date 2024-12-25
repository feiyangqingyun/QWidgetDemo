#include "delegate.h"
#include "qcombobox.h"
#include "qdebug.h"

DelegateComboBox::DelegateComboBox(const QStringList &delegateValue, QObject *parent) : QStyledItemDelegate(parent)
{
    this->delegateValue = delegateValue;
}

QWidget *DelegateComboBox::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *cbox = new QComboBox(parent);
    cbox->addItems(delegateValue);
    return cbox;
}

void DelegateComboBox::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString data = index.data(Qt::DisplayRole).toString();
    QComboBox *cbox = static_cast<QComboBox *>(editor);
    cbox->setCurrentIndex(cbox->findText(data));
}

void DelegateComboBox::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cbox = static_cast<QComboBox *>(editor);
    QString data = cbox->currentText();
    model->setData(index, data);
}
