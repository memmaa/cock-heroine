#include "fadefromcoldmodifier.h"
#include "optionsdialog.h"

FadeFromColdModifier::FadeFromColdModifier()
{
    int sampleRate = OptionsDialog::getEstimSamplingRate();
    totalSamples = (OptionsDialog::getEstimStartPlaybackFadeInTime() * sampleRate) / 1000;
    increment = 1.0 / totalSamples;
}

void FadeFromColdModifier::modify(StimSignalSample &sample)
{
    if (sampleCounter < totalSamples)
    {
        runningTotal = sampleCounter * increment;
        sample.primaryAmplitude *= runningTotal;
        sample.secondaryAmplitude *= runningTotal;
        ++sampleCounter;
    }
    else
    {
        //remove self to save processing?
    }
}