#ifndef STIMSIGNALWORKER_H
#define STIMSIGNALWORKER_H

#include <QObject>
class StereoStimSignalSample;
class StimSignalModifier;
#include <QList>

class StimSignalWorker : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalWorker(QList<StereoStimSignalSample *> * sampleVector, StimSignalModifier * modifier, QObject *parent = nullptr);

signals:
    void indexProcessed(int index);

public slots:
    void processIndex(int index);

private:
    QList<StereoStimSignalSample *> * sampleVector;
    StimSignalModifier * modifier;
};

#endif // STIMSIGNALWORKER_H
