#ifndef BREAKSOFTENERMODIFIER_H
#define BREAKSOFTENERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class BreakSoftenerModifier : public StimSignalModifier
{
public:
    BreakSoftenerModifier();
    void modify(StimSignalSample &sample) override;

private:
    qreal currentLevel = 1;
    qreal maxQuietening;
    qreal quieteningPerSilentSample;
    qreal loudeningPerActiveSample;

    bool atMaxQuietening();
    bool atMaxVolume();
};

#endif // BREAKSOFTENERMODIFIER_H
