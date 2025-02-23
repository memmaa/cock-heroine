#ifndef FADEFROMCOLDMODIFIER_H
#define FADEFROMCOLDMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class FadeFromColdModifier : public StimSignalModifier
{
public:
    FadeFromColdModifier();
    void modify(StimSignalSample &sample) override;

private:
    long totalSamples;
    long sampleCounter = 0;
    qreal runningTotal = 0;
    qreal increment = 0;
};

#endif // FADEFROMCOLDMODIFIER_H
