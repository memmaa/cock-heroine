#ifndef TRIPHASEMODIFIER_H
#define TRIPHASEMODIFIER_H

#include "../stimsignalmodifier.h"

class TriphaseModifier : public StimSignalModifier
{
public:
    TriphaseModifier();
    int getMaxTriphaseStrokeLength();
    void modify(StimSignalSample &sample) override;
};

#endif // TRIPHASEMODIFIER_H
