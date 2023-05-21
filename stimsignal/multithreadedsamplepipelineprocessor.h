#ifndef MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H
#define MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H

#include <QObject>
#include "stimsignalsample.h"
#include "stimsignalmodifier.h"
class StimSignalWorkPackage;
#include <QAtomicInt>

class MultithreadedSamplePipelineProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MultithreadedSamplePipelineProcessor(QList<StimSignalSample *> * sampleVector, QList<StimSignalModifier *> * modifiers, QObject *parent = nullptr);
    //!
    //! \brief processAll will run all the samples through all the modifiers and then return
    //!
    void processAll();
    void reportWorkComplete();

signals:

private:
    QList<StimSignalSample *> * sampleVector;
    QList<StimSignalModifier *> * modifiers;
    QList<StimSignalWorkPackage *> workQueue;
    int workAssigned;
    QAtomicInt workComplete;
};

#endif // MULTITHREADEDSAMPLEPIPELINEPROCESSOR_H
