#include "monostimsignalsample.h"

MonoStimSignalSample::MonoStimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent)
    :
      StimSignalSample(1, wholeTimestamp, fractionalTimestamp, parent)
{

}
