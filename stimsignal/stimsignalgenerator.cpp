#include "stimsignalgenerator.h"
#include "stimsignalsample.h"
#include "qmath.h"
#include "modifiers/triphasemodifier.h"
#include "modifiers/beatproximitymodifier.h"
#include <QAudioFormat>
#include <QtEndian>

int StimSignalGenerator::getMaxTriphaseStrokeLength()
{
    return DEFAULT_MAX_TRIPHASE_STROKE_LENGTH;
}

StimSignalGenerator::StimSignalGenerator(int frequency, QAudioFormat audioFormat, QObject *parent)
    :
      QIODevice(parent),
      frequency(frequency),
      audioFormat(audioFormat)
{
    modifiers.append(new TriphaseModifier());
    modifiers.append(new BeatProximityModifier());
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

QByteArray StimSignalGenerator::generate(long startTimestamp)
{
    Q_ASSERT(isPCMS16LE(audioFormat));

    const int bytesPerChannel = audioFormat.sampleSize() / 8;
    const int bytesPerSample = audioFormat.channelCount() * bytesPerChannel;

    int samplesToGenerate = audioFormat.sampleRate();
    qreal primaryPhaseIncreasePerSample = (qreal) frequency / audioFormat.sampleRate();
    QByteArray output(samplesToGenerate * bytesPerSample, '\0');
    unsigned char *ptr = reinterpret_cast<unsigned char *>(output.data());
    for (int i = 0; i < samplesToGenerate; ++i)
    {
        qreal fractionalSecond = (i * 1.0f) / (float) audioFormat.sampleRate();
        StimSignalSample sample(startTimestamp, fractionalSecond * 1000);
        sample.primaryPhase = i * primaryPhaseIncreasePerSample;
        // put in sample: qreal primarySampleValue = qSin(2 * M_PI * primarySamplePhase);
        for (auto modifier : modifiers)
        {
            modifier->modify(sample);
        }
        qToLittleEndian<qint16>(sample.primaryPcm(), ptr);
        ptr += bytesPerChannel;
        qToLittleEndian<qint16>(sample.secondaryPcm(), ptr);
        ptr += bytesPerChannel;
    }
    generateFromTimestamp += 1000; // because that's how many milliseconds we've done.
    emit readyRead();
    return output;
}

bool StimSignalGenerator::open(QIODevice::OpenMode mode)
{
    ensureQueueFull();
    assignBuffer();
    return QIODevice::open(mode) && buffer.open(mode);
}

qint64 StimSignalGenerator::readData(char *data, qint64 maxlen)
{
    if (!buffer.isOpen() || buffer.size() == 0)
    {
        assignBuffer();
        if (!buffer.isOpen())
        {
            buffer.open(QIODevice::ReadOnly);
        }
    }
    qint64 bytesRead = buffer.read(data, maxlen);
    if (bytesRead < maxlen)
    {
        //we've read all the data from this buffer
        buffer.close();
        auto empty = bufferQueue.takeFirst();
        ensureQueueFull();
        delete empty;
        assignBuffer();
        buffer.open(QIODevice::ReadOnly);
    }
    return bytesRead;
}

qint64 StimSignalGenerator::writeData(const char *, qint64 )
{
    return -1; //cannot write to this device
}

bool StimSignalGenerator::seek(qint64 pos)
{
    //XXX: not sure about this. Should I really be doing this?
    generateFromTimestamp = pos;
    return QIODevice::seek(pos);
}

void StimSignalGenerator::ensureQueueFull()
{
    while (bufferQueue.size() < 2)
    {
        QByteArray * newChunk = new QByteArray(generate(generateFromTimestamp));
        bufferQueue.append(newChunk);
    }
}

void StimSignalGenerator::assignBuffer()
{
    bool wasOpen = isOpen();
    if (wasOpen)
        close();
    buffer.setData(*bufferQueue.first());
    if (wasOpen)
        open(QIODevice::ReadOnly); //it shouldn't have been open any other way
}

void StimSignalGenerator::emptyQueue()
{
    while (!bufferQueue.isEmpty())
        delete bufferQueue.takeFirst();
}
