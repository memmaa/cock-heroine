#include "abstractbeatinterval.h"
#include "helperfunctions.h"
#include <QDebug>
#include <float.h>
#include "config.h"
#include "globals.h"

AbstractBeatInterval::AbstractBeatInterval()
{
}

AbstractBeatInterval::~AbstractBeatInterval()
{
}

bool AbstractBeatInterval::matchesThisInterval(int otherInterval) const
{
    return isWithinXPercentOf(getLength(),otherInterval,BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
}

bool AbstractBeatInterval::matchesThisInterval(AbstractBeatInterval &otherInterval) const
{
    return matchesThisInterval(otherInterval.getLength());
}

bool AbstractBeatInterval::matchesThisInterval(BeatValue & otherValue, double atTempo) const
{
    return matchesThisInterval(roundToInt(otherValue.getLength(atTempo)));
}

void AbstractBeatInterval::calculateValue() const
{
    if (beatValues.size() == 0)
    {
        qDebug() << "Warning: Trying to calculate value of interval before beat values are initialised!";
        value = 0;
        return;
    }

    if (BeatAnalysis::Configuration::tempoEstablished == false)
    {
        qDebug() << "Warning: Trying to calculate value of interval before tempo is calculated!";
        value = 0;
        return;
    }

    value = nearestValue(true);
}

float AbstractBeatInterval::deviationFromNearestKnownBeatValue() const
{
    if ( ! value )
        calculateValue();

    if ( ! value )
        return FLT_MAX;

    return percentageDifferenceBetween(value->getLength(),getLength());
}

float AbstractBeatInterval::absoluteDifferenceFromNearestKnownBeatValue() const
{
    if ( ! value )
        calculateValue();

    if ( ! value )
        return FLT_MAX;

    return absoluteDifferenceBetween(getLength(), value->getLength());
}

bool AbstractBeatInterval::isKnownBeatValue(double tempo) const
{
    bool acceptableProportion = (deviationFromNearestKnownBeatValue() <= BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
    if (!acceptableProportion)
        return false; //return early as access to 'value->' below may NPE in this case
    double tempoInterval = tempoToBeatLength(tempo);
    float absoluteDifference = absoluteDifferenceFromNearestKnownBeatValue();
    float partialBeatLength = tempoInterval / (BeatAnalysis::Configuration::allowHalfBeatsInBreaks ? 4.0 : 2.0);
    bool acceptableError = absoluteDifference <= partialBeatLength;
    return acceptableError; // && acceptableProportion (we wouldn't be here otherwise)
}

BeatValue const * AbstractBeatInterval::getValue() const
{
    if (isKnownBeatValue())
        return value;
    else
        return NULL;
}

const BeatValue *AbstractBeatInterval::nearestValue(bool mustBeActive) const
{
    float valueDifference = 100.0;
    const BeatValue * nearestValue = nullptr;
    if (BeatAnalysis::Configuration::tempoEstablished == false)
    {
        return nearestValue;
    }
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if ((beatValues.at(i).active || !mustBeActive) &&
                percentageDifferenceBetween(getLength(), beatValues.at(i).value() * BeatAnalysis::Configuration::tempoInterval()) < valueDifference)
        {
            valueDifference = percentageDifferenceBetween(getLength(), beatValues.at(i).value() * BeatAnalysis::Configuration::tempoInterval());
            nearestValue = &beatValues.at(i);
        }
    }
    return nearestValue;
}
