#ifndef BEATINTERVAL_H
#define BEATINTERVAL_H

#include "abstractbeatinterval.h"
#include <QVector>

class BeatTimestamp;

class BeatInterval : public AbstractBeatInterval
{
public:
    BeatInterval();
    BeatInterval(int index);
    ~BeatInterval();

    BeatInterval * prev = NULL;
    BeatInterval * next = NULL;

    int index;
    int startsAtTimestamp();
    int endsAtTimestamp();

    float getLength() const;
    int getIntLength() const;
};

#endif // BEATINTERVAL_H
