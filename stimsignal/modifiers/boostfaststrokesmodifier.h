#ifndef BOOSTFASTSTROKESMODIFIER_H
#define BOOSTFASTSTROKESMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

//!
//! \brief The BoostFastStrokesModifier class
//! Note that this will cause distortion if not combined with something which reduces the overall volume below 100%
//! At the time of writing, this role is done by the 'ProgressIncreaseModifier'
//!
class BoostFastStrokesModifier : public StimSignalModifier
{
public:
    BoostFastStrokesModifier();
    void modify(StimSignalSample &sample) override;
    qreal maxBoostAmount;
    int normalStrokeLength;
};

#endif // BOOSTFASTSTROKESMODIFIER_H
