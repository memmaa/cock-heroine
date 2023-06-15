#ifndef STIMSIGNALFILE_H
#define STIMSIGNALFILE_H

#include "stimsignalsource.h"
#include <QFile>
#include <QAudioFormat>

class StimSignalFile : public QFile, public StimSignalSource
{
public:
    StimSignalFile(QString filename, QObject * parent);
    bool open(QIODevice::OpenMode mode) override;
    void close() override;
    void setPlayFrom(long timestamp) override;
    static QAudioFormat deriveFormatFromFile(QFile *);
    static void seekToTimestamp(QFile *file, long timestamp, int sampleRate, int frameSize);
//    qint64 readData(char *data, qint64 maxlen) override;
//    qint64 writeData(const char *data, qint64 len) override;

private:
        qint32 sampleRate;
        qint32 bytesPerSecond;
        qint32 frameSize;

        static const int SAMPLE_RATE_LOCATION = 24; //4 bytes
        static const int CHANNEL_COUNT_LOCATION = 22; //2 bytes
//        static const int BYTES_PER_FRAME_LOCATION = 32; //2 bytes
        static const int BITS_PER_SAMPLE_LOCATION = 34; //2 bytes
        static const int WAV_HEADER_SIZE = 44;
};

#endif // STIMSIGNALFILE_H
