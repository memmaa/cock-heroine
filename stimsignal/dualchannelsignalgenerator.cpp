#include "dualchannelsignalgenerator.h"
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
#include "stereostimsignalsample.h"

#define LEFT_CHANNEL 0
#define RIGHT_CHANNEL 1

DualChannelSignalGenerator::DualChannelSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{

}

void DualChannelSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimLeftChannelStartingFrequency(), OptionsDialog::getEstimLeftChannelEndingFrequency(), LEFT_CHANNEL));
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimRightChannelStartingFrequency(), OptionsDialog::getEstimRightChannelEndingFrequency(), RIGHT_CHANNEL));
    WaypointList * leftVolumeWaypoints = createWaypointList(OptionsDialog::getEstimLeftChannelStrokeStyle() == PREF_ESTIM_ENDS_ON_BEAT_STYLE,
                                                            OptionsDialog::getEstimLeftChannelPeakPositionInCycle(),
                                                            OptionsDialog::getEstimLeftChannelTroughLevel());
    modifiers.append(new WaypointFollowerModifier(leftVolumeWaypoints, LEFT_CHANNEL));
    WaypointList * rightVolumeWaypoints = createWaypointList(OptionsDialog::getEstimRightChannelStrokeStyle() == PREF_ESTIM_ENDS_ON_BEAT_STYLE,
                                                             OptionsDialog::getEstimRightChannelPeakPositionInCycle(),
                                                             OptionsDialog::getEstimRightChannelTroughLevel());
    modifiers.append(new WaypointFollowerModifier(rightVolumeWaypoints, RIGHT_CHANNEL));
    modifiers.append(new ProgressIncreaseModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long DualChannelSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + std::max(OptionsDialog::getEstimLeftChannelFadeTime(), OptionsDialog::getEstimRightChannelFadeTime());
}

StimSignalSample *DualChannelSignalGenerator::createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp)
{
    return new StereoStimSignalSample(wholeTimestamp, fractionalTimestamp);
}
