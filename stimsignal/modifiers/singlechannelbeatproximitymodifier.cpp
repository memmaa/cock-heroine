#include "singlechannelbeatproximitymodifier.h"
#include "mainwindow.h"
#include "event.h"
#include "optionsdialog.h"

SingleChannelBeatProximityModifier::SingleChannelBeatProximityModifier()
{

}


void SingleChannelBeatProximityModifier::modify(StereoStimSignalSample &sample)
{
    //probably set these as members
    qreal peakPositionInCycle = OptionsDialog::getEstimLeftChannelPeakPositionInCycle();
    qreal fadeTime = OptionsDialog::getEstimLeftChannelFadeTime();
    bool linkedToFollowingBeat = OptionsDialog::getEstimLeftChannelStrokeStyle() == PREF_ESTIM_ENDS_ON_BEAT_STYLE;

    long beforeTimestamp = 0;
    Event * eventBefore = mainWindow->getLastEventBefore(sample.totalTimestamp());
    if (eventBefore)
        beforeTimestamp = eventBefore->timestamp;
    qreal distanceFromLast = abs(sample.distanceToTimestamp(beforeTimestamp));
    long afterTimestamp = sample.totalTimestamp() + fadeTime;
    Event * eventAfter = mainWindow->getNextEventAfter(sample.totalTimestamp());
    if (eventAfter)
        afterTimestamp = eventAfter->timestamp;

    int lengthOfCurrentStroke = afterTimestamp - beforeTimestamp;
    qreal proportionOfFullStrokeLength = (qreal) lengthOfCurrentStroke / OptionsDialog::getEstimMaxStrokeLength();
    if (proportionOfFullStrokeLength <= 1)
    {
        //in this case, we can assume that the current volume is part of a fairly regular 'cycle'.
        //work out how far through the cycle (beat) we are and what volume we would normally be at that point.
        //job done.
        qreal currentPositionInCycle = distanceFromLast / lengthOfCurrentStroke;
        qreal difference = abs(currentPositionInCycle - peakPositionInCycle);
        //to account for the fact that differences over 0.5 "start to get smaller again"
        //(i.e. the difference between 0.1 and 0.9 (0.8) is really 0.2: 1.1 - 0.9)
        //subtract 1 and take the absolute value if the answer is greater than 0.5
        //There may be a better way
        if (difference > 0.5)
            difference = abs(difference - 1);
        qreal maxReductionWithinNormalCycle = 1 - OptionsDialog::getEstimLeftChannelTroughLevel();
        qreal reduction = 2 * difference * maxReductionWithinNormalCycle;
        qreal multiplier = 1 - reduction;
        sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * multiplier);
    }
    else
    {
        //we're in a longer period of time.
        //on the up side, we know the rate of volume increase/decrease is constant
        //on the down side, we have to care whether we precede or follow the beat.

        qreal followingMultiplier = 0;
        qreal precedingMultiplier = 0;
        if (linkedToFollowingBeat)
        {
            if (eventAfter)
            {
                //the approach to the next beat will happen "as normal, at normal speed"
                qreal peakPosition = afterTimestamp - ((1 - OptionsDialog::getEstimLeftChannelPeakPositionInCycle()) * OptionsDialog::getEstimMaxStrokeLength());
                qreal distanceFromThere = abs(peakPosition - sample.totalTimestamp());
                followingMultiplier = 1 - (distanceFromThere / fadeTime);
            }
            else
            {
                followingMultiplier = 0;
            }
            if (eventBefore)
            {
                //the distance of the peak through the cycle dictates the level that the volume will be
                //at as it comes out the end of the cycle.
                qreal peakiness = abs (1 - (2 * peakPositionInCycle));
                qreal levelAtEndOfLastCycle = OptionsDialog::getEstimLeftChannelTroughLevel() + (peakiness * (1 - OptionsDialog::getEstimLeftChannelTroughLevel()));
                qreal distanceFromThere = abs(beforeTimestamp - sample.totalTimestamp());
                qreal remainingFadeout = fadeTime * levelAtEndOfLastCycle;
                precedingMultiplier = levelAtEndOfLastCycle * (1 - (distanceFromThere / remainingFadeout));
            }
            else
            {
                precedingMultiplier = 0;
            }
        }
        else
        {
            //we're linked to the preceding beat
            if (eventBefore)
            {
                //the tail of the preceding beat will happen "as normal, at normal speed"
                qreal peakPosition = beforeTimestamp + (OptionsDialog::getEstimLeftChannelPeakPositionInCycle() * OptionsDialog::getEstimMaxStrokeLength());
                qreal distanceFromThere = abs(peakPosition - sample.totalTimestamp());
                precedingMultiplier = 1 - (distanceFromThere / fadeTime);
            }
            else
            {
                precedingMultiplier = 0;
            }
            if (eventAfter)
            {
                //the distance of the peak through the cycle dictates the level that the volume will be
                //at as it enters the cycle.
                qreal peakiness = abs (1 - (2 * peakPositionInCycle));
                qreal levelAtBeginningOfNextCycle = OptionsDialog::getEstimLeftChannelTroughLevel() + (peakiness * (1 - OptionsDialog::getEstimLeftChannelTroughLevel()));
                qreal distanceFromThere = abs(afterTimestamp - sample.totalTimestamp());
                qreal remainingFadein = fadeTime * levelAtBeginningOfNextCycle;
                precedingMultiplier = levelAtBeginningOfNextCycle * (1 - (distanceFromThere / remainingFadein));
            }
            else
            {
                followingMultiplier = 0;
            }
        }
        qreal multiplier = std::max(precedingMultiplier, followingMultiplier);
        sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * multiplier);
    }
}
