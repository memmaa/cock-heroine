#include "estimwavfilewriter.h"
#include <QDebug>
#include <QtEndian>
#include "optionsdialog.h"
#include "stimsignalgenerator.h"
#include <QProgressDialog>
#include "mainwindow.h"
#include <QCoreApplication>

EstimWavFileWriter::EstimWavFileWriter(QString filePath, QObject *parent)
    :
        QObject(parent),
        file(filePath),
        cancelRequested(false)
{

}

bool EstimWavFileWriter::writeFile()
{
    bool success = file.open(QIODevice::WriteOnly);
    if (!success)
    {
        qDebug() << "Couldn't open file " << file.fileName() << " for writing!";
        return false;
    }
    QAudioFormat format = OptionsDialog::getEstimAudioFormat();
    int channelCount = format.channelCount();
    int sampleRate = format.sampleRate();
    QByteArray header(44, '\0');
    char * ptr = header.data();
    strcpy(ptr, "RIFF");
    ptr += 4;
    strcpy(ptr, "____");
    ptr += 4;
    strcpy(ptr, "WAVE");
    ptr += 4;
    strcpy(ptr, "fmt "); //or space?
    ptr += 4;
    qToLittleEndian((uint32_t) 16, ptr); // Size of data section above
    ptr += 4;
    qToLittleEndian((uint16_t) 1, ptr); //PCM
    ptr += 2;
    qToLittleEndian((uint16_t) channelCount, ptr); //No. Channels
    ptr += 2;
    qToLittleEndian((uint32_t) sampleRate, ptr);
    ptr += 4;
    uint32_t bytesPerSecond = sampleRate * /* bytes per sample */ 2 * /* channels*/ channelCount;
    qToLittleEndian((uint32_t) bytesPerSecond, ptr);
    ptr += 4;
    qToLittleEndian((uint16_t) (2 * channelCount), ptr); //byes per 'frame'
    ptr += 2;
    qToLittleEndian((uint16_t) 16, ptr); // bits per (mono) sample
    ptr += 2;
    strcpy(ptr, "data");
    ptr += 4;
    strcpy(ptr, "____");
    file.write(header);

//    QDataStream stream(&header, QIODevice::WriteOnly);
//    stream.writeRawData("RIFF", 4); //wav is a sort of riff
//    stream.writeRawData("SIZE", 4); //total file sixe - overwrite later with 32-bit file size
//    stream.writeRawData("WAVE", 4); //it's a wav file
//    stream.writeRawData("fmt ", 4); //fixed string (including null byte)
//    stream << qToLittleEndian((uint32_t) 16); // Size of data section above
//    stream << qToLittleEndian((uint16_t) 1); //PCM
//    stream << qToLittleEndian((uint16_t) 2); //No. Channels
//    stream << qToLittleEndian((uint32_t) OptionsDialog::getEstimSamplingRate());
//    uint32_t bytesPerSecond = OptionsDialog::getEstimSamplingRate() * /* bytes per sample */ 2 * /* channels*/  2;
//    stream << qToLittleEndian((uint32_t) bytesPerSecond);
//    stream << qToLittleEndian((uint16_t) 4); //byes per 'frame'
//    stream << qToLittleEndian((uint16_t) 16); // bits per (mono) sample
//    stream.writeRawData("data", 4); //fixed string
//    stream.writeRawData("SIZE", 4); //data section size - overwrite later with 32-bit number

//    wavFile.write(header);

    StimSignalGenerator * stimSignalGenerator = StimSignalGenerator::createFromPrefs(this);

    stimSignalGenerator->open(QIODevice::ReadOnly);
    mainWindow->prepareProgress(tr("Exporting E-Stim Track..."), tr("Cancel"), 0, mainWindow->totalPlayTime());
    connect(stimSignalGenerator, SIGNAL(progressed(int, int)), mainWindow, SLOT(updateProgress(int, int)));
    connect(mainWindow, SIGNAL(progressCancelRequested()), this, SLOT(requestCancel()));

    int bytesWritten = 0;

    QByteArray buffer(bytesPerSecond, '\0');
    qint64 bytesRead = 0;
    do
    {
        bytesRead = stimSignalGenerator->read(buffer.data(), bytesPerSecond);
        file.write(buffer.data(), bytesRead);
        bytesWritten += bytesRead;
//        qDebug() << "Files seems to be " << file.size();
//        qDebug() << "Bytes we think we've written " << bytesWritten;
        if (cancelRequested) //can be set by calling 'requestCancel' slot
            break;
        //probably needed so that this can be interrupted by an abort request
        QCoreApplication::processEvents();
    }
    //not ideal - this logic relies on the knowledge that the generator will
    //always give you as much data as you ask for until there's no more, and then stop.
    //Consider looking for EOF?
    while (bytesRead == bytesPerSecond);

    if (!cancelRequested)
    {
        disconnect(mainWindow, SIGNAL(progressCancelRequested()), this, SLOT(requestCancel()));
        disconnect(stimSignalGenerator, SIGNAL(progressed(int, int)), mainWindow, SLOT(updateProgress(int, int)));
        mainWindow->disposeOfProgress();
    }

    stimSignalGenerator->close();
    //generator is child of this, so deleted when this is deleted

    //finished writing data - write sizes
    QByteArray dataSize;
    QDataStream dataSizeStream(&dataSize, QIODevice::WriteOnly);
//    int32_t ourDataCalculation = bytesWritten;
//    int32_t fileBasedData = file.size() - 44;
//    if (ourDataCalculation != fileBasedData)
//        qDebug() << "There's a problem with the data size!";

    dataSizeStream << qToLittleEndian((int32_t) bytesWritten);
    file.seek(40); //that's where the data section size is
    file.write(dataSize);

    //total file size is data section plus 44 bytes for the header
//    int32_t ourFileCalculation = bytesWritten + 36;
//    int32_t fileBasedFile = file.size() - 8;
//    if (ourFileCalculation != fileBasedFile)
//        qDebug() << "There's a problem with the file size!";
    bytesWritten += 44;
    bytesWritten -= 8; //the first 8 bytes for 'RIFF####' don't count
    QByteArray fileSize;
    QDataStream fileSizeStream(&fileSize, QIODevice::WriteOnly);
    fileSizeStream << qToLittleEndian((int32_t) bytesWritten);
    file.seek(4); //that's where the file size is
    file.write(dataSize);

    file.close();
    mainWindow->disposeOfProgress();
    if (cancelRequested)
    {
        file.remove();
        return false;
    }

    return true;
}

void EstimWavFileWriter::requestCancel()
{
    cancelRequested = true;
}
