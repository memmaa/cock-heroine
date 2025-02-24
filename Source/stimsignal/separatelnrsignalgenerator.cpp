#include "separatelnrsignalgenerator.h"
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

SeparateLnRSignalGenerator::SeparateLnRSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{

}

void SeparateLnRSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimLeftChannelStartingFrequency(), OptionsDialog::getEstimLeftChannelEndingFrequency(), LEFT_CHANNEL));
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimRightChannelStartingFrequency(), OptionsDialog::getEstimRightChannelEndingFrequency(), RIGHT_CHANNEL));

    QList<unsigned char> upstrokes{e_eventType::EVENT_STROKE_UP};
    QVector<Event> upEvents = filteredEvents(upstrokes);
    WaypointList * leftVolumeWaypoints = createWaypointList(false,
                                                            0,
                                                            OptionsDialog::getEstimLeftChannelTroughLevel(),
                                                            upEvents);
    modifiers.append(new WaypointFollowerModifier(leftVolumeWaypoints, LEFT_CHANNEL));

    QList<unsigned char> downstrokes{e_eventType::EVENT_STROKE_DOWN};
    QVector<Event> downEvents = filteredEvents(downstrokes);
    WaypointList * rightVolumeWaypoints = createWaypointList(false,
                                                             0,
                                                             OptionsDialog::getEstimRightChannelTroughLevel(),
                                                             downEvents);
    modifiers.append(new WaypointFollowerModifier(rightVolumeWaypoints, RIGHT_CHANNEL));

    modifiers.append(new ProgressIncreaseModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long SeparateLnRSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + std::max(OptionsDialog::getEstimLeftChannelFadeTime(), OptionsDialog::getEstimRightChannelFadeTime());
}

StimSignalSample *SeparateLnRSignalGenerator::createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp)
{
    return new StereoStimSignalSample(wholeTimestamp, fractionalTimestamp);
}
