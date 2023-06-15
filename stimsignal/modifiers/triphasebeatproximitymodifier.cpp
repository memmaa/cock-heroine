#include "triphasebeatproximitymodifier.h"
#include "mainwindow.h"
#include "globals.h"
#include "../stimsignalgenerator.h"
#include "optionsdialog.h"

int TriphaseBeatProximityModifier::getFadeInTime()
{
    return fadeInTime;
}

int TriphaseBeatProximityModifier::getFadeInAnticipation()
{
    return fadeInAnticipation;
}

int TriphaseBeatProximityModifier::getFadeOutTime()
{
    return fadeOutTime;
}

int TriphaseBeatProximityModifier::getFadeOutDelay()
{
    return fadeOutDelay;
}

TriphaseBeatProximityModifier::TriphaseBeatProximityModifier()
{
    fadeInTime = OptionsDialog::getEstimBeatFadeInTime();
    fadeInAnticipation = OptionsDialog::getEstimBeatFadeInAnticipationTime();
    fadeOutTime = OptionsDialog::getEstimBeatFadeOutTime();
    fadeOutDelay = OptionsDialog::getEstimBeatFadeOutDelay();
}

void TriphaseBeatProximityModifier::modify(StimSignalSample &sample)
{
    //how close are we to the last event?
    Event * eventBefore = mainWindow->getLastEventBefore(sample.totalTimestamp());
    qreal volumeBefore = 0;
    if (eventBefore != nullptr)
    {
        qreal spaceBefore = abs(sample.distanceToTimestamp(eventBefore->timestamp));
        if (spaceBefore < getFadeOutDelay())
            volumeBefore = 1;
        else
        {
            spaceBefore -= getFadeOutDelay();
            if (spaceBefore <= getFadeOutTime())
                volumeBefore = (getFadeOutTime() - spaceBefore) / getFadeOutTime();
        }
    }
    //and how about the next event?
    Event * eventAfter = mainWindow->getNextEventAfter(sample.totalTimestamp());
    qreal volumeAfter = 0;
    if (eventAfter != nullptr)
    {
        qreal spaceAfter = abs(sample.distanceToTimestamp(eventAfter->timestamp));
        if (spaceAfter <= getFadeInAnticipation())
            volumeAfter = 1;
        else
        {
            spaceAfter -= getFadeInAnticipation();
            if (spaceAfter <= getFadeInTime())
                volumeAfter = (getFadeInTime() - spaceAfter) / getFadeInTime();
        }
    }

    qreal volume = qMax(volumeBefore, volumeAfter);

    for (int i = 0; i < sample.numberOfChannels(); ++i)
        sample.setAmplitude(i, sample.getAmplitude(i) * volume);
}
