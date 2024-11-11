#ifndef WAVEFILEEXPORTER_H
#define WAVEFILEEXPORTER_H

#include <QFile>
#include <QDataStream>
#include <QByteArray>

class WaveFileExporter : public QObject
{
    Q_OBJECT
public:
    explicit WaveFileExporter(QString inputWavFilename, QString outputWavFilename, QObject *parent = 0);
    bool inputFileIsValid();
    void writeOutputHeader();
    void writeSoundAt(unsigned int timestamp);
    void printDebugInformation();
    void closeFile();

private:
    QFile inputFile;
    QFile outputFile;
    QByteArray inputHeaderData;

    QDataStream inputData;
    QDataStream outputData;

    quint16 inputDataChunkStartPos;

    void processInputHeader();
    bool inputHeaderProcessed;

    qint16 numChannels;
    qint32 sampleRate;
    qint32 bitRate;
    qint16 bytesPerFrame;
    qint16 sampleResolution;
    quint32 inputAudioSize;

    quint32 sizeOfOutputDataSectionInBytes;

    qint32 intFromBytes(QByteArray bytes);

    static const char * const riffTag;
    static const char * const waveTag;
    static const char * const dataTag;

signals:

public slots:

};

#endif // WAVEFILEEXPORTER_H
