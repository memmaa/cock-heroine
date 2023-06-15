#ifndef DUALCHANNELSIGNALGENERATOR_H
#define DUALCHANNELSIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class DualChannelSignalGenerator : public StimSignalGenerator
{
public:
    explicit DualChannelSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // DUALCHANNELSIGNALGENERATOR_H
