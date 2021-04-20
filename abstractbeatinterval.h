#ifndef ABSTRACTBEATINTERVAL_H
#define ABSTRACTBEATINTERVAL_H

#include "beatvalue.h"

class AbstractBeatInterval
{
public:
    AbstractBeatInterval();
    virtual ~AbstractBeatInterval();

    virtual float getLength() const = 0;

    bool matchesThisInterval(int otherInterval) const;
    bool matchesThisInterval(AbstractBeatInterval &otherInterval) const;
    bool matchesThisInterval(BeatValue & otherValue) const;

    void calculateValue() const;
    float deviationFromNearestKnownBeatValue() const;
    float absoluteDifferenceFromNearestKnownBeatValue() const;
    bool isKnownBeatValue() const;
    const BeatValue * getValue() const;

protected:
    mutable BeatValue const * value = NULL;
};

#endif // ABSTRACTBEATINTERVAL_H
