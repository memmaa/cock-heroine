#ifndef WAYPOINTFOLLOWERMODIFIER_H
#define WAYPOINTFOLLOWERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class WaypointList;
class Waypoint;

class WaypointFollowerModifier : public StimSignalModifier
{
public:
    WaypointFollowerModifier(WaypointList * waypoints, int channel = -1);
    ~WaypointFollowerModifier();
    void modify(StimSignalSample &sample) override;

private:
    WaypointList * waypoints;
    Waypoint * lastWaypoint;
    Waypoint * nextWaypoint;
    int channel;
    qreal calculateMultiplier(qreal lastValue, qreal nextValue, qreal progress);
};

#endif // WAYPOINTFOLLOWERMODIFIER_H
