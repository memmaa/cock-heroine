#include "stimsignalsample.h"
#include <QtMath>

StimSignalSample::StimSignalSample(int numChannels, qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent)
    :
      QObject(parent),
      wholeTimestamp(wholeTimestamp),
      fractionalTimestamp(fractionalTimestamp),
      numChannels(numChannels)
{
    for (int i = 0; i < numChannels; ++i) {
        amplitudes.append(1);
        phases.append(0);
    }
}

//!
//! \brief StimSignalSample::distanceToTimestamp
//! \param timestamp the timestamp (in ms) we're working the distance to
//! \return the number of milliseconds to the given timestamp. Positive if it's in the future. Negative if the given timestamp is before this one.
//!
qreal StimSignalSample::distanceToTimestamp(qlonglong timestamp)
{
    qlonglong wholeDistance = timestamp - wholeTimestamp;
    return wholeDistance - fractionalTimestamp;
}

qreal StimSignalSample::getAmplitude(int channel) const
{
    return amplitudes[channel];
}

void StimSignalSample::setAmplitude(int channel, const qreal &value)
{
    amplitudes[channel] = value;
}

qreal StimSignalSample::getPrimaryAmplitude() const
{
    return getAmplitude(0);
}

void StimSignalSample::setPrimaryAmplitude(const qreal &value)
{
    setAmplitude(0, value);
}

qreal StimSignalSample::getPhase(int channel) const
{
    return phases[channel];
}

void StimSignalSample::setPhase(int channel, const qreal &value)
{
    phases[channel] = value;
    phases[channel] -= (int) phases[channel];
}

qreal StimSignalSample::getPrimaryPhase() const
{
    return getPhase(0);
}

void StimSignalSample::setPrimaryPhase(const qreal &value)
{
    setPhase(0, value);
}

qreal StimSignalSample::value(int channel)
{
    return amplitudes[channel] * qSin(2 * M_PI * phases[channel]);
}

qreal StimSignalSample::primaryValue()
{
    return value(0);
}

qint16 StimSignalSample::pcm(int channel)
{
    return realToPcm(value(channel));
}

qint16 StimSignalSample::primaryPcm()
{
    return pcm(0);
}

const qint16  PCMS16MaxValue     =  32767;
//const quint16 PCMS16MaxAmplitude =  32768; // because minimum is -32768

qint16 StimSignalSample::realToPcm(qreal real)
{
    return real * PCMS16MaxValue;
}
