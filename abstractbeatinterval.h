#ifndef ABSTRACTBEATINTERVAL_H
#define ABSTRACTBEATINTERVAL_H

#include "beatvalue.h"
#include "globals.h"

class AbstractBeatInterval
{
public:
    AbstractBeatInterval();
    virtual ~AbstractBeatInterval();

    virtual float getLength() const = 0;

    bool matchesThisInterval(int otherInterval) const;
    bool matchesThisInterval(AbstractBeatInterval &otherInterval) const;
    bool matchesThisInterval(BeatValue & otherValue, double atTempo = BeatAnalysis::Configuration::currentBPM) const;

    void calculateValue() const;
    float deviationFromNearestKnownBeatValue() const;
    float absoluteDifferenceFromNearestKnownBeatValue() const;
    bool isKnownBeatValue(double atTempo = BeatAnalysis::Configuration::currentBPM) const;
    const BeatValue * getValue() const;

protected:
    mutable BeatValue const * value = NULL;
    const BeatValue * nearestValue(bool mustBeActive) const;
};

#endif // ABSTRACTBEATINTERVAL_H
