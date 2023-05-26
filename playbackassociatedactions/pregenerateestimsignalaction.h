#ifndef PREGENERATEESTIMSIGNALACTION_H
#define PREGENERATEESTIMSIGNALACTION_H
#include "playbackassociatedaction.h"
#include "stimsignal/estimwavfilewriter.h"

class PregenerateEstimSignalAction : public PlaybackAssociatedAction
{
public:
    PregenerateEstimSignalAction();
    ~PregenerateEstimSignalAction();

    void kickOff() override;
    void abort() override;

private:
    QString calculateFilename();
    EstimWavFileWriter * writer;
    bool cancelRequested;
};

#endif // PREGENERATEESTIMSIGNALACTION_H
