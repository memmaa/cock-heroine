#ifndef TRIPHASESIGNALGENERATOR_H
#define TRIPHASESIGNALGENERATOR_H

#include "stimsignalgenerator.h"

class TriphaseSignalGenerator : public StimSignalGenerator
{
public:
    explicit TriphaseSignalGenerator(QAudioFormat audioFormat, QObject *parent = nullptr);
protected:
    void setModifiers() override;
    long getStopTimestamp() override;
    StimSignalSample * createSample(qlonglong wholeTimestamp, qreal fractionalTimestamp) override;
};

#endif // TRIPHASESIGNALGENERATOR_H
