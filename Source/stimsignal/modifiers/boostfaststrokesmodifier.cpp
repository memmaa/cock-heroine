#include "boostfaststrokesmodifier.h"
#include "optionsdialog.h"
#include "globals.h"
#include "mainwindow.h"
#include "stimsignal/stimsignalsample.h"

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
    // XXX
    // Note that this will cause distortion if not combined with something which reduces the overall volume below 100%
    // At the time of writing, this role is done by the 'ProgressIncreaseModifier'
    for (int i = 0; i < sample.numberOfChannels(); ++i)
        sample.setAmplitude(i, sample.getAmplitude(i) * (1 + boostAmount));
}
