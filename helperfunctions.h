#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QModelIndexList>
class QTableView;
class BeatValue;

bool isWholeNumber(float numberToCheck);
long int roundToInt(double numberToRound);
float absoluteDifferenceBetween(const double initialValue, const double newValue);
float percentageDifferenceBetween(const double initialValue, const double newValue);
bool isWithinXPercentOf(const double initialValue, const double newValue, const short percentageDifference);
int isPowerOfTwo (unsigned int x);

int greatestCommonDivisor(int a, int b);
int lowestCommonMultiple(int a, int b);
int lowestCommonMultiple(const QVector<int> & listOfNumbers);
int lowestCommonMultiple(const QVector<unsigned char> & listOfNumbers);

void setColumWidthsFromModel(QTableView * table);
bool multipleRowsSelected(const QModelIndexList & indexList);
bool containsNonContiguousRows(const QModelIndexList & indexList);
void identifyFirstAndLastRows(const QModelIndexList & indexList, int & result_first, int & result_last);
BeatValue * calculateShortestActiveBeatValue();

bool midiCanUseTempo();
void reportTempoGaps();

#endif // HELPERFUNCTIONS_H
