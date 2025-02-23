#ifndef MONOSTIMSIGNALSAMPLE_H
#define MONOSTIMSIGNALSAMPLE_H

#include "stimsignalsample.h"

class MonoStimSignalSample : public StimSignalSample
{
public:
    explicit MonoStimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent = nullptr);
};

#endif // MONOSTIMSIGNALSAMPLE_H
