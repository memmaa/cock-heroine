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
    qreal totalTimestamp() {return wholeTimestamp + fractionalTimestamp;}
    qreal primaryPhase;
    qreal secondaryPhase;
    qreal primaryAmplitude;
    qreal secondaryAmplitude;

    qreal primaryValue();
    qreal secondaryValue();

    qint16 primaryPcm();
    qint16 secondaryPcm();

    qreal distanceToTimestamp(qlonglong timestamp);

private:
    qint16 realToPcm(qreal real);

signals:

};

#endif // STIMSIGNALFRAME_H
