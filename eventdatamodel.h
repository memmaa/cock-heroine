#ifndef EVENTDATAMODEL_H
#define EVENTDATAMODEL_H

#include <QAbstractTableModel>
class Event;

class eventDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit eventDataModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Event eventFromIndex(const QModelIndex &index);
    void setEventAtIndex(const QModelIndex & indexToChange, const Event & event);
    void addEvent(const Event eventToAdd);
    void removeEvent(int index);
    //bool removeRow();
    //bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );

signals:

public slots:

};

#endif // EVENTDATAMODEL_H
