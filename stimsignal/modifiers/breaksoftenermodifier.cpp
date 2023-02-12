#include "breaksoftenermodifier.h"
#include "optionsdialog.h"

BreakSoftenerModifier::BreakSoftenerModifier()
{
    maxQuietening = qreal(OptionsDialog::getEstimCompressorStrength()) / 100;
    long samplesInQuieteningPeriod = OptionsDialog::getEstimSamplingRate() * OptionsDialog::getEstimCompressorBiteTime() + 0.5;
    long samplesInLoudeningPeriod = OptionsDialog::getEstimSamplingRate() * OptionsDialog::getEstimCompressorRelease() + 0.5;
    quieteningPerSilentSample = maxQuietening / samplesInQuieteningPeriod;
    loudeningPerActiveSample = maxQuietening / samplesInLoudeningPeriod;
}

void BreakSoftenerModifier::modify(StimSignalSample &sample)
{
    if (sample.primaryAmplitude == 0)
    {
        if (!atMaxQuietening())
            currentLevel -= quieteningPerSilentSample;
    }
    else
    {
        if (!atMaxVolume())
        {
            currentLevel += loudeningPerActiveSample;
            sample.primaryAmplitude *= currentLevel;
            sample.secondaryAmplitude *= currentLevel;
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
