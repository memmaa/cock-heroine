#ifndef BEATTIMESTAMP_H
#define BEATTIMESTAMP_H

#include "event.h"

class BeatTimestamp
{
public:
    BeatTimestamp();
    BeatTimestamp(int index, Event event);

    int originalRowIndex;
    Event eventData;
    bool isDeleted;
};

#endif // BEATTIMESTAMP_H
