#include "triphasemodifier.h"
#include "mainwindow.h"
#include "globals.h"

TriphaseModifier::TriphaseModifier()
{

}

int TriphaseModifier::getMaxTriphaseStrokeLength()
{
    //TODO: Configurable
    return 500;
}

void TriphaseModifier::modify(StimSignalSample &sample)
{
    // mimic the behaviour of the handy and funscript output.
    // pros: it matches the stroker and funscript actions
    // cons: not sure if this is the best way to make stimulation
    //       feel 'on the beat' as all action is before the beat. /////////////////////////
    qreal phaseDifference = 0;
    // we primarily care about when the next beat is.
    Event * nextEvent = mainWindow->getNextEventAfter(sample.totalTimestamp());
    if (nextEvent != nullptr && sample.distanceToTimestamp(nextEvent->timestamp) <= getMaxTriphaseStrokeLength())
    {
        //now we're part way through a 'stroke', so calculate how far through we are.
        qlonglong startStrokeAt = 0;
        Event * previousEvent = mainWindow->getLastEventBefore(sample.totalTimestamp());
        if ( previousEvent == nullptr || previousEvent->timestamp < sample.totalTimestamp() - getMaxTriphaseStrokeLength())
        {
            startStrokeAt = nextEvent->timestamp - getMaxTriphaseStrokeLength();
            if (startStrokeAt < 0)
            {
                startStrokeAt = 0; //can't start a stroke before the beginning
            }
        }
        else
        {
            startStrokeAt = previousEvent->timestamp;
        }
        int strokeLength = nextEvent->timestamp - startStrokeAt;
        //distance to timestamp in the past is negative, so invert it
        phaseDifference = -sample.distanceToTimestamp(startStrokeAt) / strokeLength;
    }

    sample.secondaryPhase = sample.primaryPhase + phaseDifference;
}
