#ifndef EVENTDATAPROXYMODEL_H
#define EVENTDATAPROXYMODEL_H

#include <QSortFilterProxyModel>

class Event;
class eventDataModel;

class EventDataProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EventDataProxyModel(QObject *parent = 0);

    Event eventFromIndex(const QModelIndex & index);
    void setEventAtIndex(const QModelIndex & indexToChange, const Event & event);
    void addEvent(const Event eventToAdd);
    void removeEvent(int indexToDelete);

signals:

public slots:

private:
    eventDataModel * source;
};

#endif // EVENTDATAPROXYMODEL_H
