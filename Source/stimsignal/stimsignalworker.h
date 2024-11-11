#ifndef STIMSIGNALWORKER_H
#define STIMSIGNALWORKER_H

class StimSignalSample;
class StimSignalModifier;
template<typename T> class QList;

class StimSignalWorker : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalWorker(QList<StimSignalSample *> * sampleVector, StimSignalModifier * modifier, QObject *parent = nullptr);
    void setVector(QList<StimSignalSample *> * newVector);

signals:
    void indexProcessed(int index);

public slots:
    void processIndex(int index);

private:
    QList<StimSignalSample *> * sampleVector;
    StimSignalModifier * modifier;
};

#endif // STIMSIGNALWORKER_H
