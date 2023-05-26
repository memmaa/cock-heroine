#include "stimsignalworker.h"
#include "stimsignalmodifier.h"

StimSignalWorker::StimSignalWorker(QList<StereoStimSignalSample *> *sampleVector, StimSignalModifier *modifier, QObject *parent)
    :
        QObject(parent),
        sampleVector(sampleVector),
        modifier(modifier)
{

}

void StimSignalWorker::processIndex(int index)
{
    modifier->modify(*((*sampleVector)[index]));
    emit indexProcessed(index);
}
