#include "fadefromcoldmodifier.h"
#include "optionsdialog.h"
#include "stimsignal/stimsignalsample.h"

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
        for (int i = 0; i < sample.numberOfChannels(); ++i)
            sample.setAmplitude(i, sample.getAmplitude(i) * runningTotal);
        ++sampleCounter;
    }
    else
    {
        //remove self to save processing?
    }
}
