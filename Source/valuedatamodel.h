#ifndef VALUEDATAMODEL_H
#define VALUEDATAMODEL_H

#include <QAbstractTableModel>
#include <QVector>

class BeatValue;

class ValueDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ValueDataModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void addValue(const BeatValue & valueToAdd);
    bool removeRow (int row);
    void recalculateCounts();

signals:

public slots:

private:
    mutable QVector<int> counts;

};

#endif // VALUEDATAMODEL_H
