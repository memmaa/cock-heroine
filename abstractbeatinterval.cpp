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

bool AbstractBeatInterval::matchesThisInterval(BeatValue & otherValue) const
{
    return matchesThisInterval(roundToInt(otherValue.getLength()));
}

void AbstractBeatInterval::calculateValue() const
{
    if (beatValues.size() == 0)
    {
        qDebug() << "Error: Trying to calculate value of interval before beat values are initialised!";
        value = 0;
        return;
    }

    if (BeatAnalysis::Configuration::tempoEstablished == false)
    {
        qDebug() << "Error: Trying to calculate value of interval before tempo is calculated!";
        value = 0;
        return;
    }

    int i = 0;
    float valueDifference = 100.0;
    for (i = 0; i < beatValues.size(); ++i)
    {
        if (beatValues.at(i).active &&
                percentageDifferenceBetween(beatValues.at(i).value() * BeatAnalysis::Configuration::tempoInterval(),getLength()) < valueDifference)
        {
            valueDifference = percentageDifferenceBetween(beatValues.at(i).value() * BeatAnalysis::Configuration::tempoInterval(),getLength());
            value = &beatValues.at(i);
        }
    }
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

    return absoluteDifferenceBetween(value->getLength(), getLength());
}

bool AbstractBeatInterval::isKnownBeatValue() const
{
    bool acceptableProportion = (deviationFromNearestKnownBeatValue() <= BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
    if (!acceptableProportion)
        return false; //return early as access to 'value->' below may NPE in this case
    float absoluteDifference = absoluteDifferenceFromNearestKnownBeatValue();
    float partialBeatLength = BeatAnalysis::Configuration::tempoInterval() / (BeatAnalysis::Configuration::allowHalfBeatsInBreaks ? 4.0 : 2.0);
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
