#ifndef BEATPATTERN_H
#define BEATPATTERN_H

#include <QVector>
#include "beatvalue.h"

class BeatPattern
{
public:
    BeatPattern();
    BeatPattern(int startBeat);

    //all forward to underlying vector
    void clear();
    void append(const BeatValue * const &t);
    int size() const;
    bool isEmpty() const;
    void remove(int i);
    void removeLast();
    const BeatValue * const & at(int i) const;
    const BeatValue *& operator[](int i);
    const BeatValue * takeFirst();
    void resize (int i);

    int startsAtBeat() const;
    void setStartBeat(int );

    int lengthInStrokes() const;
    bool containsUnknownIntervals() const;
    float lengthInBeats() const;
    float lengthInMsAtTempo();

    void reduceToShortestForm();

    int totalStrokesCovered() const;
    void setTotalStrokesCovered(int count);
    float totalBeatsCovered() const;
    int repetitions() const;
    int framesRequiredPerBeat() const;

    QVector<unsigned char> listOfDenominators() const;

    QString name() const;

    bool operator==(const BeatPattern & other) const;
    bool cyclesTheSameAs(const BeatPattern &other) const;

    static BeatPattern * byName(QString name);

private:
    QVector<const BeatValue *> vector;
    int startsAt;
    int strokesCovered;
};

#endif // BEATPATTERN_H
