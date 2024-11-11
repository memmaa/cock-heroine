#ifndef EVENTMETADATA_H
#define EVENTMETADATA_H

#include "beatpattern.h"

struct EventMetadata
{
    EventMetadata();
    float tempo;
    unsigned char valueNumerator;
    unsigned char valueDenominator;
    bool startsNewPattern;
    bool startsNewRound;
    bool endsRound;
    QString patternName;
    unsigned char patternIndex;
};

#endif // EVENTMETADATA_H
