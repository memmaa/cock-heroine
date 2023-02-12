#ifndef PHASEINVERTERMODIFIER_H
#define PHASEINVERTERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class PhaseInverterModifier : public StimSignalModifier
{
public:
    PhaseInverterModifier();
    void modify(StimSignalSample &sample) override;
};

#endif // PHASEINVERTERMODIFIER_H
