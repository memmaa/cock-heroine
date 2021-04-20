#include <cstdio>

#include "beatvalue.h"
#include "globals.h"
#include "config.h"
#include "uniquebeatinterval.h"

float BeatValue::value() const
{
    return ((float)numerator / (float)denominator);
}

float BeatValue::getLength() const
{
    return value() * BeatAnalysis::Configuration::tempoInterval();
}
