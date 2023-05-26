#ifndef STIMSIGNALSAMPLE_H
#define STIMSIGNALSAMPLE_H

#include <QObject>

class StimSignalSample : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent = nullptr);

    qreal totalTimestamp() const {return wholeTimestamp + fractionalTimestamp;}

    qreal primaryValue();

    qint16 primaryPcm();

    qreal distanceToTimestamp(qlonglong timestamp);

    qreal getPrimaryAmplitude() const;
    void setPrimaryAmplitude(const qreal &value);

    qreal getPrimaryPhase() const;
    void setPrimaryPhase(const qreal &value);

protected:
    qlonglong wholeTimestamp; //in ms
    qreal fractionalTimestamp; //also in ms (may be larger than 1)
    qreal primaryAmplitude;
    qreal primaryPhase;
    qint16 realToPcm(qreal real);
};

#endif // STIMSIGNALSAMPLE_H
