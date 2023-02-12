#include "globals.h"
#include <QTime>
#include "event.h"
#include "beattimestamp.h"
#include "beatinterval.h"
#include "uniquebeatinterval.h"
#include <QMediaPlayer>
#include <QLCDNumber>
#include "mainwindow.h"
#include "optionsdialog.h"

//For standard english QWERTY keyboard. Change here for QWERTZ, AZERTY, or whatever...
const char * defaultValueShortcuts[2][10] = {{"1","2","3","4","5","6","7","8","9","0"},{"Q","W","E","R","T","Y","U","I","O","P"}};
//QWERTZ:
//const char * defaultValueShortcuts[2][10] = {{"1","2","3","4","5","6","7","8","9","0"},{"Q","W","E","R","T","Z","U","I","O","P"}};
//For AZERTY, I think the numbers need changing to punctuation. Not sure if that causes a problem...

QMediaPlayer * videoPlayer = NULL;
EditorWindow * editor = NULL;
MainWindow * mainWindow = NULL;
QLCDNumber * lcdDisplay; //so seekToTimestamp can update it
QTimer * refreshTimer = NULL;

QString millisToTimecode(unsigned int millis)
{
    int millisInSecond = 1000;
    int millisInMinute = millisInSecond * 60;
    int millisInHour = millisInMinute * 60;
    return QString("%1:%2:%3.%4").arg(millis / millisInHour, 1, 10, QChar('0'))
                                 .arg((millis / millisInMinute) % 60, 2, 10, QChar('0'))
                                 .arg((millis / millisInSecond) % 60, 2, 10, QChar('0'))
                                 .arg(millis % millisInSecond, 3, 10, QChar('0'));
}

void setLcdDisplay(int millis)
{
    lcdDisplay->display(millisToTimecode(millis));
}

QTime * timecode = NULL;
long timecodeLastStopped = 0;
bool currentlyPlaying = false;

long currentTimecode()
{
    if (currentlyPlaying)
        return timecode->elapsed() + timecodeLastStopped;
    else
        return timecodeLastStopped;
}

void seekToTimestamp(long seekTo)
{
    timecodeLastStopped = seekTo;
    bool syncEstim = OptionsDialog::emitEstimSignal();
    if (currentlyPlaying)
    {
        mainWindow->cancelScheduledEvent();
        timecode->restart();
        if (syncEstim)
            mainWindow->stopEstimSignal();
    }

    videoPlayer->setPosition(currentTimecode());

    if (currentlyPlaying)
    {
        mainWindow->scheduleVideoSync();
        mainWindow->scheduleEvent();
        if (syncEstim)
            mainWindow->startEstimSignal();
        if (OptionsDialog::syncTimecodeOnSeek())
            mainWindow->scheduleVideoSync();
    }
    else
        videoPlayer->pause();

    setLcdDisplay(currentTimecode());
}

QVector<unsigned char> listOfMetadataDenominators()
{
    QVector<unsigned char> returnValue;
    int i = 0;
    for (i = 0; i < events.size(); ++i)
    {
        if (events[i].metadata &&
                events[i].metadata->valueDenominator != 0 &&
                ! returnValue.contains(events[i].metadata->valueDenominator))
            returnValue.append(events[i].metadata->valueDenominator);
    }
    return returnValue;
}

QVector<unsigned char> listOfTimestampDenominators()
{
    QVector<unsigned char> returnValue;
    int i = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
    {
        if (beatIntervals[i].getValue() &&
                ! returnValue.contains(beatIntervals[i].getValue()->denominator))
            returnValue.append(beatIntervals[i].getValue()->denominator);
    }
    return returnValue;
}

BeatValue * getBeatValueByName(const QString & name)
{
    BeatValue * returnValue = nullptr;
    for (int i = 0; i < beatValues.size(); ++i) {
        if (beatValues[i].name == name)
        {
            returnValue = &beatValues[i];
            break;
        }
    }
    return returnValue;
}

BeatValue * getBeatValueFromLengthInBeats(float length)
{
    BeatValue * returnValue = nullptr;
    for (int i = 0; i < beatValues.size(); ++i) {
        if (int(beatValues[i].value()) == length)
        {
            returnValue = &beatValues[i];
            break;
        }
    }
    return returnValue;
}

BeatValue * getBeatValueFromMsLength(int length)
{
    BeatValue * returnValue = nullptr;
    for (int i = 0; i < beatValues.size(); ++i) {
        if (int(beatValues[i].getLength() + 0.5) == length)
        {
            returnValue = &beatValues[i];
            break;
        }
    }
    return returnValue;
}

BeatValue * getBeatValueFromDropdownEntry(const QString & entry)
{
    QRegularExpression expWithName("\\d+ \\((.*)\\)");
    QRegularExpressionMatch nameMatch = expWithName.match(entry);
    if (nameMatch.hasMatch()) {
        QString name = nameMatch.captured(1);
        return getBeatValueByName(name);
    }
    QRegularExpression expWithoutName("\\d+");
    QRegularExpressionMatch numberwang = expWithoutName.match(entry);
    if (numberwang.hasMatch()) {
        QString lengthString = numberwang.captured();
        int length = lengthString.toInt();
        return getBeatValueFromMsLength(length);
    }
    return nullptr;
}

QVector<Event> events;
QVector<BeatTimestamp> beatTimestamps;
QVector<BeatValue> beatValues;
QVector<BeatInterval> beatIntervals;
QList<UniqueBeatInterval> uniqueBeatIntervals;
QVector<BeatPattern> beatPatterns;
QVector<EventDispatcher *> dispatchers;

double tempoToBeatLength(double tempo)
{
    return 60000.0 / tempo;
}

double beatLengthToTempo(double beatLength)
{
    return 60000.0 / beatLength;
}
