#ifndef MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H
#define MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H

#include <QObject>
#include "stereostimsignalsample.h"
#include "stimsignalmodifier.h"
//class StimSignalWorkPackage;
//#include <QAtomicInt>
#include "stimsignalworker.h"


class MultithreadedSamplePipelineProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MultithreadedSamplePipelineProcessor(QList<StimSignalModifier *> * modifiers, QObject *parent = nullptr);
    ~MultithreadedSamplePipelineProcessor();
    //!
    //! \brief processAll will run all the samples through all the modifiers and then return
    //!
    void processAll(QList<StimSignalSample *> * vector );

signals:
    void runThroughAllWorkers(int index);

public slots:
    void registerWorkComplete(int index);

private:
    void setUpWorkers();
    QList<StimSignalSample *> * sampleVector;
    QList<StimSignalModifier *> * modifiers;
    //belongs to old, slow method
//    QList<StimSignalWorkPackage *> workQueue;
//    int workAssigned;
//    QAtomicInt workComplete;
    QList<StimSignalWorker *> workers;
    QList<QThread *> workerThreads;
    bool allDone = false;
};

#endif // MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H
