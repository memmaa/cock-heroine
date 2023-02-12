#include <cstdio>

#include "beatvalue.h"
#include "globals.h"
#include "config.h"
#include "uniquebeatinterval.h"
#include "helperfunctions.h"

float BeatValue::value() const
{
    return ((float)numerator / (float)denominator);
}

float BeatValue::getLength(double atTempo) const
{
    return value() * tempoToBeatLength(atTempo);
}

bool BeatValue::isBestEnabledMatchFor(int intervalInMs, double tempo)
{
    BeatValue * nearestValue = nullptr;
    double beatLength = tempoToBeatLength(tempo);
    double valueDifference = 100.0;
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if (beatValues.at(i).active &&
                percentageDifferenceBetween(intervalInMs, beatValues.at(i).value() * beatLength) < valueDifference)
        {
            valueDifference = percentageDifferenceBetween(intervalInMs, beatValues.at(i).value() * beatLength);
            nearestValue = &beatValues[i];
        }
    }
    return *nearestValue == *this;
}

bool BeatValue::isBestPossibleMatchFor(int intervalInMs, double tempo)
{
    BeatValue * nearestValue = nullptr;
    double beatLength = tempoToBeatLength(tempo);
    double valueDifference = 100.0;
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if (percentageDifferenceBetween(intervalInMs, beatValues.at(i).value() * beatLength) < valueDifference)
        {
            valueDifference = percentageDifferenceBetween(intervalInMs, beatValues.at(i).value() * beatLength);
            nearestValue = &beatValues[i];
        }
    }
    return *nearestValue == *this;
}

bool BeatValue::isPossibleMatchFor(int intervalInMs, double tempo)
{
    float deviationFromValue = percentageDifferenceBetween(getLength(tempo),intervalInMs);
    bool acceptableProportion = (deviationFromValue <= BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
    if (!acceptableProportion)
        return false; //return early as access to 'value->' below may NPE in this case
    double tempoInterval = tempoToBeatLength(tempo);
    float absoluteDifference = abs(getLength(tempo) - intervalInMs);
    float partialBeatLength = tempoInterval / (BeatAnalysis::Configuration::allowHalfBeatsInBreaks ? 4.0 : 2.0);
    bool acceptableError = absoluteDifference <= partialBeatLength;
    return acceptableError; // && acceptableProportion (we wouldn't be here otherwise)
}

bool BeatValue::operator==(const BeatValue & other)
{
    return numerator == other.numerator &&
            denominator == other.denominator &&
            name == other.name;
}
