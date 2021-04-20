#ifndef INTERVALDATAMODEL_H
#define INTERVALDATAMODEL_H

#include <QAbstractTableModel>

class BeatInterval;

class IntervalDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit IntervalDataModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool containsUnknownIntervals();
    bool containsPoorlyMatchedIntervals (float threshold = 40);

signals:

public slots:

};

#endif // INTERVALDATAMODEL_H
