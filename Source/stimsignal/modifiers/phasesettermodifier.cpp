#include "phasesettermodifier.h"
#include "optionsdialog.h"
#include "mainwindow.h"
#include "stimsignal/stimsignalsample.h"

PhaseSetterModifier::PhaseSetterModifier(int startFrequency, int endFrequency, int channel)
    :
      phase(0),
      step(0),
      startingFrequency(startFrequency),
      endingFrequency(endFrequency),
      updateCounter(OptionsDialog::getEstimSamplingRate()),
      frequencyUpdateInterval(OptionsDialog::getEstimSamplingRate()),
      channel(channel)
{

}

void PhaseSetterModifier::modify(StimSignalSample &sample)
{
    if (updateCounter >= frequencyUpdateInterval)
        setStep(sample.totalTimestamp());
    if (channel == -1)
    {
        for (int i = 0; i < sample.numberOfChannels(); ++i)
            sample.setPhase(i, phase);
    }
    else
    {
        sample.setPhase(channel, phase);
    }
    phase += step;
}

void PhaseSetterModifier::setStep(long timestamp)
{
    qreal endPortion = mainWindow->progressThroughGame(timestamp);
    qreal beginningPortion = 1 - endPortion;
    int currentFrequency = (beginningPortion * startingFrequency) + (endPortion * endingFrequency);
    step = (qreal) currentFrequency / OptionsDialog::getEstimSamplingRate();
    phase -= (int) phase;
    updateCounter = 0;
}
