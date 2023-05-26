#include "stereostimsignalsample.h"
#include <QtMath>

StereoStimSignalSample::StereoStimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent)
    :
      StimSignalSample(wholeTimestamp, fractionalTimestamp, parent),
      secondaryAmplitude(1),
      secondaryPhase(0)
{

}

qreal StereoStimSignalSample::getSecondaryPhase() const
{
    return secondaryPhase;
}

void StereoStimSignalSample::setSecondaryPhase(const qreal &value)
{
    secondaryPhase = value;
    secondaryPhase -= (int) secondaryPhase;
}

qreal StereoStimSignalSample::secondaryValue()
{
    return secondaryAmplitude * qSin(2 * M_PI * secondaryPhase);
}

qint16 StereoStimSignalSample::secondaryPcm()
{
    return realToPcm(secondaryValue());
}

qreal StereoStimSignalSample::getSecondaryAmplitude() const
{
    return secondaryAmplitude;
}

void StereoStimSignalSample::setSecondaryAmplitude(const qreal &value)
{
    secondaryAmplitude = value;
}
