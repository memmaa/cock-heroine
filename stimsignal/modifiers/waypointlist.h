#ifndef WAYPOINTLIST_H
#define WAYPOINTLIST_H

#include <QList>
#include "waypoint.h"

class WaypointList : public QList<Waypoint *>
{
public:
    WaypointList();
    ~WaypointList();

    void plonkOnTheEnd(Waypoint *newValue);
    void insertTroughs(qreal troughLevel);
    void squeezeInBetween(int insertionIndex, Waypoint * newValue);
};

#endif // WAYPOINTLIST_H
