#include "boostfaststrokesmodifier.h"
#include "optionsdialog.h"
#include "globals.h"
#include "mainwindow.h"

BoostFastStrokesModifier::BoostFastStrokesModifier()
{
    maxBoostAmount = 0.01 * OptionsDialog::getEstimBoostShortStrokes();
    normalStrokeLength = OptionsDialog::getEstimMaxStrokeLength();
}

void BoostFastStrokesModifier::modify(StimSignalSample &sample)
{
    qreal timestamp = sample.totalTimestamp();
    Event * before = mainWindow->getLastEventBefore(timestamp);
    Event * after = mainWindow->getNextEventAfter(timestamp);
    if (before == nullptr || after == nullptr)
        //can't (or shouldn't) help with this
        return;
    long length = after->timestamp - before->timestamp;
    if (length <= 0 || length > normalStrokeLength)
        //can't (or shouldn't) help with this
        return;
    qreal boostAmount = ((qreal) (normalStrokeLength - length) / normalStrokeLength) * maxBoostAmount;
    sample.primaryAmplitude *= (1 + boostAmount);
    sample.secondaryAmplitude *= (1 + boostAmount);
}
