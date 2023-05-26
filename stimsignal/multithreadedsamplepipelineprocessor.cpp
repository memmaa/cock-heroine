#include "multithreadedsamplepipelineprocessor.h"
#include "stereostimsignalsample.h"
#include "stimsignalmodifier.h"
#include <QThreadPool>
#include <QApplication>

MultithreadedSamplePipelineProcessor::MultithreadedSamplePipelineProcessor(QList<StereoStimSignalSample *> *sampleVector, QList<StimSignalModifier *> *modifiers, QObject *parent)
    :
      QObject(parent),
      sampleVector(sampleVector),
      modifiers(modifiers),
//      workAssigned(0),
//      workComplete(0)
      allDone(false)
{
    //Nothing to do here...
}

void MultithreadedSamplePipelineProcessor::processAll()
{
    //The below is not particularly fast (presumably due to the overhead of splitting up the work into so many small packages)
    //and on an AMD Ryzen7 4800U, it's actually slower than signle-threading.
    //below you will find a second attempt...
//    for (int thisSample = 0; thisSample < sampleVector->length(); ++thisSample)
//    {
//        for (int thisModifier = 0; thisModifier < modifiers->length(); ++thisModifier)
//        {
//            StimSignalSample * sample = sampleVector->at(thisSample);
//            StimSignalModifier * modifier = modifiers->operator[](thisModifier);
//            StimSignalWorkPackage * workPackage = new StimSignalWorkPackage(this,sample,modifier);
//            workQueue.append(workPackage);
//            ++workAssigned;
//        }
//    }
//    for (int i = 0; i < workQueue.length(); ++i) {
//        QThreadPool::globalInstance()->start(workQueue.at(i));
//    }
//    while (1)
//    {
//        if (workComplete.load() >= workAssigned)
//            return;
//    }
    for (int i = 0; i < modifiers->length(); ++i)
    {
        StimSignalWorker * worker = new StimSignalWorker(sampleVector, modifiers->at(i));
        QThread * thread = new QThread();
        worker->moveToThread(thread);
        workers.append(worker);
        workerThreads.append(thread);

        if (i == 0)
        {
            //first run through, connect up the first worker so we can feed it work
            connect(this, SIGNAL(runThroughAllWorkers(int)), worker, SLOT(processIndex(int)));
        }
        else
        {
            //all subsequent runs connect the current worker to receive work from the previous one
            StimSignalWorker * lastWorker = workers.at(i-1);
            connect(lastWorker, SIGNAL(indexProcessed(int)), worker, SLOT(processIndex(int)));
        }
        if (i == modifiers->length() - 1)
        {
            //on the last run through, set the worker to notify us when it's done
            connect(worker, SIGNAL(indexProcessed(int)), this, SLOT(registerWorkComplete(int)));
        }
        thread->start();
    }

    for (int i = 0; i < sampleVector->length(); ++i) {
        emit runThroughAllWorkers(i);
    }

    while (1)
    {
        if (allDone)
            return;
        QCoreApplication::processEvents();
    }
}

void MultithreadedSamplePipelineProcessor::registerWorkComplete(int index)
{
    if (index == sampleVector->length() - 1)
    {
        allDone = true;
    }
}
