#include "stimsignalworkpackage.h"
#include "multithreadedsamplepipelineprocessor.h"

StimSignalWorkPackage::StimSignalWorkPackage(MultithreadedSamplePipelineProcessor * employer, StimSignalSample *sample, StimSignalModifier *modifier)
    :
      employer(employer),
      sample(sample),
      modifier(modifier)
{

}

void StimSignalWorkPackage::run()
{
    modifier->modify(*sample);
    employer->reportWorkComplete();
}
