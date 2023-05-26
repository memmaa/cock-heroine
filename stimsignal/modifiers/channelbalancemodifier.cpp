#include "channelbalancemodifier.h"
#include "optionsdialog.h"

ChannelBalanceModifier::ChannelBalanceModifier()
{
    int pan = OptionsDialog::getEstimSignalPan();
    panLeft = (pan < 0);
    attenuation = 1 - (0.01 * abs(pan));
}

void ChannelBalanceModifier::modify(StereoStimSignalSample &sample)
{
    if (panLeft)
    {
        sample.setSecondaryAmplitude(sample.getSecondaryAmplitude() * attenuation);
    }
    else
    {
        sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * attenuation);
    }
}
