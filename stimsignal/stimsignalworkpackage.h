#ifndef STIMSIGNALWORKPACKAGE_H
#define STIMSIGNALWORKPACKAGE_H

#include <QRunnable>
class MultithreadedSamplePipelineProcessor;
#include "stimsignalsample.h"
#include "stimsignalmodifier.h"

class StimSignalWorkPackage : public QRunnable
{
public:
    StimSignalWorkPackage(MultithreadedSamplePipelineProcessor * employer, StimSignalSample * sample, StimSignalModifier * modifier);
    MultithreadedSamplePipelineProcessor * employer;
    StimSignalSample * sample;
    StimSignalModifier * modifier;

    void run() override;
};

#endif // STIMSIGNALWORKPACKAGE_H
