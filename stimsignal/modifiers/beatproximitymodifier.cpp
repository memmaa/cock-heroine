#include "beatproximitymodifier.h"
#include "mainwindow.h"
#include "globals.h"
#include "../stimsignalgenerator.h"

int getStimFadeInTime()
{
    return DEFAULT_STIM_FADE_IN_LENGTH;
}

int getStimFadeOutTime()
{
    return DEFAULT_STIM_FADE_OUT_LENGTH;
}

BeatProximityModifier::BeatProximityModifier()
{

}

void BeatProximityModifier::modify(StimSignalSample &sample)
{
    //how close are we to the last event?
    Event * eventBefore = mainWindow->getLastEventBefore(sample.totalTimestamp());
    qreal volumeBefore = 0;
    if (eventBefore != nullptr)
    {
        qreal spaceBefore = abs(sample.distanceToTimestamp(eventBefore->timestamp));
        if (spaceBefore <= getStimFadeInTime())
        {
            volumeBefore = (getStimFadeInTime() - spaceBefore) / getStimFadeInTime();
        }
    }
    Event * eventAfter = mainWindow->getNextEventAfter(sample.totalTimestamp());
    qreal volumeAfter = 0;
    if (eventAfter != nullptr)
    {
        qreal spaceAfter = abs(sample.distanceToTimestamp(eventAfter->timestamp));
        if (spaceAfter <= getStimFadeOutTime())
        {
            volumeAfter = (getStimFadeOutTime() - spaceAfter) / getStimFadeOutTime();
        }
    }

    qreal volume = qMax(volumeBefore, volumeAfter);

    sample.primaryAmplitude *= volume;
    sample.secondaryAmplitude *= volume;
}
