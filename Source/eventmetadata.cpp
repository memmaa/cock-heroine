#include "eventmetadata.h"

EventMetadata::EventMetadata()
    :
    tempo(0),
    valueNumerator(0),
    valueDenominator(0),
    startsNewPattern(false),
    startsNewRound(false),
    endsRound(false),
    patternName(QString()),
    patternIndex(0)
{
    //just the LIST above...
}
