#ifndef CHANNELBALANCEMODIFIER_H
#define CHANNELBALANCEMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class ChannelBalanceModifier : public StimSignalModifier
{
public:
    ChannelBalanceModifier();
    void modify(StereoStimSignalSample &sample) override;

private:
    bool panLeft;
    float attenuation;

};

#endif // CHANNELBALANCEMODIFIER_H
