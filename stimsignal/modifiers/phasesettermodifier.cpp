#include "phasesettermodifier.h"
#include "optionsdialog.h"
#include "mainwindow.h"

PhaseSetterModifier::PhaseSetterModifier(int startFrequency, int endFrequency)
    :
      phase(0),
      step(0),
      startingFrequency(startFrequency),
      endingFrequency(endFrequency),
      updateCounter(OptionsDialog::getEstimSamplingRate()),
      frequencyUpdateInterval(OptionsDialog::getEstimSamplingRate())
{

}

void PhaseSetterModifier::modify(StereoStimSignalSample &sample)
{
    if (updateCounter >= frequencyUpdateInterval)
        setStep(sample.totalTimestamp());
    sample.setPrimaryPhase(phase);
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
