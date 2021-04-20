#include "helperfunctions.h"
#include <QModelIndexList>
#include <QDebug>
#include <cmath>
#include <QAbstractItemModel>
#include <QTableView>
#include "beatvalue.h"
#include "globals.h"
#include "event.h"

void setColumWidthsFromModel(QTableView * table)
{
    QAbstractItemModel * model = table->model();
    if (!model)
        return;
    for (int i = 0; i < model->columnCount(); ++i)
    {
        table->setColumnWidth(i,model->headerData(i,Qt::Horizontal,Qt::SizeHintRole).toSize().width());
    }
}

long int roundToInt(double numberToRound)
{
    if (numberToRound >= 0)
        numberToRound += 0.5;
    else
        numberToRound -= 0.5;
    return numberToRound;
}

bool isWholeNumber(float numberToCheck)
{
    float nearestWholeNumber = round(numberToCheck);

    return ( (nearestWholeNumber - 0.01) < numberToCheck &&
                                           numberToCheck < (nearestWholeNumber + 0.01));
}

float absoluteDifferenceBetween(double initialValue, double newValue)
{
    return abs(initialValue - newValue);
}

float percentageDifferenceBetween(double initialValue, double newValue)
{
    return ( absoluteDifferenceBetween(initialValue, newValue) * 100 ) / initialValue;
}

bool isWithinXPercentOf(double initialValue, double newValue, short percentageDifference)
{
    return ( percentageDifferenceBetween(initialValue,newValue) <= percentageDifference );
}

int isPowerOfTwo (unsigned int x)
{
  return ((x != 0) && !(x & (x - 1)));
}

bool multipleRowsSelected(const QModelIndexList & indexList)
{
    if (indexList.isEmpty())
        return false;

    int firstRowSelected = indexList[0].row();
    int i = 0;
    for (i = 0; i < indexList.size(); ++i)
    {
        if (indexList[i].row() != firstRowSelected)
        {
            return true;
        }
    }
    return false;
}

bool containsNonContiguousRows(const QModelIndexList & indexList)
{
    QVector<int> uniqueRows;
    for (int i = 0; i < indexList.size(); ++i) {
        if ( ! uniqueRows.contains(indexList[i].row()))
            uniqueRows.append(indexList[i].row());
    }
    qSort(uniqueRows);
    for (int i = 1; i < uniqueRows.size(); ++i) {
        if ((uniqueRows[i] - uniqueRows[i-1]) != 1)
            return true;
    }
    return false;
}

void identifyFirstAndLastRows(const QModelIndexList & indexList, int & result_first, int & result_last)
{
    if (indexList.isEmpty())
    {
        result_first = 0;
        result_last = 0;
        return;
    }
    else
    {
        result_first = indexList[0].row();
        result_last = indexList[0].row();
        for (int i = 1; i < indexList.size(); ++i) {
            if (indexList[i].row() < result_first)
                result_first = indexList[i].row();
            else if (indexList[i].row() > result_last)
                result_last = indexList[i].row();
        }
    }
}

BeatValue * calculateShortestActiveBeatValue()
{
    if (beatValues.isEmpty())
        return NULL;
    BeatValue * returnValue = NULL;
    bool foundValue = false;
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if ( ! beatValues[i].active)
            continue;

        if ( ! foundValue)
        {
            returnValue = &beatValues[i];
            foundValue = true;
        }
        else if (beatValues[i].value() < returnValue->value())
            returnValue = &beatValues[i];
    }
    return returnValue;
}

int greatestCommonDivisor(int a, int b)
{
    //lovingly lifted from http://stackoverflow.com/questions/4229870/c-algorithm-to-calculate-least-common-multiple-for-multiple-numbers
    for (;;)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int lowestCommonMultiple(int a, int b)
{
    //the SMALLEST common multiple of two numbers is guaranteed to be the two numbers
    //timesed by each other, and then DIVIDED by the BIGGEST number that divides
    //them both (greatest common divisor):
    //
    // return (a * b) / greatestCommonDivisor(a, b);

    //however, it's more efficient (by keeping the numbers smaller) to do the division first:
    // return ( a / greatestCommonDivisor(a, b) ) * b; //brackets for clarity, division happens first anyway
    //integer division is safe because (a / greatestCommonDivisor(a, b)) is always an integer.

    //avoid division by zero:
    int gcd = greatestCommonDivisor(a, b);

    return gcd ? (a / gcd * b) : 0;
}

int lowestCommonMultiple(const QVector<int> & listOfNumbers)
{
    //WARNING: THIS IS COPIED AND PASTED BELOW - DON'T LET THINGS GET OUT OF SYNC!!! Haven't I heard of templates?
    if (listOfNumbers.isEmpty())
    {
        qDebug() << "Empty list passed to lowestCommonMultiple function!";
        return 16 * 12; //some messy but relatively safe value
    }
    if (listOfNumbers.size() == 1)
        return listOfNumbers.at(0);

    int i = 2;
    int returnValue = lowestCommonMultiple(listOfNumbers.at(0),listOfNumbers.at(1));
    for (i = 2; i < listOfNumbers.size(); ++i)
        returnValue = lowestCommonMultiple(returnValue, listOfNumbers.at(i));
    return returnValue;
}


int lowestCommonMultiple(const QVector<unsigned char> & listOfNumbers)
{
    //WARNING: THIS IS COPIED AND PASTED FROM ABOVE - DON'T LET THINGS GET OUT OF SYNC!!! Haven't I heard of templates?
    if (listOfNumbers.isEmpty())
    {
        qDebug() << "Empty list passed to lowestCommonMultiple function!";
        return 16 * 12; //some messy but relatively safe value
    }
    if (listOfNumbers.size() == 1)
        return listOfNumbers.at(0);

    int i = 2;
    int returnValue = lowestCommonMultiple(listOfNumbers.at(0),listOfNumbers.at(1));
    for (i = 2; i < listOfNumbers.size(); ++i)
        returnValue = lowestCommonMultiple(returnValue, listOfNumbers.at(i));
    return returnValue;
}

bool midiCanUseTempo()
{
    bool lastEventOk = true;
    bool thisEventOk = false;
    for (int i = 0; i < events.size(); ++i)
    {
        thisEventOk = events[i].metadata != NULL &&
                events[i].metadata->tempo != 0 &&
                events[i].metadata->valueNumerator != 0;
        if (!thisEventOk && !lastEventOk)
            return false;
        lastEventOk = thisEventOk;
    }
    return true;
}

void reportTempoGaps()
{
    bool lastEventOk = true;
    bool thisEventOk = false;
    bool inTempoGap = false;
    for (int i = 0; i < events.size(); ++i)
    {
        thisEventOk = events[i].metadata != NULL &&
                events[i].metadata->tempo != 0 &&
                events[i].metadata->valueNumerator != 0;
        if (!thisEventOk && !lastEventOk && !inTempoGap)
        {
            qDebug() << "Tempo gap starts at " << millisToTimecode(events[i-1].timestamp);
            inTempoGap = true;
        }
        else if (inTempoGap && thisEventOk)
        {
            inTempoGap = false;
            qDebug() << "Tempo gap ends at " << millisToTimecode(events[i-1].timestamp);
        }
        lastEventOk = thisEventOk;
    }
}
