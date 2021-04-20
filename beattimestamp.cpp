#include "beattimestamp.h"

BeatTimestamp::BeatTimestamp()
    :
      originalRowIndex(-1),
      isDeleted(false)
{
}

BeatTimestamp::BeatTimestamp(int index, Event event)
    :
      originalRowIndex(index),
      eventData(event),
      isDeleted(false)
{
}
