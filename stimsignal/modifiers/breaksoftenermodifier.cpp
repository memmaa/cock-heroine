#include "breaksoftenermodifier.h"
#include "optionsdialog.h"

BreakSoftenerModifier::BreakSoftenerModifier(int checkThisChannel)
    :
        currentLevel(1),
        channel(checkThisChannel)
{
    maxQuietening = qreal(OptionsDialog::getEstimCompressorStrength()) / 100;
    long samplesInQuieteningPeriod = OptionsDialog::getEstimSamplingRate() * OptionsDialog::getEstimCompressorBiteTime() + 0.5;
    long samplesInLoudeningPeriod = OptionsDialog::getEstimSamplingRate() * OptionsDialog::getEstimCompressorRelease() + 0.5;
    quieteningPerSilentSample = maxQuietening / samplesInQuieteningPeriod;
    loudeningPerActiveSample = maxQuietening / samplesInLoudeningPeriod;

    if (channel == -1)
    {
        if (OptionsDialog::getEstimChannelCount() == 1)
            channel = 0;
        else
        {
            if (OptionsDialog::getEstimLeftChannelFadeTime() > OptionsDialog::getEstimRightChannelFadeTime())
                channel = 1;
            else
                channel = 0;
        }
    }
}

void BreakSoftenerModifier::modify(StimSignalSample &sample)
{
    if (sample.getAmplitude(channel) == 0)
    {
        if (!atMaxQuietening())
            currentLevel -= quieteningPerSilentSample;
    }
    else
    {
        if (!atMaxVolume())
        {
            currentLevel += loudeningPerActiveSample;
            for (int i = 0; i < sample.numberOfChannels(); ++i)
                sample.setAmplitude(i, sample.getAmplitude(i) * currentLevel);
        }
    }
}

bool BreakSoftenerModifier::atMaxQuietening()
{
    return currentLevel <= maxQuietening;
}

bool BreakSoftenerModifier::atMaxVolume()
{
    return currentLevel >= 1;
}
