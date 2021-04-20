#include "midifilewriter.h"
#include <QDebug>
#include "helperfunctions.h"
#include "math.h"

const char * const MidiFileWriter::headerTag = "MThd";
const char * const MidiFileWriter::trackTag = "MTrk";
const int MidiFileWriter::firstTrackSizePos = 18;

quint8 MidiFileWriter::defaultDrumNote = 37; //side stick


MidiFileWriter::MidiFileWriter(QString filename, QObject *parent) :
    QObject(parent),
    outputMidiFile(filename),
    outputMidiData(&outputMidiFile),
    lastPositionWritten(0),
    runningStatus(0)
{
    if (! outputMidiFile.open(QIODevice::WriteOnly))
        qDebug() << "Could not open file for writing:" << filename;
}

quint8 MidiFileWriter::writeVariableLengthQuantity(quint32 value)
//adapted from  http://www.csw2.co.uk/tech/midi2.htm
{
    quint32 buffer;
    quint8 bytesNeeded = 1;
    buffer = value & 0x7f;
    while((value >>= 7) > 0)
    {
        buffer <<= 8;
        buffer |= 0x80;
        buffer += (value &0x7f);
        ++bytesNeeded;
    }
    while (true)
    {
        outputMidiData << (quint8) buffer;
        if(buffer & 0x80)
            buffer >>= 8;
        else
            break;
    }
    return bytesNeeded;
}

bool MidiFileWriter::writeHeader(bool useSMPTE, qint16 framesPerBeat)
{
    outputMidiFile.seek(0);
    outputMidiData.writeRawData(headerTag,4);
    outputMidiData << (qint32) 6; //midi header size
    outputMidiData << (qint16) 0; //format: only type 0 supported for now
    outputMidiData << (qint16) 1; //number of tracks - this is always 1 for type 0 files

    if (useSMPTE)
        outputMidiData << (qint16) 0b1110011000101000; //1 tick per ms - 40 ticks per frame, 25fps
    else
        outputMidiData << (qint16) (framesPerBeat & 0b0111111111111111);

    outputMidiData.writeRawData(trackTag,4);
    //firstTrackSizePos = outputMidiFile.pos(); //seems to work fine with default of 18
    outputMidiData << (qint32) 0; //midi track chunk size - fill in later
    return true;
}

quint16 MidiFileWriter::writeTitle(QString title)
{
    outputMidiData << (qint8) 0x00; //time delta
    outputMidiData << (qint16) 0xFF03; //Sequence/track title
    quint16 bytesWritten = 3;
    bytesWritten += writeVariableLengthQuantity(title.length()); //length
    bytesWritten += outputMidiData.writeRawData(title.toLatin1().data(),title.length());//text
    return bytesWritten;
}

bool MidiFileWriter::writePatchChange(quint8 channel, quint8 patchNo)
{
    //assign track to instrument (patch change)
    outputMidiData << (qint8) 0x00; //time delta
    runningStatus = (0xC0 | channel); //nybl 1 Program change, nybl 2 channel
    outputMidiData << (qint8) runningStatus;
    outputMidiData << (qint8) patchNo;
    return true;
}

bool MidiFileWriter::writeTimeSignature(quint8 numerator, quint8 denominator, quint8 clocksPerMetronomeClick, quint8 demisemiquaversPerCrochet)
{
    if (!isPowerOfTwo(denominator) &&
            denominator != 0)
        return false; //Excessively long (~=1500 chars) TODO!!!!... was here. not sure why.

    outputMidiData << (qint8) 0x00; //time delta
    outputMidiData << (qint16) 0xFF58; //time signature
    outputMidiData << (qint8) 4; //four bytes follow for a time sig event:
    outputMidiData << (qint8) numerator; //beats in a bar
    quint8 divisions = 0;
    while ( (denominator >> 1) != 0)
    {
        denominator >>= 1;
        ++divisions;
    }
    outputMidiData << (qint8) divisions; //divisions of bar - negative power of 2
    outputMidiData << (qint8) clocksPerMetronomeClick; //clocks per metronome click - default 24
    outputMidiData << (qint8) demisemiquaversPerCrochet; //demisemiquavers per crochet (don't ask, just leave)
    return true;
}

bool MidiFileWriter::writeTempo(quint16 BPM, int afterFrames)
{
    quint32 microsecondsPerBeat = 60 * 1000000l / BPM; //microseconds in minute / BPM
    return writeTempo_usPerBeat(microsecondsPerBeat, afterFrames);
}

bool MidiFileWriter::writeTempo(double BPM, int afterFrames)
{
    quint32 microsecondsPerBeat = round((long)60 * 1000000l / BPM); //microseconds in minute / BPM
    return writeTempo_usPerBeat(microsecondsPerBeat, afterFrames);
}

bool MidiFileWriter::writeTempo_usPerBeat(quint32 microsecondsPerBeat, int afterFrames)
{
    writeVariableLengthQuantity(afterFrames); //time delta
    outputMidiData << (qint16) 0xFF51; //tempo
    outputMidiData << (qint8) 3; //three (awkward) bytes follow for a tempo event:
    quint8 firstByte, middleByte, lastByte;
    firstByte = microsecondsPerBeat >> 16;
    middleByte = microsecondsPerBeat >> 8;
    lastByte = microsecondsPerBeat >> 0;
    outputMidiData << firstByte;
    outputMidiData << middleByte;
    outputMidiData << lastByte;
    runningStatus = 0;
    return true;
}

void MidiFileWriter::writeBeat(quint32 timestampInMiliseconds, quint8 note, quint8 channel, quint8 velocity)
{
    writeVariableLengthQuantity(timestampInMiliseconds - lastPositionWritten); //delta time in 1ms SMTPE frames
    writeBeatNow(note, channel, velocity);
    lastPositionWritten = timestampInMiliseconds;
}

void MidiFileWriter::writeBeat_frameDelta(quint32 framesSinceLastBeat, quint8 note, quint8 channel, quint8 velocity)
{
    writeVariableLengthQuantity(framesSinceLastBeat); //delta time, in frames
    writeBeatNow(note, channel, velocity);
}

void MidiFileWriter::writeBeatNow(quint8 note, quint8 channel, quint8 velocity)
{
    //assume delta time already written
    quint8 thisStatus = 0x90 | channel;
    if (thisStatus != runningStatus)
    {
        outputMidiData << thisStatus;
        runningStatus = thisStatus;
    }
    outputMidiData << (qint8) note;
    outputMidiData << (qint8) velocity;

    outputMidiData << (qint8) 0; //time delta - immediately stop again (shouldn't matter for drums)
    //running status: note on, channel 10
    outputMidiData << (qint8) note; //side stick
    outputMidiData << (qint8) 0; //velocity 0 ~= note off
}

void MidiFileWriter::writeEndOfTrack()
{
    outputMidiData << (qint8) 0x00; //time delta
    outputMidiData << (qint16) 0xFF2F;//end of track
    outputMidiData << (qint8) 0x00;//dummy byte for end of track 'value'

    outputMidiFile.seek(firstTrackSizePos);
    outputMidiData << (quint32) ( outputMidiFile.size() - 22 );

    outputMidiFile.close();
}
