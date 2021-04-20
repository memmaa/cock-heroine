#ifndef UNIQUEBEATINTERVAL_H
#define UNIQUEBEATINTERVAL_H

#include "abstractbeatinterval.h"

class UniqueBeatInterval : public AbstractBeatInterval
{
public:
    UniqueBeatInterval();
    UniqueBeatInterval(float len);
    UniqueBeatInterval(float len, short cnt);
    ~UniqueBeatInterval();

    float length;
    short count;

    int registerInterval(float interval);

    float getLength() const {return length;}
};

void reorderUniqueBeatIntervals();

#endif // UNIQUEBEATINTERVAL_H
