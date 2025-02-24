#ifndef SEPARATELNRSIGNALGENERATOR_H
#define SEPARATELNRSIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class SeparateLnRSignalGenerator : public StimSignalGenerator
{
public:
    explicit SeparateLnRSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // SEPARATELNRSIGNALGENERATOR_H
