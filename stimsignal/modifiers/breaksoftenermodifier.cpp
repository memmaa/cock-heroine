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

void BreakSoftenerModifier::modify(StereoStimSignalSample &sample)
{
    if (sample.getPrimaryAmplitude() == 0)
    {
        if (!atMaxQuietening())
            currentLevel -= quieteningPerSilentSample;
    }
    else
    {
        if (!atMaxVolume())
        {
            currentLevel += loudeningPerActiveSample;
            sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * currentLevel);
            sample.setSecondaryAmplitude(sample.getSecondaryAmplitude() * currentLevel);
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
