#ifndef PHASESETTERMODIFIER_H
#define PHASESETTERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class PhaseSetterModifier : public StimSignalModifier
{
public:
    //!
    //! \brief PhaseSetterModifier
    //! \param startFrequency
    //! \param endFrequency
    //! \param channel by deafult this is 0, and will apply to the first channel. Set to -1 to apply to all channels
    //!
    PhaseSetterModifier(int startFrequency, int endFrequency, int channel = 0);
    void modify(StimSignalSample &sample) override;

private:
    void setStep(long timestamp);
    qreal phase;
    qreal step;
    int startingFrequency;
    int endingFrequency;
    int updateCounter;
    int frequencyUpdateInterval;
    int channel;
};

#endif // PHASESETTERMODIFIER_H
