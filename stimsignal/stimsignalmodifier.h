#ifndef STIMSIGNALMODIFIER_H
#define STIMSIGNALMODIFIER_H

#include <QObject>
#include "stimsignalsample.h"

class StimSignalModifier : public QObject
{
    Q_OBJECT
public:
    explicit StimSignalModifier(QObject *parent = nullptr);

    virtual void modify(StimSignalSample & sample) = 0;

signals:

};

#endif // STIMSIGNALMODIFIER_H
