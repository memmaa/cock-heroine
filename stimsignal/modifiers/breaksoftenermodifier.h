#ifndef BREAKSOFTENERMODIFIER_H
#define BREAKSOFTENERMODIFIER_H

#include "stimsignal/stimsignalmodifier.h"

class BreakSoftenerModifier : public StimSignalModifier
{
public:
    //!
    //! \brief BreakSoftenerModifier this modifier will apply quietning to the beginning of signals following a long break.
    //!        It will always quieten all channels equally, but you can set the channel parameter to dictate which channel
    //!        will be used to decide whether a break is happening (whether that channel is currently silent)
    //! \param channel set which channel will be checked to see whether the signal is currently silent (which
    //!        is taken to mean now is a break). If omitted, a channel will be chosen based on which is most often
    //!        silent (that is - the one with the shorter decay time)
    //!
    BreakSoftenerModifier(int channel = -1);
    void modify(StimSignalSample &sample) override;

private:
    qreal currentLevel;
    qreal maxQuietening;
    qreal quieteningPerSilentSample;
    qreal loudeningPerActiveSample;
    int channel;

    bool atMaxQuietening();
    bool atMaxVolume();
};

#endif // BREAKSOFTENERMODIFIER_H
