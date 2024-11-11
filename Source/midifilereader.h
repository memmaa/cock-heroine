#ifndef MIDIFILEREADER_H
#define MIDIFILEREADER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QTextStream>

class MidiFileReader : public QObject
{
public:
    MidiFileReader(QString filename, QObject *parent);
    quint32 readVariableLengthQuantity();
    void printAsVariableLengthQuantity(unsigned int value);
    bool readFile();
    bool handleNewTempo();
    void configureIntegerDivisionCompensation();
    bool handleMidiNoteOn(unsigned char statusByte);
    bool createEventNow(unsigned char key, unsigned char velocity, unsigned char channel);
    bool handleMetaEvent();
    bool ignoreVariableQuantity();
    bool ignoreFixedQuantity(unsigned int bytesToIgnore);
    void advanceCurrentTimeByDivisions(unsigned int numberOfDivisions);

    QFile inputMidiFile;
    QDataStream inputMidiData;

    long currentMicroseconds;

    bool usingSMTPE;

    int microsecondsPerBeat;
    int microsecondsPerDivision;

    quint8 framesPerSecond;
    quint8 ticksPerFrame;
    unsigned int ticksPerSecond;
    unsigned int framesPerBeat;

    int extraMicroseconds;
    int extraMicrosecondsCounter;
    int addExtraMicrosecondsEvery;

    quint8 runningStatus;

    static const int firstTrackSizePos;
};

#endif // MIDIFILEREADER_H
