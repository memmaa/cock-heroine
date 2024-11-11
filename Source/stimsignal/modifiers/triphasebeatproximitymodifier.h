#ifndef BEATPROXIMITYMODIFIER_H
#define BEATPROXIMITYMODIFIER_H

#include "../stimsignalmodifier.h"

#define DEFAULT_STIM_FADE_IN_LENGTH 4000
#define DEFAULT_STIM_FADE_OUT_LENGTH 4000

class TriphaseBeatProximityModifier : public StimSignalModifier
{
public:
    TriphaseBeatProximityModifier();
    void modify(StimSignalSample &sample) override;
private:
    int fadeInTime;
    int fadeInAnticipation;
    int fadeOutTime;
    int fadeOutDelay;
    int getFadeInTime();
    int getFadeInAnticipation();
    int getFadeOutTime();
    int getFadeOutDelay();
};

#endif // BEATPROXIMITYMODIFIER_H
