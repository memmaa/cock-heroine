#ifndef MIDIFILEWRITER_H
#define MIDIFILEWRITER_H

#include <QObject>
#include <QFile>
#include <QDataStream>

class MidiFileWriter : public QObject
{
    Q_OBJECT
public:
    explicit MidiFileWriter(QString filename, QObject *parent = 0);

    bool writeHeader(bool useSMPTE, qint16 framesPerBeat = 0);
    quint8 writeVariableLengthQuantity(quint32 value);
    quint16 writeTitle(QString title);
    bool writeTimeSignature(quint8 numerator, quint8 denominator, quint8 clocksPerMetronomeClick = 24, quint8 demisemiquaversPerCrochet = 8);
    bool writeTempo(quint16 BPM, int afterFrames = 0);
    bool writeTempo(double BPM, int afterFrames = 0);
    bool writeTempo_usPerBeat(quint32 microsecondsPerBeat, int afterFrames = 0);
    bool writePatchChange(quint8 channel, quint8 patchNo);
    void writeBeat(quint32 timestampInMiliseconds, quint8 note = defaultDrumNote, quint8 channel = 9 /*drums*/, quint8 velocity = 127 /*full whack*/);
    void writeBeat_frameDelta(quint32 framesSinceLastBeat, quint8 note = defaultDrumNote, quint8 channel = 9 /*drums*/, quint8 velocity = 127 /*full whack*/);
    void writeBeatNow(quint8 note = defaultDrumNote, quint8 channel = 9 /*drums*/, quint8 velocity = 127 /*full whack*/);
    void writeEndOfTrack();

signals:

public slots:

private:
    QFile outputMidiFile;
    QDataStream outputMidiData;

    long lastPositionWritten;
    quint8 runningStatus;

    static const char * const headerTag;
    static const char * const trackTag;
    static const int firstTrackSizePos;
    static quint8 defaultDrumNote;
};

#define BASS_DRUM_1 36
#define SIDE_STICK 37
#define ACOUSTIC_SNARE 38
#define ELECTRIC_SNARE 40
#define CLOSED_HIHAT 41
#define OPEN_HIHAT 46

#endif // MIDIFILEWRITER_H
