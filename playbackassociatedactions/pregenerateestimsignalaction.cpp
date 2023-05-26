#include "pregenerateestimsignalaction.h"
#include "mainwindow.h"
#include <QFileInfo>

PregenerateEstimSignalAction::PregenerateEstimSignalAction()
    :
        PlaybackAssociatedAction(tr("Pre-generating E-stim Signal")),
        cancelRequested(false)
{

}

PregenerateEstimSignalAction::~PregenerateEstimSignalAction()
{
    writer->deleteLater();
}

void PregenerateEstimSignalAction::kickOff()
{
    emit started(name);
    QString filename = calculateFilename();
    if (filename.isEmpty())
    {
        emit failed(name, QString(tr("Could not calculate a temporary filename to store the pre-generated estim signal. Please save the current project data, or load a video.")));
        return;
    }
    else if (QFileInfo(filename).exists())
    {
        //hooray - we can reuse this file, so nothing to do here.
        emit completed(name);
        return;
    }
    //OK, let's do this...

    writer = new EstimWavFileWriter(filename);
    if (writer->writeFile() || cancelRequested)
    {
        emit completed(name);
        return;
    }
    emit failed(name, "Something went wrong while writing the e-stim file. Sorry! :-(");
}

void PregenerateEstimSignalAction::abort()
{
    cancelRequested = true;
    writer->requestCancel();
}

QString PregenerateEstimSignalAction::calculateFilename()
{
    if (!mainWindow->getLoadedScript().isEmpty())
    {
        QFileInfo fileInfo(mainWindow->getLoadedScript());
//        QString textToRemove = fileInfo.completeSuffix();
        QString textToRemove = fileInfo.suffix();
        QString newBaseName = fileInfo.absoluteFilePath();
        newBaseName.chop(textToRemove.length());
        return newBaseName + "wav";
    }
    else if (!mainWindow->getLoadedVideo().isEmpty())
    {
        QFileInfo fileInfo(mainWindow->getLoadedVideo());
        QString textToRemove = fileInfo.completeSuffix();
//        QString textToRemove = fileInfo.suffix();
        QString newBaseName = fileInfo.absoluteFilePath();
        newBaseName.chop(textToRemove.length());
        return newBaseName + "wav";
    }
    return "";
}

