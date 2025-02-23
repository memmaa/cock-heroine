#ifndef SINGLECHANNELBEATPROXIMITYMODIFIER_H
#define SINGLECHANNELBEATPROXIMITYMODIFIER_H

#include "../stimsignalmodifier.h"

#include <QObject>


class SingleChannelBeatProximityModifier : public StimSignalModifier
{
public:
    SingleChannelBeatProximityModifier(bool cycleStartsOnBeat, qreal peakPositionWithinCycle, qreal troughLevel, qreal fadeTime);
    void modify(StimSignalSample &sample) override;

private:
    bool cycleStartsOnBeat;
    qreal peakPositionInCycle;
    qreal troughLevel;
    qreal fadeTime;
    int getFadeTime();
    int maxStrokeLength;
    qreal amountFadedInOneBeat;
};

#endif // SINGLECHANNELBEATPROXIMITYMODIFIER_H
