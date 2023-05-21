#ifndef STIMSIGNALFRAME_H
#define STIMSIGNALFRAME_H

#include <QObject>

class StimSignalSample : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalSample(qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent = nullptr);

//private:
    qlonglong wholeTimestamp; //in ms
    qreal fractionalTimestamp; //also in ms (may be larger than 1)
    qreal totalTimestamp() const {return wholeTimestamp + fractionalTimestamp;}
    qreal primaryAmplitude;
    qreal secondaryAmplitude;

    qreal primaryValue();
    qreal secondaryValue();

    qint16 primaryPcm();
    qint16 secondaryPcm();

    qreal distanceToTimestamp(qlonglong timestamp);

    qreal getPrimaryPhase() const;
    void setPrimaryPhase(const qreal &value);

    qreal getSecondaryPhase() const;
    void setSecondaryPhase(const qreal &value);

private:
    qreal primaryPhase;
    qreal secondaryPhase;
    qint16 realToPcm(qreal real);

signals:

};

#endif // STIMSIGNALFRAME_H
