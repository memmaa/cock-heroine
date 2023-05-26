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
#include "stimsignal/triphasesignalgenerator.h"
#include "stimsignal/singlechannelsignalgenerator.h"

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

qint64 StimSignalGenerator::generate(char *data, qint64 maxlen)
{
    if (modifiers.isEmpty())
        setModifiers();

    qint64 bytesGenerated = 0;
    const int bytesPerChannel = audioFormat.sampleSize() / 8;
    const int bytesPerSample = audioFormat.channelCount() * bytesPerChannel;

    int samplesToGenerate = maxlen / bytesPerSample;
    char *ptr = reinterpret_cast<char *>(data);

    long stopAt = getStopTimestamp();
    if (generateFromTimestamp > stopAt)
    {
//        qDebug() << "Not generating any more data because we're trying to generate from " << generateFromTimestamp;
        samplesToGenerate = 0;
    }

    QList<StereoStimSignalSample *> sampleVector;
    for (int i = 0; i < samplesToGenerate; ++i)
    {
        qreal fractionalSecond = (sampleCounter * 1.0f) / (float) audioFormat.sampleRate();
        if (fractionalSecond > stopAt)
            break;
        StereoStimSignalSample * sample = new StereoStimSignalSample(generateFromTimestamp, fractionalSecond * 1000);
        sampleVector.append(sample);
    }
    if (!sampleVector.isEmpty())
    {
        MultithreadedSamplePipelineProcessor processor(&sampleVector, &modifiers, this);
        processor.processAll();
    }

    for (int i = 0; i < sampleVector.length(); ++i)
    {
        qToLittleEndian<qint16>(sampleVector[i]->primaryPcm(), ptr);
        ptr += bytesPerChannel;
        qToLittleEndian<qint16>(sampleVector[i]->secondaryPcm(), ptr);
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
    emit progressed(generateFromTimestamp, stopAt);
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
}

StimSignalGenerator *StimSignalGenerator::createFromPrefs(QObject *parent)
{
    QAudioFormat format = OptionsDialog::getEstimAudioFormat();
    switch (OptionsDialog::getEstimSignalType())
    {
    case MONO:
        return new SingleChannelSignalGenerator(format, parent);
    default:
        return new TriphaseSignalGenerator(format, parent);
    }
}
