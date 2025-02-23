#include "beatinterval.h"
#include "beattimestamp.h"
#include "globals.h"

BeatInterval::BeatInterval()
    :
      index(-1)
{
    //nothing here
}

BeatInterval::BeatInterval(int index)
    :
      index(index)
{
    //nothing here
}

BeatInterval::~BeatInterval()
{
}

int BeatInterval::startsAtTimestamp()
{
    return beatTimestamps.at(index).eventData.timestamp;
}

int BeatInterval::endsAtTimestamp()
{
    return beatTimestamps.at(index + 1).eventData.timestamp;
}

float BeatInterval::getLength() const
{
    return (float) getIntLength();
}

int BeatInterval::getIntLength() const
{
    return beatTimestamps.at(index + 1).eventData.timestamp - beatTimestamps.at(index).eventData.timestamp;
}
