#include "stimsignalfile.h"
#include <QtEndian>
#include <QDebug>

StimSignalFile::StimSignalFile(QString filename, QObject *parent)
    :
        QFile(filename, parent)
{
}

bool StimSignalFile::open(QIODevice::OpenMode mode)
{
    bool success = QIODevice::open(mode);
    if (success && fileName().endsWith(".wav"))
    {
        char sampleRateData[4];
        seek(24); //that's where the sample rate is stored
        read(sampleRateData, 4);
        sampleRate = qFromLittleEndian<qint32>(sampleRateData);

        char bytesPerSecData[4];
        seek(28); //bytes per second
        read(bytesPerSecData, 4);
        bytesPerSecond = qFromLittleEndian<qint32>(bytesPerSecData);

        char frameSizeData[4];
        seek(32); //that's where the size of a 'frame' is stored (mono sample size (2 bytes) * num channels (1 or 2))
        read(frameSizeData, 4);
        frameSize = qFromLittleEndian<qint32>(frameSizeData);

        seek(WAV_HEADER_SIZE); //beginning of data section
    }
    return success;
}

void StimSignalFile::close()
{
    return QIODevice::close();
}

void StimSignalFile::setPlayFrom(long timestamp)
{
    //calculate the frame number we need. there are sampleRate frames per second
    long frameNumber = timestamp * sampleRate;
    //but timestamp is in miliseconds, so we've gone 1000 times too far...
    frameNumber /= 1000;
    //this is the number of the frame we need
    //the frame will be this many bytes into the file:
    long seekLocation = frameNumber * frameSize;
    //plus space for the header
    seekLocation += WAV_HEADER_SIZE;

    seek(seekLocation);
}

QAudioFormat StimSignalFile::deriveFormatFromFile(QFile * file)
{
    if (!file->fileName().endsWith(".wav"))
    {
        qDebug() << "This isn't a wav file. We don't support that yet (consider supporting me?)";
        return QAudioFormat();
    }
    bool fileWasOpen = file->isOpen();
    if (!fileWasOpen && !file->open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open file to fild audio format.";
        return QAudioFormat();
    }

    QAudioFormat format;
    format.setByteOrder(QAudioFormat::LittleEndian); //wav is little-endian
    format.setSampleType(QAudioFormat::SignedInt); //or at least that's the way the generator creates them
    format.setCodec("audio/pcm");

    char sampleRateData[4];
    file->seek(SAMPLE_RATE_LOCATION);
    file->read(sampleRateData, 4);
    int sampleRate = qFromLittleEndian<qint32>(sampleRateData);
    format.setSampleRate(sampleRate);

    char channelCountData[2];
    file->seek(CHANNEL_COUNT_LOCATION);
    file->read(channelCountData, 2);
    qint16 channelCount = qFromLittleEndian<qint16>(channelCountData);
    format.setChannelCount(channelCount);

    char bitsPerSampleData[2];
    file->seek(BITS_PER_SAMPLE_LOCATION);
    file->read(bitsPerSampleData, 2);
    qint16 bitsPerSample = qFromLittleEndian<qint16>(bitsPerSampleData);
    format.setSampleSize(bitsPerSample);

    if (!fileWasOpen)
        file->close();
    return format;
}

void StimSignalFile::seekToTimestamp(QFile * file, long timestamp, int sampleRate, int frameSize)
{
    //calculate the frame number we need. there are sampleRate frames per second
    long frameNumber = timestamp * sampleRate;
    //but timestamp is in miliseconds, so we've gone 1000 times too far...
    frameNumber /= 1000;
    //this is the number of the frame we need
    //the frame will be this many bytes into the file:
    long seekLocation = frameNumber * frameSize;
    //plus space for the header
    seekLocation += WAV_HEADER_SIZE;

    file->seek(seekLocation);
}


//qint64 StimSignalFile::readData(char *data, qint64 maxlen)
//{
//    return file.readData(data, maxlen);
//}
