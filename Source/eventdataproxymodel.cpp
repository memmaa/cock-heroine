#include "eventdataproxymodel.h"
#include "eventdatamodel.h"
#include "event.h"

EventDataProxyModel::EventDataProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    source = static_cast<eventDataModel *>(sourceModel());
}

Event EventDataProxyModel::eventFromIndex(const QModelIndex &index)
{
    if (!source)
        source = static_cast<eventDataModel *>(sourceModel());
    return source->eventFromIndex(mapToSource(index));
}

void EventDataProxyModel::setEventAtIndex(const QModelIndex & indexToChange, const Event & event)
{
    if (!source)
        source = static_cast<eventDataModel *>(sourceModel());
    source->setEventAtIndex(mapToSource(indexToChange),event);
}

void EventDataProxyModel::addEvent(const Event eventToAdd)
{
    if (!source)
        source = static_cast<eventDataModel *>(sourceModel());
    source->addEvent(eventToAdd);
}

void EventDataProxyModel::removeEvent(int indexToDelete)
{
    if (!source)
        source = static_cast<eventDataModel *>(sourceModel());
    int otherRow = mapToSource(index(indexToDelete,0)).row();
    source->removeEvent(otherRow);
}
