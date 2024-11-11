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
//!
//! \brief beatValues a list of all the possible 'types' of interval length we recognise.
//!        At time of writing these are populated in the beat analysis code, but they could (also) be
//! 	   loaded from a save file. New types of value are added via the editor (values table).
//! 	   Also can be enabled or created from the Enable/Identify Values Dialog.
//!
extern QVector<BeatValue> beatValues;
//!
//! \brief beatIntervals This is just a list of the 'spaces' between the beats (BeatTimestamps).
//!
extern QVector<BeatInterval> beatIntervals;
//!
//! \brief uniqueBeatIntervals If you were to create a graph for all the intervals, with interval length
//!        as the x axis and abundance as the y axis, then the idea is you have one uniqueBeatInterval for
//! 	   each peak on that graph.
//!
extern QList<UniqueBeatInterval> uniqueBeatIntervals;
extern QVector<BeatPattern> beatPatterns;
extern const char * defaultValueShortcuts[2][10];

extern QString loadedVideo;
extern QString loadedScript;

extern QTimer * refreshTimer;
extern QMediaPlayer * videoPlayer;
extern QMediaPlayer * stimSignalPlayer;
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
BeatValue * getBeatValueFromMsLength(int length);
BeatValue * getBeatValueFromLengthInBeats(float length);
BeatValue * getBeatValueFromDropdownEntry(const QString & entry);
QString millisToTimecode(unsigned int millis);

extern QVector<EventDispatcher *> dispatchers;

double tempoToBeatLength(double tempo);
double beatLengthToTempo(double beatLength);

#endif // GLOBALS_H
