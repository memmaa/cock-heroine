#include "stimsignalgenerator.h"
#include "stimsignalmodifier.h"
#include "stereostimsignalsample.h"
#include "qmath.h"
#include <QAudioFormat>
#include <QtEndian>
#include <QDebug>
#include "globals.h"
#include "mainwindow.h"
#include "multithreadedsamplepipelineprocessor.h"
#include "optionsdialog.h"
#include "stimsignal/singlechannelsignalgenerator.h"
#include "stimsignal/dualchannelsignalgenerator.h"
#include "separatelnrsignalgenerator.h"
#include "stimsignal/triphasesignalgenerator.h"
#include "modifiers/waypointlist.h"
#include "modifiers/waypoint.h"

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

    setGenerateFrom(0);
}

StimSignalGenerator::~StimSignalGenerator()
{
    sampleProcessor->deleteLater();
    for (auto modifier : modifiers)
    {
        modifier->deleteLater();
    }
}

qint64 StimSignalGenerator::generate(char *data, qint64 maxlen)
{
    if (modifiers.isEmpty())
    {
        setModifiers();
        sampleProcessor = new MultithreadedSamplePipelineProcessor(&modifiers, this);
    }

    qint64 bytesGenerated = 0;
    const int bytesPerChannel = audioFormat.sampleSize() / 8;
    const int channelCount = audioFormat.channelCount();
    const int bytesPerSample = channelCount * bytesPerChannel;

    int samplesToGenerate = maxlen / bytesPerSample;
    char *ptr = reinterpret_cast<char *>(data);

    long stopAt = getStopTimestamp();
    if (generateFromTimestamp > stopAt)
    {
//        qDebug() << "Not generating any more data because we're trying to generate from " << generateFromTimestamp;
        samplesToGenerate = 0;
    }

    QList<StimSignalSample *> sampleVector;
    for (int i = 0; i < samplesToGenerate; ++i)
    {
        qreal fractionalSecond = (sampleCounter * 1.0f) / (float) audioFormat.sampleRate();
        StimSignalSample * sample = createSample(generateFromTimestamp, fractionalSecond * 1000);
        qreal totalTimestamp = sample->totalTimestamp();
//        if (i)
//        {
//            qreal diff = abs(totalTimestamp - sampleVector.last()->totalTimestamp());
//            if (diff < 0.123 || diff > 0.127)
//                qDebug() << "WEIRD: difference from last: " << diff;
//        }
//        else
//            qDebug() << "started generating at: " << totalTimestamp;
//        if (i == samplesToGenerate - 1)
//            qDebug() << "finished generating at: " << totalTimestamp;
        if (totalTimestamp > stopAt)
            break;
        sampleVector.append(sample);
        ++sampleCounter;
        if (sampleCounter >= audioFormat.sampleRate())
        {
            setGenerateFrom(generateFromTimestamp + 1000); // because that's how many milliseconds we've allocated.
//            qDebug() << "generate from: " << generateFromTimestamp;
        }
    }
    if (!sampleVector.isEmpty())
    {
        sampleProcessor->processAll(&sampleVector);
    }

    for (int i = 0; i < sampleVector.length(); ++i)
    {
        for (int thisChannel = 0; thisChannel < channelCount; ++thisChannel)
        {
            qToLittleEndian<qint16>(sampleVector[i]->pcm(thisChannel), ptr);
            ptr += bytesPerChannel;
        }
        bytesGenerated += bytesPerSample;
        Q_ASSERT(ptr <= (data + maxlen));
        delete sampleVector[i]; //we're done with it now - do not access again
    }
    //if (bytesGenerated)
        //emit readyRead();
    emit progressed(generateFromTimestamp, stopAt);
    return bytesGenerated;
}

bool StimSignalGenerator::open(QIODevice::OpenMode mode)
{
    return QIODevice::open(mode);
}

void StimSignalGenerator::close()
{
    return QIODevice::close();
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
}

void StimSignalGenerator::setPlayFrom(long timestamp)
{
    setGenerateFrom(timestamp);
}

StimSignalGenerator *StimSignalGenerator::createFromPrefs(QObject *parent)
{
    QAudioFormat format = OptionsDialog::getEstimAudioFormat();
    switch (OptionsDialog::getEstimSignalType())
    {
    case MONO:
        return new SingleChannelSignalGenerator(format, parent);
    case STEREO:
        return new DualChannelSignalGenerator(format, parent);
    case SEPARATE_L_AND_R:
        return new SeparateLnRSignalGenerator(format, parent);
    case TRIPHASE:
        return new TriphaseSignalGenerator(format, parent);
    default:
        return new SingleChannelSignalGenerator(format, parent);
    }
}

WaypointList * StimSignalGenerator::createWaypointList(bool waypointsComeOnOrBeforeBeat, qreal peakPositionInCycle, qreal troughLevel, QVector<Event> eventsToUse)
{
    WaypointList * list = new WaypointList();
    int maxStrokeLength = OptionsDialog::getEstimMaxStrokeLength();
    qreal minimumBaseAmount = 0.75;
    qreal maxBoostAmount = 0.01 * OptionsDialog::getEstimBoostShortStrokes();
    for (int i = 0; i < eventsToUse.length(); ++i)
    {
        if (waypointsComeOnOrBeforeBeat)
        {
            long start = 0;
            long end = eventsToUse[i].timestamp;
            //deal with corner case: first beat
            if (i)
            {
                start = eventsToUse[i-1].timestamp;
            }
            int length = std::min(int(end - start), maxStrokeLength);
            start = end - length; //we already set start, but this handles first beat or long beat
            qreal peak = start + (peakPositionInCycle * length);
            int otherLength = maxStrokeLength;
            if (i < (eventsToUse.length() - 1))
            {
                otherLength = std::min((int) (eventsToUse[i+1].timestamp - end), maxStrokeLength);
            }
            int valueForBoostCalculation = std::max(length, otherLength);
            qreal boostAmount = ((qreal) (maxStrokeLength - valueForBoostCalculation) / maxStrokeLength) * maxBoostAmount;
            qreal baseFraction = (qreal) eventsToUse[i].value / eventsToUse[i].maxPossibleValue();
            qreal baseAmount = minimumBaseAmount + ((1 - minimumBaseAmount) * baseFraction);
            list->plonkOnTheEnd(new Waypoint(peak, baseAmount + boostAmount));
        }
        else
            //waypoints come on or after the the beat
        {
            long start = eventsToUse[i].timestamp;
            long end = start + maxStrokeLength;
            //deal with corner case: last beat
            if (i < (eventsToUse.length() - 1))
            {
                end = eventsToUse[i+1].timestamp;
            }
            int length = std::min(int(end - start), maxStrokeLength);
            qreal peak = start + (peakPositionInCycle * length);
            int otherLength = maxStrokeLength;
            if (i)
            {
                otherLength = std::min(int(start - eventsToUse[i-1].timestamp), maxStrokeLength);
            }
            int valueForBoostCalculation = std::max(length, otherLength);
            qreal boostAmount = ((qreal) (maxStrokeLength - valueForBoostCalculation) / maxStrokeLength) * maxBoostAmount;
            qreal baseFraction = (qreal) eventsToUse[i].value / eventsToUse[i].maxPossibleValue();
            qreal baseAmount = minimumBaseAmount + ((1 - minimumBaseAmount) * baseFraction);
            list->plonkOnTheEnd(new Waypoint(peak, baseAmount + boostAmount));
        }
    }
    list->insertTroughs(troughLevel);
    return list;
}
