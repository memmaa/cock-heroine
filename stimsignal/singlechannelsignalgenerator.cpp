#include "singlechannelsignalgenerator.h"
#include "optionsdialog.h"
#include "modifiers/phasesettermodifier.h"
#include "modifiers/singlechannelbeatproximitymodifier.h"
#include "modifiers/progressincreasemodifier.h"
#include "modifiers/boostfaststrokesmodifier.h"
#include "modifiers/fadefromcoldmodifier.h"
#include "modifiers/breaksoftenermodifier.h"
#include "modifiers/channelbalancemodifier.h"
#include "mainwindow.h"

SingleChannelSignalGenerator::SingleChannelSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      StimSignalGenerator(audioFormat, parent)
{
}

void SingleChannelSignalGenerator::setModifiers()
{
    modifiers.append(new PhaseSetterModifier(OptionsDialog::getEstimLeftChannelStartingFrequency(), OptionsDialog::getEstimLeftChannelEndingFrequency()));
    modifiers.append(new SingleChannelBeatProximityModifier());
    modifiers.append(new ProgressIncreaseModifier());
//    modifiers.append(new BoostFastStrokesModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());
}

long SingleChannelSignalGenerator::getStopTimestamp()
{
    return mainWindow->totalPlayTime() + OptionsDialog::getEstimLeftChannelFadeTime();
}
