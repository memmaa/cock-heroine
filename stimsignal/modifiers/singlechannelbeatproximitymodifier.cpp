#include "singlechannelbeatproximitymodifier.h"
#include "mainwindow.h"
#include "event.h"
#include "optionsdialog.h"

SingleChannelBeatProximityModifier::SingleChannelBeatProximityModifier(bool cycleStartsOnBeat, qreal peakPositionWithinCycle, qreal troughLevel, qreal fadeTime)
    :
        cycleStartsOnBeat(cycleStartsOnBeat),
        peakPositionInCycle(peakPositionWithinCycle),
        troughLevel(troughLevel),
        fadeTime(fadeTime),
        maxStrokeLength(OptionsDialog::getEstimMaxStrokeLength()),
        amountFadedInOneBeat(OptionsDialog::getEstimMaxStrokeLength() / fadeTime)
{

}


void SingleChannelBeatProximityModifier::modify(StimSignalSample &sample)
{
    //probably set these as members
    bool linkedToFollowingBeat = !cycleStartsOnBeat;

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
    qreal proportionOfFullStrokeLength = (qreal) lengthOfCurrentStroke / maxStrokeLength;
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
        qreal maxReductionWithinNormalCycle = 1 - troughLevel;
        qreal reduction = 2 * difference * maxReductionWithinNormalCycle;
        qreal multiplier = 1 - reduction;
        if (multiplier > 1 || multiplier < 0)
            qDebug() << "multiplier out of range: " << multiplier;

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
                qreal peakPosition = afterTimestamp - ((1 - peakPositionInCycle) * maxStrokeLength);
                qreal distanceFromThere = abs(peakPosition - sample.totalTimestamp());
                if (distanceFromThere < fadeTime)
                    followingMultiplier = 1 - (distanceFromThere / fadeTime);
                else
                    followingMultiplier = 0;
            }
            else
            {
                followingMultiplier = 0;
            }
            if (eventBefore)
            {
                //the distance of the peak through the cycle dictates the level that the volume will be
                //at as it comes out the end of the cycle.
                qreal levelAtEndOfLastCycle = 1 - ((1 - peakPositionInCycle) * amountFadedInOneBeat);
                if (levelAtEndOfLastCycle < 0)
                    precedingMultiplier = 0;
                else
                {
                    qreal distanceFromThere = abs(beforeTimestamp - sample.totalTimestamp());
                    qreal remainingFadein = fadeTime * levelAtEndOfLastCycle;
                    if (distanceFromThere > remainingFadein)
                        followingMultiplier = 0;
                    else
                        followingMultiplier = levelAtEndOfLastCycle * (1 - (distanceFromThere / remainingFadein));
                }
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
                qreal peakPosition = beforeTimestamp + (peakPositionInCycle * maxStrokeLength);
                qreal distanceFromThere = abs(peakPosition - sample.totalTimestamp());
                if (distanceFromThere < fadeTime)
                    precedingMultiplier = 1 - (distanceFromThere / fadeTime);
                else
                    precedingMultiplier = 0;
            }
            else
            {
                precedingMultiplier = 0;
            }
            if (eventAfter)
            {
                //the distance of the peak through the cycle dictates the level that the volume will be
                //at as it enters the cycle.
                qreal levelAtBeginningOfNextCycle = 1 - (peakPositionInCycle * amountFadedInOneBeat);
                if (levelAtBeginningOfNextCycle < 0)
                    followingMultiplier = 0;
                else
                {
                    qreal distanceFromThere = abs(afterTimestamp - sample.totalTimestamp());
                    qreal remainingFadein = fadeTime * levelAtBeginningOfNextCycle;
                    if (distanceFromThere > remainingFadein)
                        followingMultiplier = 0;
                    else
                        followingMultiplier = levelAtBeginningOfNextCycle * (1 - (distanceFromThere / remainingFadein));
                }
            }
            else
            {
                followingMultiplier = 0;
            }
        }
        if (precedingMultiplier > 1 || precedingMultiplier < 0)
            qDebug() << "precedingMultiplier out of range: " << precedingMultiplier;
        if (followingMultiplier > 1 || followingMultiplier < 0)
            qDebug() << "followingMultiplier out of range: " << followingMultiplier;
        qreal multiplier = std::max(precedingMultiplier, followingMultiplier);
        if (multiplier > 1 || multiplier < 0)
            qDebug() << "multiplier out of range: " << multiplier;
        sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * multiplier);
    }
}
