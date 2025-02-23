#include "waypoint.h"

Waypoint::Waypoint(long timestamp, qreal multiplier)
    :
        timestamp(timestamp),
        multiplier(multiplier),
        next(nullptr),
        previous(nullptr)
{

}
