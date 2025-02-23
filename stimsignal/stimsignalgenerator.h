#ifndef STIMSIGNALGENERATOR_H
#define STIMSIGNALGENERATOR_H

#include <QBuffer>
#include <QAudioFormat>
#include "stimsignalsource.h"
class StimSignalModifier;
class StimSignalSample;
class MultithreadedSamplePipelineProcessor;
class WaypointList;

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

class StimSignalGenerator : public QIODevice, public StimSignalSource
{
    Q_OBJECT
public:
    explicit StimSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
    ~StimSignalGenerator();
    //TODO: destructor to delete modifiers

    qint64 generate(char *data, qint64 maxlen);

    bool open(OpenMode mode) override;
    void close() override;
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    bool seek(qint64 pos) override; //don't do this
    bool isSequential() const override;
    void setGenerateFrom(long from);
    void setPlayFrom(long timestamp) override;

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
    WaypointList * createWaypointList(bool waypointsComeOnOrBeforeBeat, qreal peakPositionInCycle, qreal troughLevel);

private:
    virtual StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) = 0;
    MultithreadedSamplePipelineProcessor * sampleProcessor;

signals:
    void progressed(int progress, int outOf);

};

#endif // STIMSIGNALGENERATOR_H
