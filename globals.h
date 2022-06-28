#ifndef GLOBALS_H
#define GLOBALS_H

#include <QVector>
#include <QList>
#include "beatpattern.h"

class QTime;
class QTimer;
class Event;
class BeatTimestamp;
class BeatValue;
class BeatInterval;
class UniqueBeatInterval;
class QMediaPlayer;
class EditorWindow;
class MainWindow;
class QLCDNumber;
class EventDispatcher;

extern QVector<Event> events;
extern QVector<BeatTimestamp> beatTimestamps;
extern QVector<BeatValue> beatValues;
extern QVector<BeatInterval> beatIntervals;
extern QList<UniqueBeatInterval> uniqueBeatIntervals;
extern QVector<BeatPattern> beatPatterns;
extern const char * defaultValueShortcuts[2][10];

extern QTimer * refreshTimer;
extern QMediaPlayer * videoPlayer;
extern MainWindow * mainWindow;
extern EditorWindow * editor;
extern QTime * timecode;
extern long timecodeLastStopped;
extern bool currentlyPlaying;
extern long currentTimecode();
extern void seekToTimestamp (long seekTo);
QVector<unsigned char> listOfTimestampDenominators();
QVector<unsigned char> listOfMetadataDenominators();
extern QLCDNumber * lcdDisplay; //so seekToTimestamp can update it
void setLcdDisplay(int millis);

BeatValue * getBeatValueByName(const QString & name);
BeatValue * getBeatValueFromIntLength(int length);
BeatValue * getBeatValueFromDropdownEntry(const QString & entry);
QString millisToTimecode(unsigned int millis);

extern QVector<EventDispatcher *> dispatchers;

#endif // GLOBALS_H
