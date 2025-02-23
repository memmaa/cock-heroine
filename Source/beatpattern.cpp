#include "beatpattern.h"
#include "globals.h"
#include "helperfunctions.h"
#include "beatanalysis.h"
#include "beatvalue.h"

BeatPattern::BeatPattern()
    :
      startsAt(-1),
      strokesCovered(0)
{
}

BeatPattern::BeatPattern(int startBeat)
    :
      startsAt(startBeat),
      strokesCovered(0)
{
}

void BeatPattern::clear()
{
    vector.clear();
}

void BeatPattern::append(const BeatValue * const &t)
{
    vector.append(t);
}

int BeatPattern::size() const
{
    return vector.size();
}

bool BeatPattern::isEmpty() const
{
    return vector.isEmpty();
}

void BeatPattern::remove(int i)
{
    vector.remove(i);
}

void BeatPattern::removeLast()
{
    vector.removeLast();
}

const BeatValue * const & BeatPattern::at(int i) const
{
    return vector.at(i);
}

const BeatValue *& BeatPattern::operator[](int i)
{
    return vector[i];
}

const BeatValue * BeatPattern::takeFirst()
{
    return vector.takeFirst();
}

void BeatPattern::resize(int i)
{
    vector.resize(i);
}

int BeatPattern::startsAtBeat() const
{
    return startsAt;
}

void BeatPattern::setStartBeat(int startBeat)
{
    startsAt = startBeat;
}

int BeatPattern::lengthInStrokes() const
{
    return size();
}

bool BeatPattern::containsUnknownIntervals() const
{
    int i = 0;
    for (i = 0; i < size(); ++i)
        if (vector[i] == NULL)
            return true;
    return false;
}

float BeatPattern::lengthInBeats() const
{
    if (isEmpty() || containsUnknownIntervals())
        return 0.0;
    int framesPerBeat = lowestCommonMultiple(listOfDenominators());
    int frameCount = 0;
    for (int i = 0; i < size(); ++i)
    {
        frameCount += vector[i] ? roundToInt(vector[i]->value() * framesPerBeat) : 1;
    }
    return (float) frameCount / (float) framesPerBeat;
}

float BeatPattern::lengthInMsAtTempo()
{
    return lengthInBeats() * BeatAnalysis::Configuration::tempoInterval();
}

void BeatPattern::reduceToShortestForm()
{
    int shortestForm = size();
    int i = size() - 1;
    for (; i > 0; --i)
    {
        BeatPattern shorterForm = *this;
        shorterForm.resize(i);
        if (shorterForm == *this)
            shortestForm = i;
    }
    vector.resize(shortestForm);
}

int BeatPattern::totalStrokesCovered() const
{
    if (strokesCovered == 0)
        return lengthInStrokes();
    else
        return strokesCovered;
}

void BeatPattern::setTotalStrokesCovered(int count)
{
    strokesCovered = count;
}

float BeatPattern::totalBeatsCovered() const
{
    if (containsUnknownIntervals())
        return 0.0;
    float returnValue = repetitions() * lengthInBeats();
    if (lengthInStrokes() > 1 && totalStrokesCovered() % lengthInStrokes() == lengthInStrokes() - 1)
        returnValue -= at(size() - 1)->value();

    return returnValue;
}

int BeatPattern::repetitions() const
{
    if (totalStrokesCovered() <= lengthInStrokes())
        return 1;
    if (lengthInStrokes() == 1)
        return totalStrokesCovered();
    int returnValue = totalStrokesCovered() / lengthInStrokes(); //deliberate integer division
    if (totalStrokesCovered() % lengthInStrokes() == lengthInStrokes() - 1)
        ++returnValue; //allow for 'almosts'
    return returnValue;
}

QVector<unsigned char> BeatPattern::listOfDenominators() const
{
    QVector<unsigned char> returnValue;
    int i = 0;
    for (i = 0; i < vector.size(); ++i)
    {
        if (vector[i] &&
                ! returnValue.contains(vector[i]->denominator))
            returnValue.append(vector[i]->denominator);
    }
    return returnValue;
}

QString BeatPattern::name() const
{
    if (totalStrokesCovered() == 1)
    {
        if (vector[0] && !(vector[0]->name.isNull()))
        {
            return vector[0]->name;
        }
        else
        {
            return QString("Unknown");
        }
    }

    QString returnValue;

    QString separatingString = "-";
    QString connectingString = "";
    QString interRepetitionalString = " - ";
    bool justListBeatValues = false;
    bool useExactNaming = false;
    bool useGlobalDenominators = false;
    bool collapseClusters = false;
    bool separateClusterNumbering = false;
    if (collapseClusters)
        separateClusterNumbering = true;
    bool omitTrailingSeparators = true;
    if (useExactNaming)
        omitTrailingSeparators = false;
    bool repeatShortPatternNames = true;
    unsigned char maxPatternLengthToRepeatName = 3;

    if (justListBeatValues)
    {
        int i = 0;
        for (i = 0; i < vector.size(); ++i)
        {
            if (vector[i])
                returnValue += vector[i]->name;
            else
                returnValue += "Unknown";
             if (i+1 != vector.size())
                returnValue += ", ";
        }
    }

    //for 'exact' naming, calculate lowest common multiple of all
    //values' denominators. Use this to work out how many 'frames' per beat.
    else if (useExactNaming)
    {
        int framesPerBeat = 1;
        if (useGlobalDenominators)
            framesPerBeat = lowestCommonMultiple(listOfTimestampDenominators());
        else
            framesPerBeat = lowestCommonMultiple(listOfDenominators());

        int i = 0, j = 0;
        for (i = 0; i < lengthInStrokes(); ++i)
        {
            QString numberAsString = QString::number(i+1);
            returnValue += vector[i] ? numberAsString : "?";
            int framesForThisBeat = vector[i] ? roundToInt(vector[i]->value() * framesPerBeat) : 1;
            for (j = framesForThisBeat - numberAsString.length(); j > 0; --j)
                returnValue += separatingString;
        }
    }
    else //use 'flexible' naming:
    {
        //calculate the shortest interval in the pattern
        unsigned char strokeCounter = 0;
        int i = 0;
        while (i < size() &&
               ! vector[i])
            ++i;
        if (i >= size())
            return "Multiple unknown values";
        const BeatValue * shortestBeat = at(i);
        for (/*leave 'i' as is*/; i < size(); ++i)
        {
            if (at(i) && at(i)->value() < shortestBeat->value())
                shortestBeat = at(i);
        }
        for (i = 0; i < size(); ++i)
        {
            returnValue += QString::number(++strokeCounter);

            if (collapseClusters && //'roll back' if this is part of a collapsed cluster
                    at(i) == shortestBeat &&
                    i+1 != size()) //not last stroke
                returnValue.chop(QString::number(strokeCounter).length());

            if (at(i) == shortestBeat && //print connecting string (if any) between members of non-collapsed cluster
                    i+1 != size() &&
                    ! collapseClusters)
                returnValue += connectingString;

            if (i+1 != size() && //print separating string between clusters, reset strokeCounter if appropriate
                   at(i)   != shortestBeat)
            {
                returnValue += separatingString;
                if (separateClusterNumbering)
                    strokeCounter = 0;
            }

            if (! omitTrailingSeparators && //print trailing separator if appropriate
                    i+1 == size() &&
                    at(i)   != shortestBeat)
                returnValue += separatingString;
        }

        if (repeatShortPatternNames &&
                size() <= maxPatternLengthToRepeatName)
        {
            QString copy = returnValue;
            returnValue += interRepetitionalString + copy;
        }
    }

    return returnValue;
}

bool BeatPattern::operator==(const BeatPattern & other) const
{
    int longestPatternLength = size() > other.size() ?
                               size() :
                               other.size();
    int i = 0, j = 0, k = 0;
    for (i = 0; i < longestPatternLength; ++i)
    {
        if (at(j) != other.at(k))
            return false;
        ++j;
        ++k;
        j %= size();
        k %= other.size();
    }
    if (j || k) //one of the patterns didn't loop fully
        return false;
    else
        return true;
}

bool BeatPattern::cyclesTheSameAs(const BeatPattern & other) const
{
    BeatPattern cycledPattern = other;
    int i = 0;
    for (i = 0; i < cycledPattern.size(); ++i)
    {
        if (*this == cycledPattern)
            return true;
        cycledPattern.append(cycledPattern.takeFirst());
    }
    return false;
}

BeatPattern * BeatPattern::byName(QString name)
{
    for (int i = 0; i < beatPatterns.size(); ++i)
    {
        if (beatPatterns[i].name() == name)
            return &beatPatterns[i];
    }
    return NULL;
}
