#include "progressincreasemodifier.h"
#include "optionsdialog.h"
#include "mainwindow.h"
#include "globals.h"
#include "stimsignal/stimsignalsample.h"

ProgressIncreaseModifier::ProgressIncreaseModifier()
{
    int increasePercent = OptionsDialog::getEstimTotalSignalGrowth();
    //leave space for 'boosting' fast strokes
    qreal endingVolume = (qreal) 100 / (100 + OptionsDialog::getEstimBoostShortStrokes());
    initialValue = (endingVolume * 100) / (100 + increasePercent);
    totalIncrease = endingVolume - initialValue;
}

void ProgressIncreaseModifier::modify(StimSignalSample &sample)
{
    qreal progress = mainWindow->progressThroughGame(sample.totalTimestamp());
    qreal scaleValue = initialValue + (progress * totalIncrease);
    for (int i = 0; i < sample.numberOfChannels(); ++i)
        sample.setAmplitude(i, sample.getAmplitude(i) * scaleValue);
}
