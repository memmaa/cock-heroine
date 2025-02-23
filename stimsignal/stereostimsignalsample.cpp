#include "stereostimsignalsample.h"
#include <QtMath>

StereoStimSignalSample::StereoStimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent)
    :
      StimSignalSample(2, wholeTimestamp, fractionalTimestamp, parent)
{

}

qreal StereoStimSignalSample::getSecondaryPhase() const
{
    return getPhase(1);
}

void StereoStimSignalSample::setSecondaryPhase(const qreal &value)
{
    setPhase(1, value);
}

qreal StereoStimSignalSample::secondaryValue()
{
    return value(1);
}

qint16 StereoStimSignalSample::secondaryPcm()
{
    return realToPcm(secondaryValue());
}

qreal StereoStimSignalSample::getSecondaryAmplitude() const
{
    return getAmplitude(1);
}

void StereoStimSignalSample::setSecondaryAmplitude(const qreal &value)
{
    setAmplitude(1, value);
}
