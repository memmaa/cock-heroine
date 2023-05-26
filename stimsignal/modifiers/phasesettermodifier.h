#ifndef PHASESETTERMODIFIER_H
#define PHASESETTERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class PhaseSetterModifier : public StimSignalModifier
{
public:
    PhaseSetterModifier(int startFrequency, int endFrequency);
    void modify(StereoStimSignalSample &sample) override;

private:
    void setStep(long timestamp);
    qreal phase;
    qreal step;
    int startingFrequency;
    int endingFrequency;
    int updateCounter;
    int frequencyUpdateInterval;
};

#endif // PHASESETTERMODIFIER_H
