#ifndef STIMSIGNALSAMPLE_H
#define STIMSIGNALSAMPLE_H

#include <QObject>

class StimSignalSample : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalSample(int numChannels, qlonglong wholeTimestamp, qreal fractionalTimestamp, QObject *parent = nullptr);

    qreal totalTimestamp() const {return wholeTimestamp + fractionalTimestamp;}
    qreal distanceToTimestamp(qlonglong timestamp);

    char numberOfChannels(){return numChannels;}
    qreal getAmplitude(int channel) const;
    void setAmplitude(int channel, const qreal &value);
    qreal getPrimaryAmplitude() const;
    void setPrimaryAmplitude(const qreal &value);

    qreal getPhase(int channel) const;
    void setPhase(int channel, const qreal &value);
    qreal getPrimaryPhase() const;
    void setPrimaryPhase(const qreal &value);

    qreal value(int channel);
    qreal primaryValue();

    qint16 pcm(int channel);
    qint16 primaryPcm();

protected:
    qlonglong wholeTimestamp; //in ms
    qreal fractionalTimestamp; //also in ms (may be larger than 1)
    char numChannels;
    QList<qreal> amplitudes;
    QList<qreal> phases;
    qint16 realToPcm(qreal real);
};

#endif // STIMSIGNALSAMPLE_H
