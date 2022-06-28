#ifndef STIMSIGNALGENERATOR_H
#define STIMSIGNALGENERATOR_H

#include <QBuffer>
#include <QAudioFormat>
class StimSignalModifier;

#define DEFAULT_MAX_TRIPHASE_STROKE_LENGTH 500

class StimSignalGenerator : public QIODevice
{
    Q_OBJECT
public:
    static int getMaxTriphaseStrokeLength();

    explicit StimSignalGenerator(int frequency, QAudioFormat audioFormat, QObject *parent = nullptr);

    QByteArray generate(long startTimestamp);

    bool open(OpenMode mode) override;
    qint64 readData(char *data, qint64 maxlen) override; //TODO
    qint64 writeData(const char *data, qint64 len) override;
    bool seek(qint64 pos) override;

private:
    int frequency;
    int sampleCounter;
    QAudioFormat audioFormat;
    long generateFromTimestamp = 0;
    void ensureQueueFull();
    void assignBuffer();
    void emptyQueue();
    QList<StimSignalModifier *> modifiers;
    QList<QByteArray *> bufferQueue;
    QBuffer buffer;
signals:

};

#endif // STIMSIGNALGENERATOR_H
