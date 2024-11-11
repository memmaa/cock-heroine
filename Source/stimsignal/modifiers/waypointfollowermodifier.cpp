#include "waypointfollowermodifier.h"
#include "waypointlist.h"
#include <QtMath>
#include "stimsignal/stimsignalsample.h"
#include "waypoint.h"

WaypointFollowerModifier::WaypointFollowerModifier(WaypointList *waypoints, int channel)
    :
        waypoints(waypoints),
        nextWaypoint(waypoints->first()),
        channel(channel)
{
}

WaypointFollowerModifier::~WaypointFollowerModifier()
{
    delete waypoints;
}

void WaypointFollowerModifier::modify(StimSignalSample &sample)
{
    if (sample.totalTimestamp() < waypoints->first()->timestamp || sample.totalTimestamp() >= waypoints->last()->timestamp)
    {
        if (channel == -1)
        {
            for (int i = 0; i < sample.numberOfChannels(); ++i)
                sample.setAmplitude(i, 0);
        }
        else
        {
            sample.setAmplitude(channel, 0);
        }
        return;
    }
    if (sample.totalTimestamp() >= nextWaypoint->timestamp)
    {
        lastWaypoint = nextWaypoint;
        nextWaypoint = lastWaypoint->next;
    }
    qreal distanceToLast = sample.totalTimestamp() - lastWaypoint->timestamp;
    qreal distanceToNext = nextWaypoint->timestamp - sample.totalTimestamp();
    qreal nextWeight = distanceToLast / (distanceToLast + distanceToNext);
    qreal totalMultiplier = calculateMultiplier(lastWaypoint->multiplier, nextWaypoint->multiplier, nextWeight);
    if (channel == -1)
    {
        for (int i = 0; i < sample.numberOfChannels(); ++i)
            sample.setAmplitude(i, totalMultiplier);
    }
    else
    {
        sample.setAmplitude(channel, totalMultiplier);
    }
}

qreal WaypointFollowerModifier::calculateMultiplier(qreal lastValue, qreal nextValue, qreal progress)
{
//    qreal zeroToOne = (progress * nextValue) + ((1 - progress) * lastValue);
    //simple implementation:
//    return zeroToOne;

    //SINple implementation
    qreal difference = nextValue - lastValue;
    return lastValue + difference * (0.5 + (0.5 * qCos(M_PI + M_PI * progress)));
}
