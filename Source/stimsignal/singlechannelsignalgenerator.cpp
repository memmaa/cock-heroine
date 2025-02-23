#include "singlechannelsignalgenerator.h"
#include "optionsdialog.h"
#include "modifiers/phasesettermodifier.h"
//#include "modifiers/singlechannelbeatproximitymodifier.h"
#include "modifiers/waypointfollowermodifier.h"
#include "modifiers/progressincreasemodifier.h"
//#include "modifiers/boostfaststrokesmodifier.h"
#include "modifiers/fadefromcoldmodifier.h"
#include "modifiers/breaksoftenermodifier.h"
#include "modifiers/channelbalancemodifier.h"
#include "mainwindow.h"
#include "monostimsignalsample.h"

SingleChannelSignalGenerator::SingleChannelSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{

}

void SingleChannelSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimLeftChannelStartingFrequency(), OptionsDialog::getEstimLeftChannelEndingFrequency()));
    WaypointList * volumeWaypoints = createWaypointList(OptionsDialog::getEstimLeftChannelStrokeStyle() == PREF_ESTIM_ENDS_ON_BEAT_STYLE,
                                                        OptionsDialog::getEstimLeftChannelPeakPositionInCycle(),
                                                        OptionsDialog::getEstimLeftChannelTroughLevel());
    modifiers.append(new WaypointFollowerModifier(volumeWaypoints));
    modifiers.append(new ProgressIncreaseModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long SingleChannelSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + OptionsDialog::getEstimLeftChannelFadeTime();
}

StimSignalSample *SingleChannelSignalGenerator::createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp)
{
    return new MonoStimSignalSample(wholeTimestamp, fractionalTimestamp);
}
