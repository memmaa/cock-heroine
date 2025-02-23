#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <QObject>
#include "event.h"

class EventDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit EventDispatcher(QObject *parent = nullptr);
    virtual void dispatch(Event event) = 0;

signals:

};

#endif // EVENTDISPATCHER_H
