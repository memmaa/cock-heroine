#include "stimsignalworker.h"
#include "stimsignalmodifier.h"

StimSignalWorker::StimSignalWorker(QList<StimSignalSample *> *sampleVector, StimSignalModifier *modifier, QObject *parent)
    :
        QObject(parent),
        sampleVector(sampleVector),
        modifier(modifier)
{

}

void StimSignalWorker::setVector(QList<StimSignalSample *> *newVector)
{
    sampleVector = newVector;
}

void StimSignalWorker::processIndex(int index)
{
    modifier->modify(*((*sampleVector)[index]));
    emit indexProcessed(index);
}
