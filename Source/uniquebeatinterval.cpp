#include "uniquebeatinterval.h"
#include "config.h"
#include "helperfunctions.h"
#include "globals.h"

UniqueBeatInterval::UniqueBeatInterval()
    :
      length(0),
      count(0)
{
    //nothing here
}

UniqueBeatInterval::UniqueBeatInterval(float len)
    :
      length(len),
      count(1)
{
    //nothing here
}

UniqueBeatInterval::UniqueBeatInterval(float len, short cnt)
    :
      length(len),
      count(cnt)
{
    //nothing here
}

UniqueBeatInterval::~UniqueBeatInterval()
{
}

int UniqueBeatInterval::registerInterval(float interval)
{
    if (matchesThisInterval(interval))
    {
        double workingValue = (count * length) + interval;
        ++count;
        length = workingValue / count;
    }
    else
    {
        length = interval;
        count = 1;
    }
    return length;
}
