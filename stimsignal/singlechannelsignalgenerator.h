#ifndef SINGLECHANNELSIGNALGENERATOR_H
#define SINGLECHANNELSIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class SingleChannelSignalGenerator : public StimSignalGenerator
{
public:
    explicit SingleChannelSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // SINGLECHANNELSIGNALGENERATOR_H
