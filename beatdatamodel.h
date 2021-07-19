#ifndef BEATDATAMODEL_H
#define BEATDATAMODEL_H

#include <QAbstractTableModel>
#include <editorwindow.h>
class EventDataProxyModel;
class Event;
class BeatTimestamp;

class BeatDataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit BeatDataModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    //! \param deleted almost always false - only true if restoring pre-deleted timestamps from an undo snapshot
    void addEvent(const Event &newEvent, int index = -1);
    void addBeat(const QModelIndex & index);
    void addBeats(EventDataProxyModel * model, const QModelIndexList & indexList);
    void addEventsFromSnapshot(EditorWin::RollbackSnapshot snapshot);

    Event eventFromRow(int row);
    void changeEventAt(int index, const Event & event);
    void changeTimestampAt(int index, long newTimestamp);

    void removeBeat(int index);
    bool removeRows (int row, int count, const QModelIndex &);

    BeatTimestamp & operator[](int index);

    void cancelQueuedDeletions();
    void writeChangesToOriginalModel();

private:
    EventDataProxyModel * originModel;
    QVector<int> indexesToDelete;

    friend void EditorWindow::createRollbackSnapshot();
    friend void EditorWindow::applySnapshot(EditorWin::RollbackSnapshot);

signals:

public slots:

};

#endif // BEATDATAMODEL_H
