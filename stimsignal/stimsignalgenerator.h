#ifndef STIMSIGNALGENERATOR_H
#define STIMSIGNALGENERATOR_H

#include <QBuffer>
#include <QAudioFormat>
class StimSignalModifier;

//DONE: Add prefs for: starting frequ, - Used
//                     ending freq, - Used
//                     percentage vol increase over duration, - Used
//                     max stroke duration, - Used
//                     (slightly?) increase volume of faster strokes - Used
//                     stroke type (up-down-beat or down-beat-up) - Used
//                     invert phase - Used
//                     initial fade-in time from cold - Used
//                     lower volume upto X% after breaks upto Yseconds - Used
//                     lift volume reduction over period of... - Used
//                     panning left or right - Used
//DONE: Use above prefs
//DONE: Allow saving of signal to a file
//DONE: Allow selection of audio device (stim signal)
//DONE: Start and stop signal with video

class StimSignalGenerator : public QIODevice
{
    Q_OBJECT
public:
    explicit StimSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
    //TODO: destructor to delete modifiers

    qint64 generate(char *data, qint64 maxlen);

//    bool open(OpenMode mode) override;
    qint64 readData(char *data, qint64 maxlen) override; //TODO
    qint64 writeData(const char *data, qint64 len) override;
    bool seek(qint64 pos) override; //don't do this
    bool isSequential() const override;
    void setGenerateFrom(long from);

    static StimSignalGenerator * createFromPrefs(QObject * parent);

protected:
    virtual void setModifiers() = 0;
    virtual long getStopTimestamp() = 0;
    int sampleCounter;
    QAudioFormat audioFormat;
    long generateFromTimestamp = 0;
    int startingFrequency;
    int endingFrequency;
    QList<StimSignalModifier *> modifiers;

signals:
    void progressed(int progress, int outOf);

};

#endif // STIMSIGNALGENERATOR_H
