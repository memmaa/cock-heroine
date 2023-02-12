#include "channelbalancemodifier.h"
#include "optionsdialog.h"

ChannelBalanceModifier::ChannelBalanceModifier()
{
    int pan = OptionsDialog::getEstimSignalPan();
    panLeft = (pan < 0);
    attenuation = 1 - (0.01 * abs(pan));
}

void ChannelBalanceModifier::modify(StimSignalSample &sample)
{
    if (panLeft)
        sample.secondaryAmplitude *= attenuation;
    else
        sample.primaryAmplitude *= attenuation;
}
