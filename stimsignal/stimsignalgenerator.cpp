#include "stimsignalgenerator.h"
#include "stimsignalsample.h"
#include "qmath.h"
#include "modifiers/triphasemodifier.h"
#include "modifiers/beatproximitymodifier.h"
#include "modifiers/progressincreasemodifier.h"
#include "modifiers/boostfaststrokesmodifier.h"
#include "modifiers/phaseinvertermodifier.h"
#include "modifiers/fadefromcoldmodifier.h"
#include "modifiers/breaksoftenermodifier.h"
#include "modifiers/channelbalancemodifier.h"
#include <QAudioFormat>
#include <QtEndian>
#include <QDebug>
#include "globals.h"
#include "mainwindow.h"
#include "optionsdialog.h"


int StimSignalGenerator::getCurrentFrequency()
{
    return currentFrequency;
}

bool isPCM(const QAudioFormat &format)
{
    return (format.codec() == "audio/pcm");
}

bool isPCMS16LE(const QAudioFormat &format)
{
    return isPCM(format) &&
           format.sampleType() == QAudioFormat::SignedInt &&
           format.sampleSize() == 16 &&
           format.byteOrder() == QAudioFormat::LittleEndian;
}

StimSignalGenerator::StimSignalGenerator(QAudioFormat audioFormat, QObject *parent)
    :
      QIODevice(parent),
      audioFormat(audioFormat)
{
    Q_ASSERT(isPCMS16LE(audioFormat));

    modifiers.append(new TriphaseModifier());
    modifiers.append(new BeatProximityModifier());
    modifiers.append(new ProgressIncreaseModifier());
    modifiers.append(new BoostFastStrokesModifier());
    if (OptionsDialog::getEstimInvertStrokes())
        modifiers.append(new PhaseInverterModifier());
    modifiers.append(new FadeFromColdModifier());
    modifiers.append(new BreakSoftenerModifier());
    if (OptionsDialog::getEstimSignalPan())
        modifiers.append(new ChannelBalanceModifier());

    startingFrequency = OptionsDialog::getEstimStartingFrequency();
    endingFrequency = OptionsDialog::getEstimEndingFrequency();
    setGenerateFrom(0);
}

qint64 StimSignalGenerator::generate(char *data, qint64 maxlen)
{
    qint64 bytesGenerated = 0;
    const int bytesPerChannel = audioFormat.sampleSize() / 8;
    const int bytesPerSample = audioFormat.channelCount() * bytesPerChannel;

    int samplesToGenerate = maxlen / bytesPerSample;
    qreal primaryPhaseIncreasePerSample = (qreal) getCurrentFrequency() / audioFormat.sampleRate();
    char *ptr = reinterpret_cast<char *>(data);

    long stopAt = mainWindow->totalPlayTime() + OptionsDialog::getEstimBeatFadeOutDelay() + OptionsDialog::getEstimBeatFadeOutTime();
    if (generateFromTimestamp > stopAt)
    {
//        qDebug() << "Not generating any more data because we're trying to generate from " << generateFromTimestamp;
        samplesToGenerate = 0;
    }

    for (int i = 0; i < samplesToGenerate; ++i)
    {
        qreal fractionalSecond = (sampleCounter * 1.0f) / (float) audioFormat.sampleRate();
        if (fractionalSecond > stopAt)
            break;
        StimSignalSample sample(generateFromTimestamp, fractionalSecond * 1000);
        sample.setPrimaryPhase(sampleCounter * primaryPhaseIncreasePerSample);
        for (auto modifier : modifiers)
        {
            modifier->modify(sample);
        }
        qToLittleEndian<qint16>(sample.primaryPcm(), ptr);
        ptr += bytesPerChannel;
        qToLittleEndian<qint16>(sample.secondaryPcm(), ptr);
        ptr += bytesPerChannel;
        bytesGenerated += bytesPerSample;
        Q_ASSERT(ptr <= (data + maxlen));
        ++sampleCounter;
        if (sampleCounter >= audioFormat.sampleRate())
        {
            setGenerateFrom(generateFromTimestamp + 1000); // because that's how many milliseconds we've done.
//            qDebug() << "generate from: " << generateFromTimestamp;
        }
    }
    //if (bytesGenerated)
        //emit readyRead();
    return bytesGenerated;
}

qint64 StimSignalGenerator::readData(char *data, qint64 maxlen)
{
    return generate(data, maxlen);
}

qint64 StimSignalGenerator::writeData(const char *, qint64 )
{
    return -1; //cannot write to this device
}

bool StimSignalGenerator::seek(qint64 )
{
    return false;
}

bool StimSignalGenerator::isSequential() const
{
    return true;
}

void StimSignalGenerator::setGenerateFrom(long from)
{
    generateFromTimestamp = from;
    sampleCounter = 0;
    calculateCurrentFrequency();
}

void StimSignalGenerator::calculateCurrentFrequency()
{
    qreal endPortion = mainWindow->progressThroughGame(generateFromTimestamp);
    qreal beginningPortion = 1 - endPortion;
    currentFrequency = (beginningPortion * startingFrequency) + (endPortion * endingFrequency);
}
