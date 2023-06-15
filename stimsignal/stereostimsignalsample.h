#ifndef STIMSIGNALFRAME_H
#define STIMSIGNALFRAME_H

#include "stimsignalsample.h"

class StereoStimSignalSample : public StimSignalSample
{
    Q_OBJECT
public:
    explicit StereoStimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent = nullptr);

    qreal secondaryValue();

    qint16 secondaryPcm();

    qreal getSecondaryAmplitude() const;
    void setSecondaryAmplitude(const qreal &value);

    qreal getSecondaryPhase() const;
    void setSecondaryPhase(const qreal &value);

signals:

};

#endif // STIMSIGNALFRAME_H
