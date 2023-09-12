#include "triphasemodifier.h"
#include "mainwindow.h"
#include "globals.h"
#include "optionsdialog.h"
#include "beatinterval.h"

TriphaseModifier::TriphaseModifier()
{
    strokeLength = OptionsDialog::getEstimMaxStrokeLength();
    if (OptionsDialog::getEstimTriphaseStrokeStyle() == PREF_ESTIM_UP_DOWN_BEAT_STROKE_STYLE)
        style = UP_DOWN_BEAT;
    else
        style = DOWN_BEAT_UP;
}

int TriphaseModifier::getMaxTriphaseStrokeLength()
{
    return strokeLength;
}

void TriphaseModifier::modify(StimSignalSample &sample)
{
    qreal phaseDifference = 0;

    long nextEventTimestamp = 0;
    long previousEventTimestamp = 0;
    bool previousEventFound = false;
    bool nextEventFound = false;
    Event * nextEvent = mainWindow->getNextEventAfter(sample.totalTimestamp());
    if (nextEvent != nullptr)
    {
        nextEventFound = true;
        nextEventTimestamp = nextEvent->timestamp;
    }
    Event * previousEvent = mainWindow->getLastEventBefore(sample.totalTimestamp());
    if (previousEvent != nullptr)
    {
        previousEventFound = true;
        previousEventTimestamp = previousEvent->timestamp;
    }
    switch (style)
    {
    case UP_DOWN_BEAT:
        // mimic the behaviour of the handy and funscript output.
        // pros: it matches the stroker and funscript actions
        // cons: not sure if this is the best way to make stimulation
        //       feel 'on the beat' as all action is before the beat. /////////////////////////
        // we primarily care about when the next beat is.
        if (nextEventFound && sample.distanceToTimestamp(nextEventTimestamp) <= getMaxTriphaseStrokeLength())
        {
            //now we're part way through a 'stroke', so calculate how far through we are.
            qlonglong startStrokeAt = 0;
            if ( !previousEventFound || std::abs(sample.distanceToTimestamp(previousEventTimestamp)) > getMaxTriphaseStrokeLength())
            {
                startStrokeAt = nextEventTimestamp - getMaxTriphaseStrokeLength();
                if (startStrokeAt < 0)
                {
                    startStrokeAt = 0; //can't start a stroke before the beginning
                }
            }
            else
            {
                startStrokeAt = previousEventTimestamp;
            }
            int strokeLength = nextEventTimestamp - startStrokeAt;
            //distance to timestamp in the past is negative, so invert it
            phaseDifference = -sample.distanceToTimestamp(startStrokeAt) / strokeLength;
        }
        break; //case
    case DOWN_BEAT_UP:
        //this style defaults to 'up' rather than 'down' when not stroking
        qreal distance = 1; //i.e. nowhere near the beat (will get converted to 0.5, which is max distance from any integer)
        int strokeLength = getMaxTriphaseStrokeLength();
        if (nextEventFound && previousEventFound)
            strokeLength = nextEventTimestamp - previousEventTimestamp;
        int halfOfStrokeLength = strokeLength / 2;

        if (nextEventFound && sample.distanceToTimestamp(nextEventTimestamp) <= halfOfStrokeLength)
        {
            //now we're on a 'down stroke', calculate how far from the beat we are
            distance = sample.distanceToTimestamp(nextEventTimestamp) / halfOfStrokeLength;
            //distance is between 0 and 1
        }
        else if (previousEventFound && std::abs(sample.distanceToTimestamp(previousEventTimestamp)) <= halfOfStrokeLength)
        {
            //we're on an 'up stroke', calculate distance from beat, distance will be negative
            distance = sample.distanceToTimestamp(previousEventTimestamp) / halfOfStrokeLength;
            //distance is between 0 and -1
        }
        //constrain to between -0.5 and +0.5: phase 'wraps around' below 0 as well as above 1
        phaseDifference = 0.5 * distance;
        break; //case
    }
    sample.setPhase(1, sample.getPrimaryPhase() + phaseDifference);
}
