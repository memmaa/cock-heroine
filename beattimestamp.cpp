#include "beattimestamp.h"

BeatTimestamp::BeatTimestamp()
    :
      originalRowIndex(-1),
      isDeleted(false)
{
}

BeatTimestamp::BeatTimestamp(int index, Event event, bool deleted)
    :
      originalRowIndex(index),
      eventData(event),
      isDeleted(deleted)
{
}
