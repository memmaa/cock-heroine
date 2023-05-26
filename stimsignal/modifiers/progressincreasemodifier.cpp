#include "progressincreasemodifier.h"
#include "optionsdialog.h"
#include "mainwindow.h"
#include "globals.h"

ProgressIncreaseModifier::ProgressIncreaseModifier()
{
    int increasePercent = OptionsDialog::getEstimTotalSignalGrowth();
    //leave space for 'boosting' fast strokes
    qreal endingVolume = (qreal) 100 / (100 + OptionsDialog::getEstimBoostShortStrokes());
    initialValue = (endingVolume * 100) / (100 + increasePercent);
    totalIncrease = endingVolume - initialValue;
}

void ProgressIncreaseModifier::modify(StereoStimSignalSample &sample)
{
    qreal progress = mainWindow->progressThroughGame(sample.totalTimestamp());
    qreal scaleValue = initialValue + (progress * totalIncrease);
    sample.setPrimaryAmplitude(sample.getPrimaryAmplitude() * scaleValue);
    sample.setSecondaryAmplitude(sample.getSecondaryAmplitude() * scaleValue);
}
