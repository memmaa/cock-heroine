#ifndef PATTERNDATAMODEL_H
#define PATTERNDATAMODEL_H

#include <QAbstractTableModel>

class PatternDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PatternDataModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

signals:

public slots:

};

#endif // PATTERNDATAMODEL_H
