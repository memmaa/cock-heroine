#ifndef BEATTIMESTAMP_H
#define BEATTIMESTAMP_H

#include "event.h"

class BeatTimestamp
{
public:
    BeatTimestamp();
    //!
    //! \brief BeatTimestamp
    //! \param index
    //! \param event
    //! \param deleted almost always false - only true if restoring pre-deleted timestamps from an undo snapshot
    //!
    BeatTimestamp(int index, Event event, bool deleted = false);

    int originalRowIndex;
    Event eventData;
    bool isDeleted;
};

#endif // BEATTIMESTAMP_H
