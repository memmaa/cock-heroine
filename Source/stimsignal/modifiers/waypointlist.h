#ifndef WAYPOINTLIST_H
#define WAYPOINTLIST_H

class Waypoint;

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
