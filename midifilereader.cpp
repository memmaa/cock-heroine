#include "midifilereader.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

#include "globals.h"
#include "helperfunctions.h"
#include "mainwindow.h"

MidiFileReader::MidiFileReader(QString filename, QObject *parent) :
    QObject(parent),
    inputMidiFile(filename),
    inputMidiData(&inputMidiFile),
    currentMicroseconds(0),
    usingSMTPE(false),
    microsecondsPerBeat(0),
    microsecondsPerDivision(0),
    framesPerSecond(0),
    ticksPerFrame(0),
    ticksPerSecond(0),
    framesPerBeat(0),
    extraMicroseconds(0),
    extraMicrosecondsCounter(0),
    addExtraMicrosecondsEvery(0),
    runningStatus(0)
{
    if (! inputMidiFile.open(QIODevice::ReadOnly))
        qDebug() << "Could not open file for reading:" << filename;
}

quint32 MidiFileReader::readVariableLengthQuantity()
{
    quint32 retVal = 0;
    quint8 thisByte = 0;
    bool lastByte = false;

    while (lastByte == false)
    {
        inputMidiData >> thisByte;
        lastByte = ((thisByte & 0b10000000) == 0); //first bit is a 0
        qint8 thisValue = thisByte & 0b01111111;
        retVal <<= 7;
        retVal += thisValue;
        if (lastByte)
            return retVal;
    }
    return retVal;
}

bool MidiFileReader::readFile()
{
    if (! inputMidiFile.isOpen())
    {
        qDebug() << "File not open for reading";
        return false;
    }

    char fileHeader[4];
    inputMidiData.readRawData(fileHeader,4);
    QString qFileHeader = QString(fileHeader);
    qFileHeader.truncate(4);

    if (qFileHeader != QString("MThd"))
    {
        qDebug() << QString("Incorrect file header: %1").arg(QString(fileHeader));
        return false;
    }

    qint32 midiHeaderSize;
    qint16 midiFormat;
    qint16 numTracks;
    inputMidiData >> midiHeaderSize;
    inputMidiData >> midiFormat;
    inputMidiData >> numTracks;

    if (midiHeaderSize != 6)
    {
        qDebug() << QString("Unexpected header size: %1 (expected 6)").arg(midiHeaderSize);
        return false;
    }

    if (midiFormat != 0)
    {
        qDebug() << QString("Unfortunately, Sillyface hasn't programmed the reading of type %1 MIDI files (only type 0).").arg(midiFormat);
        return false;
    }

    if (numTracks != 1)
    {
        qDebug() << QString("Unexpected number of tracks: %1 (expected 1).").arg(numTracks);
        return false;
    }

    qint16 divisionInfo;
    inputMidiData >> divisionInfo;
    if (divisionInfo & 0b1000000000000000) //first bit is a 1
    {
        usingSMTPE = true;
        framesPerSecond = - (divisionInfo >> 8);
        --framesPerSecond; //negative complement adjustment
        ticksPerFrame = divisionInfo; //truncates
        if (framesPerSecond != 24 &&
                framesPerSecond != 25 &&
                framesPerSecond != 29 &&
                framesPerSecond != 30 )
        {
            qDebug() << QString("Unknown SMTPE framerate: %1 (expected 24,25,29 or 30).").arg(framesPerSecond);
            return false;
        }
        ticksPerSecond = framesPerSecond * ticksPerFrame;
        configureIntegerDivisionCompensation();
    }
    else //first bit is 0
    {
        usingSMTPE = false;
        framesPerBeat = divisionInfo;
    }

    char trackHeader[4];
    inputMidiData.readRawData(trackHeader,4);
    QString qTrackHeader = QString(trackHeader);
    qTrackHeader.truncate(4);

    if (qTrackHeader != QString("MTrk"))
    {
        qDebug() << QString("Incorrect track header: %1").arg(QString(fileHeader));
        return false;
    }

    quint32 trackChunkSize = 0;
    inputMidiData >> trackChunkSize;

    //and now start reading...

    bool error = false;
    QString errorMessage;
    unsigned char statusByte = 0;
    unsigned char runningStatus = 0;
    unsigned char thisByte = 0;

    while (error != true && !inputMidiFile.atEnd())
    {
        QTextStream(stdout) << '\n';
        quint32 deltaTime = readVariableLengthQuantity();
        advanceCurrentTimeByDivisions(deltaTime);
        printAsVariableLengthQuantity(deltaTime);

        inputMidiFile.peek((char *)&thisByte,1);
        if (thisByte & 0x80) //this is a status byte
        {
            inputMidiData >> statusByte;
            QTextStream(stdout) << QString(" %1 ").arg((ushort)statusByte,2,16,QChar('0')).toUpper();
        }
        else //we're using 'running' status, so reuse the value from last time
        {
            statusByte = runningStatus;
            QTextStream(stdout) << QString("(%1)").arg((ushort)statusByte,2,16,QChar('0')).toUpper();
        }
        switch (statusByte >> 4) //check the first nyble first
        {
            case 0:
                //we have no status byte!
                //anything lower than 0x80 is a data byte, not a status byte
                //MIDI standard says nothing should be done until a status byte is found.
                qDebug() << "WARNING: Missing status byte!";
                continue;
            case 0x8:
                //note off - we don't care
                ignoreFixedQuantity(2);
                runningStatus = statusByte;
                break;
            case 0x9:
                error = !handleMidiNoteOn(thisByte);
                runningStatus = statusByte;
                break;
            case 0xA:
                //Polyphonic Key Pressure - we don't care
                ignoreFixedQuantity(2);
                runningStatus = statusByte;
                break;
            case 0xB:
                //Controller Change - we don't care
                ignoreFixedQuantity(2);
                runningStatus = statusByte;
                break;
            case 0xC:
                //Program Change - we don't care
                ignoreFixedQuantity(1);
                runningStatus = statusByte;
                break;
            case 0xD:
                //Channel Key Pressure - we don't care
                ignoreFixedQuantity(1);
                runningStatus = statusByte;
                break;
            case 0xE:
                //Pitch Bend - we don't care
                ignoreFixedQuantity(2);
                runningStatus = statusByte;
                break;
            case 0xF:
                //this is where all the interesting stuff that we wish
                //we didn't have to deal with happens
                switch (statusByte)
                {
                    case 0b11111000: //system real time: timing clock
                    case 0b11111010: //system real time: start (song)
                    case 0b11111011: //system real time: stop
                    case 0b11111110: //system real time: active sensing
                    //case 0b11111111: //system real time: reset (potentially clashes with 0xFF meta messages, so ignore)
                        //system real time messages don't cancel running status.
                        //ignore
                        continue;
                    case 0xFF:
                        //metaEvent
                        error = !handleMetaEvent();
                        runningStatus = 0;
                        break;
                    case 0xF0: //type 1 system exclusive message
                    case 0xF7: //type 2 system exclusive message
                        error = !ignoreVariableQuantity();
                        runningStatus = 0;
                        break;
                    default:
                        error = true;
                        runningStatus = 0;
                        errorMessage = QString("Unknown status byte : ") + QString("%1.").arg(thisByte,2,16,QChar('0')).toUpper();
                        break;
                }
                break;
            default:
                error = true;
                runningStatus = 0;
                errorMessage = QString("Unknown byte : ") + QString("%1.").arg(thisByte,2,16,QChar('0')).toUpper();
                break;
        }
    }

    if (error)
    {
        qDebug() << errorMessage;
        return false;
    }
    return true;
}

bool MidiFileReader::handleMidiNoteOn(unsigned char statusByte)
{
    unsigned char channel = statusByte & 0x0F;
    quint8 key;
    quint8 velocity;
    inputMidiData >> key;
    QTextStream(stdout) << QString("%1 ").arg((ushort)key,2,16,QChar('0')).toUpper();
    inputMidiData >> velocity;
    QTextStream(stdout) << QString("%1 ").arg((ushort)velocity,2,16,QChar('0')).toUpper();
    if (velocity) //otherwise it's a note off, we don't care.
    {
        QTextStream(stdout) << "Note on";
        createEventNow(key, velocity, channel); //do we care about channel? Let's say yes.
    }
    else
        QTextStream(stdout) << "Note off";
    return true; //success
}

bool MidiFileReader::createEventNow(unsigned char /*key*/, unsigned char /*velocity*/, unsigned char /*channel*/)
{
    int currentTimestamp = (currentMicroseconds + 500) / 1000;
    Event newEvent(currentTimestamp,1,127);
    mainWindow->addEventToTable(newEvent);
    return true;
}

bool MidiFileReader::ignoreVariableQuantity()
{
    unsigned int lengthToIgnore = readVariableLengthQuantity();
    printAsVariableLengthQuantity(lengthToIgnore);
    quint8 aByte;
    for (unsigned int i = 0; i < lengthToIgnore; ++i)
    {
        inputMidiData >> aByte;
        QTextStream(stdout) << QString("%1 ").arg((ushort)aByte,2,16,QChar('0')).toUpper();
    }
    return true; //success
}

bool MidiFileReader::ignoreFixedQuantity(unsigned int bytesToIgnore)
{
    quint8 aByte;
    for (unsigned int i = 0; i < bytesToIgnore; ++i)
    {
        inputMidiData >> aByte;
        QTextStream(stdout) << QString("%1 ").arg((ushort)aByte,2,16,QChar('0')).toUpper();
    }
    return true; //success
}

bool MidiFileReader::handleMetaEvent()
{
    unsigned char type;
    inputMidiData >> type;
    QTextStream(stdout) << QString("%1 ").arg((ushort)type,2,16,QChar('0')).toUpper();
    unsigned int eventLength;
    char * dataBytes = NULL;
    switch (type)
    {
        case 0x00: // Sequence number
            qDebug() << "Sequence numbers not supported";
            return false; //failure
        case 0x01: // Text event
            if (type == 0x01) QTextStream(stdout) << "Some text: ";
            //fall through
        case 0x02: // Copyright notice
            if (type == 0x02) QTextStream(stdout) << "Ooh look - copyright! ";
                   //fall through"
        case 0x03: // Sequence or track name
            if (type == 0x03) QTextStream(stdout) << "Sequence or track name: ";
                   //fall through"
        case 0x04: // Instrument name
            if (type == 0x04) QTextStream(stdout) << "Instrument name: ";
                   //fall through"
        case 0x05: // Lyric text
            if (type == 0x05) QTextStream(stdout) << "Lyric text: ";
                   //fall through"
        case 0x06: // Marker text
            if (type == 0x06) QTextStream(stdout) << "Marker text: ";
                   //fall through"
        case 0x07: // Cue point
            if (type == 0x07) QTextStream(stdout) << "Cue point text: ";
                   //fall through"
            eventLength = readVariableLengthQuantity();
            dataBytes = new char[eventLength + 1];
            inputMidiData.readRawData(dataBytes,eventLength); //need somewhere to store string...
            QTextStream(stdout) << dataBytes;
            delete [] dataBytes;
            dataBytes = NULL;
            break;
        case 0x20: // MIDI channel prefix assignment
            qDebug() << "MIDI channel prefix assignments not supported";
            return false; //failure
        case 0x2F: // End of track
            QTextStream(stdout) << "End of track found.";
            ignoreVariableQuantity();
            return false;
            break;
        case 0x51: // Tempo change
            QTextStream(stdout) << "Tempo: ";
            return handleNewTempo();
        case 0x54: // SMPTE offset
            qDebug() << "SMPTE offset not supported! :-(";
            return false;
        case 0x58: // Time signature
            if (type == 0x58) QTextStream(stdout) << "Time signature: ";
        case 0x59: // Key signature
            if (type == 0x59) QTextStream(stdout) << "Key signature: ";
            //don't care
            ignoreVariableQuantity();
            break;
        case 0x7F: // Sequencer specific event
            QTextStream(stdout) << "Ignoring Sequencer specific event...";
            ignoreVariableQuantity();
            break;
    }

    return true; //success
}

bool MidiFileReader::handleNewTempo()
{
    quint32 eventContent;
    inputMidiData >> eventContent;
    unsigned char eventLength = eventContent >> 24;
    if (eventLength != 3)
        return false;
    if (usingSMTPE)
    {
        QTextStream(stdout) << "Ignoring tempo change in SMTPE-type file";
        return true; //could be false for error on this condition
    }
    microsecondsPerBeat = eventContent & 0x00FFFFFF; //strip off '3' event length
    double bpm = 60 / ((double)microsecondsPerBeat / 1000000);
    QTextStream(stdout) << microsecondsPerBeat << " microseconds per beat (roughly " << bpm << " BPM).";
    configureIntegerDivisionCompensation();
    return true;
}

void MidiFileReader::configureIntegerDivisionCompensation()
{
    if (usingSMTPE)
    {
        microsecondsPerDivision = 1000000 / ticksPerSecond;
        int remainder = 1000000 % microsecondsPerDivision;
        if (remainder)
        {
            int gcd = greatestCommonDivisor(remainder,ticksPerSecond);
            extraMicroseconds = remainder / gcd;
            addExtraMicrosecondsEvery = framesPerBeat / gcd;
        }
        else
        {
            extraMicroseconds = 0;
        }
    }
    else
    {
        microsecondsPerDivision = microsecondsPerBeat / framesPerBeat;
        int remainder = microsecondsPerBeat % microsecondsPerDivision;
        if (remainder)
        {
            int gcd = greatestCommonDivisor(remainder,framesPerBeat);
            extraMicroseconds = remainder / gcd;
            addExtraMicrosecondsEvery = framesPerBeat / gcd;
        }
        else
        {
            extraMicroseconds = 0;
        }
    }
    extraMicrosecondsCounter = 0;
}

void MidiFileReader::advanceCurrentTimeByDivisions(unsigned int numberOfDivisions)
{
    currentMicroseconds += numberOfDivisions * microsecondsPerDivision;
    if (extraMicroseconds)
    {
        extraMicrosecondsCounter += numberOfDivisions;
        currentMicroseconds += (extraMicroseconds / addExtraMicrosecondsEvery);
        extraMicrosecondsCounter %= addExtraMicrosecondsEvery;
    }
}

void MidiFileReader::printAsVariableLengthQuantity(unsigned int value)
{
    unsigned char byte1, byte2, byte3, byte4;
    byte1 = value >> 21;
    byte2 = value >> 14;
    byte3 = value >> 7;
    byte4 = value;
    if (byte1)
    {
        byte1 = byte1 | 0x80;
        QTextStream(stdout) << QString(" %1").arg((ushort)byte1,2,16,QChar('0')).toUpper();
    }
    else
        QTextStream(stdout) << QString("   ");
    if (byte2)
    {
        byte2 = byte2 | 0x80;
        QTextStream(stdout) << QString(" %1").arg((ushort)byte2,2,16,QChar('0')).toUpper();
    }
    else
        QTextStream(stdout) << QString("   ");
    if (byte3)
    {
        byte3 = byte3 | 0x80;
        QTextStream(stdout) << QString(" %1").arg((ushort)byte3,2,16,QChar('0')).toUpper();
    }
    else
        QTextStream(stdout) << QString("   ");

    byte4 = byte4 & 0x7F;
    QTextStream(stdout) << QString(" %1 ").arg((ushort)byte4,2,16,QChar('0')).toUpper();
}
