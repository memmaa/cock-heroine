#ifndef SINGLECHANNELBEATPROXIMITYMODIFIER_H
#define SINGLECHANNELBEATPROXIMITYMODIFIER_H

#include "../stimsignalmodifier.h"

#include <QObject>


class SingleChannelBeatProximityModifier : public StimSignalModifier
{
public:
    SingleChannelBeatProximityModifier();
    void modify(StereoStimSignalSample &sample) override;

private:
    int fadeTime;
    int getFadeTime();
};

#endif // SINGLECHANNELBEATPROXIMITYMODIFIER_H
