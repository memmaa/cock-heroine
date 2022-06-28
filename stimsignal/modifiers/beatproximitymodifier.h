#ifndef BEATPROXIMITYMODIFIER_H
#define BEATPROXIMITYMODIFIER_H

#include "../stimsignalmodifier.h"

#include <QObject>

#define DEFAULT_STIM_FADE_IN_LENGTH 4000
#define DEFAULT_STIM_FADE_OUT_LENGTH 4000

class BeatProximityModifier : public StimSignalModifier
{
public:
    BeatProximityModifier();
    void modify(StimSignalSample &sample) override;
};

#endif // BEATPROXIMITYMODIFIER_H
