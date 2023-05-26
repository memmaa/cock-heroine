#ifndef TRIPHASEMODIFIER_H
#define TRIPHASEMODIFIER_H

#include "../stimsignalmodifier.h"

enum StrokeStyle {
    UP_DOWN_BEAT,
    DOWN_BEAT_UP
};

class TriphaseModifier : public StimSignalModifier
{
public:
    TriphaseModifier();
    void modify(StereoStimSignalSample &sample) override;
private:
    int strokeLength;
    int getMaxTriphaseStrokeLength();
    StrokeStyle style;
};

#endif // TRIPHASEMODIFIER_H
