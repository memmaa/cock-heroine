#ifndef EVENTTABLEDELEGATE_H
#define EVENTTABLEDELEGATE_H

#include <QStyledItemDelegate>

class EventTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit EventTableDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

};

#endif // EVENTTABLEDELEGATE_H
