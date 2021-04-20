#include "eventtabledelegate.h"
#include <limits>
#include <QSpinBox>
#include <QCheckBox>

EventTableDelegate::EventTableDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget * EventTableDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const
{
    if (index.column() == 1 ||
        index.column() == 2 ||
        index.column() == 3)
    {
        QSpinBox * editor = new QSpinBox(parent);
        editor->setMinimum(0);
        if (index.column() == 1)
            editor->setMaximum(INT_MAX);
        else if (index.column() == 2)
            editor->setMaximum(0xFF);
        else if (index.column() == 3)
            editor->setMaximum(0xFFFF);

        int value = index.data(Qt::EditRole).toInt();
        editor->setValue(value);

        return editor;
    }
    else if (index.column() == 4)
    {
        QCheckBox * editor = new QCheckBox(parent);
        return editor;
    }
    else
        return QStyledItemDelegate::createEditor(parent, option, index);
}

void EventTableDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    if (index.column() == 1 ||
        index.column() == 2 ||
        index.column() == 3 )
    {
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QSpinBox * thisEditor = qobject_cast<QSpinBox *>(editor);
        thisEditor->setValue(value);
    }
    else if (index.column() == 4)
    {
        bool value = index.model()->data(index, Qt::EditRole).toBool();
        QCheckBox * thisEditor = qobject_cast<QCheckBox *>(editor);
        thisEditor->setChecked(value);
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
}

void EventTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    if (index.column() == 1 ||
        index.column() == 2 ||
        index.column() == 3 )
    {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();

        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == 4)
    {
        QCheckBox *checkBox = static_cast<QCheckBox*>(editor);
        bool value = checkBox->isChecked();

        model->setData(index, value, Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

void EventTableDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
