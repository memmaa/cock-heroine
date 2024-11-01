#include "wavefileexporter.h"
#include <QDebug>

#define INITIAL_WAV_READ_SIZE 512 //how many bytes to read in and try to find all relevant header info in input wav file.
#define SIZE_OF_SIMPLE_WAV_HEADER 44

const char * const WaveFileExporter::riffTag = "RIFF";
const char * const WaveFileExporter::waveTag = "WAVEfmt ";
const char * const WaveFileExporter::dataTag = "data";

WaveFileExporter::WaveFileExporter(QString inputFilename, QString outputFilename, QObject *parent) :
    QObject(parent),
    inputFile(inputFilename),
    outputFile(outputFilename),
    inputData(&inputFile),
    outputData(&outputFile),
    inputHeaderProcessed(false)
{
    if (! inputFile.open(QIODevice::ReadOnly))
    qDebug() << "Could not open file for reading:" << inputFilename;

    if (! outputFile.open(QIODevice::WriteOnly))
    qDebug() << "Could not open file for writing:" << outputFilename;

    inputHeaderData = inputFile.read(INITIAL_WAV_READ_SIZE);

    inputData.setByteOrder(QDataStream::LittleEndian);
    outputData.setByteOrder(QDataStream::LittleEndian);

    inputDataChunkStartPos = inputHeaderData.indexOf(dataTag);
}

bool WaveFileExporter::inputFileIsValid()
{
    qint16 dataType = intFromBytes(inputHeaderData.mid(20,2));

    /*if (inputHeaderData.size() != SIZE_OF_SIMPLE_WAV_HEADER)
    {
        qDebug() << "Empty or corrupted file. inputHeaderData.size is" << inputHeaderData.size();
        return false;
    }*/
    if (inputHeaderData.startsWith(riffTag) == false)
    {
        qDebug() << "File does not start with RIFF tag";
        return false;
    }
    if (inputHeaderData.indexOf(waveTag) != 8)
    {
        qDebug() << "WAVE tag not at index 8";
        return false;
    }
    if (inputHeaderData.indexOf(dataTag) == -1)
    {
        qDebug() << "could not find data tag";
        return false;
    }
    if (dataType != 1)
    {
        qDebug() << "dataType is" << dataType;
        return false;
    }

    return true;
}

void WaveFileExporter::processInputHeader()
{
    if (! inputFileIsValid())
        return;

    inputFile.seek(22);
    inputData >> numChannels;

    inputFile.seek(24);
    inputData >> sampleRate;


    inputFile.seek(28);
    inputData >> bitRate;

    inputFile.seek(32);
    inputData >> bytesPerFrame;

    inputFile.seek(34);
    inputData >> sampleResolution;

    inputFile.seek(inputDataChunkStartPos + 4);
    inputData >> inputAudioSize;

    inputHeaderProcessed = true;
}

void WaveFileExporter::writeOutputHeader()
{
    if (! inputHeaderProcessed)
        processInputHeader();

    outputData.writeRawData(riffTag,4); //Marks the file as a riff file. Characters are each 1 byte long.
    outputData << (qint32)0; //Size of the overall file - 8 bytes, in bytes (32-bit integer). Fill this in after creation.

    outputData.writeRawData(waveTag,8);

    outputData << (qint32)16; //total size in bytes of following /*6*/ bits of tag info:

    /*1*/outputData << (qint16)1; //PCM format (not compressed)
    /*2*/outputData << (qint16)numChannels; //number of channels: 1 = mono, 2 = stereo
    /*3*/outputData << (qint32)sampleRate; //sample rate (Hz)
    /*4*/outputData << (qint32)(sampleRate * sampleResolution * numChannels) / 8; //bitrate (bytes per second)
    /*5*/outputData << (qint16)((sampleResolution * numChannels) / 8); //bytes per frame (frame = one sample per chanel)
    /*6*/outputData << (qint16)sampleResolution; //bits per sample

    outputData.writeRawData(dataTag,4);
    sizeOfOutputDataSectionInBytes = 0;
    outputData << (qint32)sizeOfOutputDataSectionInBytes; //Size of the data section in bytes (32-bit integer). Fill this in after creation.
}

void WaveFileExporter::writeSoundAt(unsigned int timestamp)
{
    if (! inputHeaderProcessed)
        processInputHeader();

    qDebug() << "WritingSoundAt" << timestamp;
    unsigned long beatStartSample = ((unsigned long)timestamp * (unsigned long)bitRate) / (unsigned long)1000;
    //round up to the next start of a frame
    while ( (beatStartSample % bytesPerFrame) != 0)
        ++beatStartSample;

    while (beatStartSample > outputFile.size() - SIZE_OF_SIMPLE_WAV_HEADER)
        outputData << (qint8)0;
    outputFile.seek(beatStartSample + SIZE_OF_SIMPLE_WAV_HEADER);
    inputFile.seek(inputDataChunkStartPos + 8);
    //using alloca here to avoid memory management because MSVC doesn't support variable length arrays
    char* frame = (char *) alloca(bytesPerFrame);
    unsigned int bytesWritten = 0;
    while (bytesWritten < inputAudioSize)
    {
        inputFile.read(frame,bytesPerFrame);
        outputFile.write(frame,bytesPerFrame);
        bytesWritten += bytesPerFrame;
    }
}

void WaveFileExporter::closeFile()
{
    inputFile.close();

    //finalise file:
    //write data subchunk size:
    outputFile.seek(40);
    outputData << (qint32) (outputFile.size() - SIZE_OF_SIMPLE_WAV_HEADER);
    //write wave chunk size:
    outputFile.seek(4);
    outputData << (qint32) (outputFile.size() - 8);

    outputFile.close();
}

void WaveFileExporter::printDebugInformation()
{
    if (! inputHeaderProcessed)
        processInputHeader();

    qDebug() << "This sound has " << numChannels << " channels of audio.";
    qDebug() << "This sound has " << sampleRate << " samples per second.";
    qDebug() << "This sound has " << bitRate << " bytes per second.";
    qDebug() << "This sound has " << bytesPerFrame << " bytes per frame.";
    qDebug() << "This sound has " << sampleResolution << " bits per sample.";
    qDebug() << "The input contains " << inputAudioSize << " bytes of audio data.";
}

qint32 WaveFileExporter::intFromBytes(QByteArray bytes)
{
    qint32 returnValue = 0;
    int i = bytes.size();
    while (i >= 0)
    {
        returnValue += (bytes[i] << (i*8));
        --i;
    }
    return returnValue;
}
