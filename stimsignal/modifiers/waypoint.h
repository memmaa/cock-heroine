#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <Qt>

class Waypoint
{
public:
    Waypoint(long timestamp, qreal multiplier);

    long timestamp;
    qreal multiplier;
    Waypoint * next;
    Waypoint * previous;
};

#endif // WAYPOINT_H
