#ifndef BOOSTFASTSTROKESMODIFIER_H
#define BOOSTFASTSTROKESMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class BoostFastStrokesModifier : public StimSignalModifier
{
public:
    BoostFastStrokesModifier();
    void modify(StimSignalSample &sample) override;
    qreal maxBoostAmount;
    int normalStrokeLength;
};

#endif // BOOSTFASTSTROKESMODIFIER_H
