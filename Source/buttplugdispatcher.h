#ifndef BUTTPLUGDISPATCHER_H
#define BUTTPLUGDISPATCHER_H

#include "eventdispatcher.h"

class ButtplugInterface;

class ButtplugDispatcher : public EventDispatcher
{
public:
    ButtplugDispatcher(ButtplugInterface * nterface, QObject * parent = nullptr);
    void dispatch(Event event);

private:
    //!
    //! \brief nterface can't call it 'interface' because that seems to be defined to 'struct' :-o
    //!
    ButtplugInterface * nterface;

    void triggerEventNow(Event event);
};

#endif // BUTTPLUGDISPATCHER_H
