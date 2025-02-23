#include "stimsignalsource.h"
#include "optionsdialog.h"
#include "stimsignalgenerator.h"
#include "stimsignalfile.h"
#include "mainwindow.h"
#include <QDir>
#include <QFileInfo>

StimSignalSource::StimSignalSource()
{

}

QIODevice *StimSignalSource::createFromPrefs(QObject *parent)
{
    QString userSpecifiedFilename = OptionsDialog::getEstimSourceFilename();

    QString pregeneratedFilename = calculatePregeneratedStimFilename();

    switch (OptionsDialog::getEstimSourceMode())
    {
    case FROM_FILE:
        return new QFile(userSpecifiedFilename, parent);
    case PREGENERATED:
        return new QFile(pregeneratedFilename, parent);
    case ON_THE_FLY:
        return StimSignalGenerator::createFromPrefs(parent);
    }
    return nullptr;
}

QString StimSignalSource::calculatePregeneratedStimFilename()
{
    QString basePath = OptionsDialog::getLastOpenedLocation();
    if (basePath.isEmpty())
        basePath = QDir::toNativeSeparators(QDir::homePath());

    QString fullPath = loadedScript;
    if (fullPath.isEmpty())
        fullPath = loadedVideo;
    if (fullPath.isEmpty())
        fullPath = QDir::cleanPath(basePath + QDir::separator() + "Untitled.wav");
    QFileInfo pathInfo(fullPath);
    QString baseName = pathInfo.completeBaseName(); //baseName or completeBaseName?
    QString generatedFilename = QString("%1-%2-%3.wav")
        .arg(baseName)
        .arg(OptionsDialog::getEstimSettingsFilenameSuffix())
        .arg(currentEventsHash(),0,16,QLatin1Char('0'));
    return QDir::cleanPath(pathInfo.path() + QDir::separator() + generatedFilename);
}

#define A 54059 /* a prime */
#define B 76963 /* another prime */
#define C 86969 /* yet another prime */
#define FIRSTH 37 /* also prime */
unsigned int StimSignalSource::currentEventsHash()
{
   unsigned h = FIRSTH;
   for (auto event : events)
   {
       h = (h * A) ^ (event.toHash() * B);
   }
   return h; // or return h % C;
}
