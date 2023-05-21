#include "multithreadedsamplepipelineprocessor.h"
#include "stimsignalsample.h"
#include "stimsignalmodifier.h"
#include "stimsignalworkpackage.h"
#include <QThreadPool>
#include <QApplication>

MultithreadedSamplePipelineProcessor::MultithreadedSamplePipelineProcessor(QList<StimSignalSample *> *sampleVector, QList<StimSignalModifier *> *modifiers, QObject *parent)
    :
      QObject(parent),
      sampleVector(sampleVector),
      modifiers(modifiers),
      workAssigned(0),
      workComplete(0)
{
    //Nothing to do here...
}

void MultithreadedSamplePipelineProcessor::processAll()
{
    for (int thisSample = 0; thisSample < sampleVector->length(); ++thisSample)
    {
        for (int thisModifier = 0; thisModifier < modifiers->length(); ++thisModifier)
        {
            StimSignalSample * sample = sampleVector->at(thisSample);
            StimSignalModifier * modifier = modifiers->operator[](thisModifier);
            StimSignalWorkPackage * workPackage = new StimSignalWorkPackage(this,sample,modifier);
            workQueue.append(workPackage);
            ++workAssigned;
        }
    }
    for (int i = 0; i < workQueue.length(); ++i) {
        QThreadPool::globalInstance()->start(workQueue.at(i));
    }
    while (1)
    {
        if (workComplete.load() >= workAssigned)
            return;
    }
}

void MultithreadedSamplePipelineProcessor::reportWorkComplete()
{
    workComplete.fetchAndAddOrdered(1);
}
