#ifndef PROGRESSINCREASEMODIFIER_H
#define PROGRESSINCREASEMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class ProgressIncreaseModifier : public StimSignalModifier
{
public:
    ProgressIncreaseModifier();
    void modify(StereoStimSignalSample &sample) override;
    qreal initialValue = 1;
    qreal totalIncrease = 0;
};

#endif // PROGRESSINCREASEMODIFIER_H
