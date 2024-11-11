#include "beatanalysis.h"
#include "globals.h"
#include "editorwindow.h"
#include <QString>
#include "uniquebeatinterval.h"
#include "beatinterval.h"
#include "beatpattern.h"
#include "helperfunctions.h"
#include <cmath> //for fabs()
#include "valuedatamodel.h"

unsigned char BeatAnalysis::Configuration::maxUniqueIntervalMergeCycles = 3;
float BeatAnalysis::Configuration::maxPercentAcceptableBeatError = 20;
float BeatAnalysis::Configuration::badMatchThreshhold = 40;
bool BeatAnalysis::Configuration::allowTripletValues = false;
float BeatAnalysis::Configuration::minNumberOfBeatsToQualifyAsBreak = 8.25;
bool BeatAnalysis::Configuration::allowHalfBeatsInBreaks = false;
float BeatAnalysis::Configuration::maxPermissibleProportionUnmatchedBeats = (2.0/32.0);

bool BeatAnalysis::Configuration::inputTempoProvided = false;
double BeatAnalysis::Configuration::providedInputTempo = 0;
unsigned short BeatAnalysis::Configuration::maxExpectedBPM = 160;
unsigned short BeatAnalysis::Configuration::minExpectedBPM = 80;
bool BeatAnalysis::Configuration::tempoEstablished = false;
double BeatAnalysis::Configuration::currentBPM = 0;
unsigned short BeatAnalysis::Configuration::minExpectedTempoInterval() {return (60 * 1000) / maxExpectedBPM;}
unsigned short BeatAnalysis::Configuration::maxExpectedTempoInterval() {return (60 * 1000) / minExpectedBPM;}
double BeatAnalysis::Configuration::tempoInterval() {return (60 * 1000) / currentBPM;}
void BeatAnalysis::Configuration::setTempoFromInterval(double interval) { currentBPM = (60 * 1000) / interval;}
void BeatAnalysis::Configuration::setTempoFromInterval(const AbstractBeatInterval & interval) { currentBPM = (60 * 1000) / interval.getLength();}
void BeatAnalysis::Configuration::expectSlowTempo() {minExpectedBPM = 40; maxExpectedBPM = 80;}
void BeatAnalysis::Configuration::expectNormalTempo() {minExpectedBPM = 80; maxExpectedBPM = 160;}
void BeatAnalysis::Configuration::expectFastTempo() {minExpectedBPM = 160; maxExpectedBPM = 320;}

bool BeatAnalysis::Configuration::matchPatternMembersByNearestKnownValue = true;
bool BeatAnalysis::Configuration::matchPatternMembersByActualValue = true;
unsigned char BeatAnalysis::Configuration::maxPatternLengthInStrokes = 9;
bool BeatAnalysis::Configuration::allowOddPatternLengths = false;
bool BeatAnalysis::Configuration::preferLongPatterns = true;
unsigned char BeatAnalysis::Configuration::minLongPatternLengthInStrokes = 5;
unsigned char BeatAnalysis::Configuration::minLongPatternRepetitionsToPrefer = 3;

bool BeatAnalysis::calculateUniqueBeatIntervals()
{
    int i = 0, j = 0, k = 0;

    char mergeIterations = 0;
    bool changesMade = false;
    do
    {
        //initially each beat interval is identified by the first encountered interval of its kind, which may not be representitive.
        //average all intervals of each unique type and rescan to see whether this brings new intervals 'into range',
        //or identifies outliers which were previously included in the interval type.
        for (i = 0; i < uniqueBeatIntervals.size(); ++i)
        {
            int intervalCount = 0;
            double intervalTimeTotal = 0;
            for (j = 0; j < beatIntervals.size(); ++j)
            {
                if (uniqueBeatIntervals[i].matchesThisInterval(beatIntervals[j]))
                {
                    ++intervalCount;
                    intervalTimeTotal += beatIntervals[j].getLength();
                }
            }
            if (intervalCount)
            {
                uniqueBeatIntervals[i].count = intervalCount;
                uniqueBeatIntervals[i].length = intervalTimeTotal / intervalCount;
            }
            else
            {
                editor->outputToConsole("Error calculating unique beat intervals.",true);
                editor->outputToConsole("Interval length: " + QString::number(uniqueBeatIntervals[i].length),true);
                return false;
            }
        }

        //Sort unique intervals by decreasing frequency of occurrence
        reorderUniqueBeatIntervals();

        //for each unique interval between beats
        for (i = 0; i < uniqueBeatIntervals.size(); ++i)
        {
            //print how many of those intervals there are
            editor->outputToConsole(QString("~%1ms: %2.").arg(uniqueBeatIntervals[i].length,4,'f',0).arg(uniqueBeatIntervals[i].count,3),true);
        }

        if (changesMade)
        {
            editor->outputToConsole("Optimising...\n",true);
            changesMade = false;
        }

        //merge any 'unique' intervals that are now too close
        bool mergesMade = false;
        do
        {
            mergesMade = false;
            for (i = 0; i < (uniqueBeatIntervals.size()-1); ++i)
            {
                for (j = i + 1; j < uniqueBeatIntervals.size(); ++j)
                {
                    if (uniqueBeatIntervals[i].matchesThisInterval(uniqueBeatIntervals[j]))
                    {
                        editor->outputToConsole(QString("Merging similar intervals %1ms and %2ms ").arg(uniqueBeatIntervals[i].length,0,'f',2).arg(uniqueBeatIntervals[j].length,0,'f',2));
                        //average all the intervals that are of either of the types to be merged
                        int intervalCount = 0;
                        double intervalTimeTotal = 0;
                        for (k = 0; k < beatIntervals.size(); ++k)
                        {
                            if ( uniqueBeatIntervals[i].matchesThisInterval(beatIntervals[k]) ||
                                 uniqueBeatIntervals[j].matchesThisInterval(beatIntervals[k]) )
                            {
                                ++intervalCount;
                                intervalTimeTotal += beatIntervals[k].getLength();
                            }
                        }

                        //int hybridInterval = intervalTimeTotal / intervalCount;
                        UniqueBeatInterval hybridInterval(intervalTimeTotal / intervalCount,intervalCount);

                        ////we now have the average of the 2 intervals to be merged
                        ////delete the intervals to be merged, and create a new hybrid interval
                        //uniqueBeatIntervals.removeAt(uniqueBeatIntervals.indexOf(orderedUniqueBeatIntervals[i]));
                        //uniqueBeatIntervals.removeAt(uniqueBeatIntervals.indexOf(orderedUniqueBeatIntervals[j]));
                        //uniqueBeatIntervals.append(hybridInterval);
                        //orderedUniqueBeatIntervals.remove(j);
                        //orderedUniqueBeatIntervals.remove(i);
                        //orderedUniqueBeatIntervals.insert(i+1,hybridInterval);
                        //printf("to %ims.\n",hybridInterval);

                        //we now have the average of the 2 intervals to be merged
                        //delete the intervals to be merged, and insert the new hybrid interval
                        uniqueBeatIntervals.removeAt(j);
                        uniqueBeatIntervals.removeAt(i);
                        uniqueBeatIntervals.insert(i+1,hybridInterval); //can this give index out of range?
                        editor->outputToConsole(QString("to %2ms.\n").arg(hybridInterval.length,0,'f',2));

                        mergesMade = true;
                        changesMade = true;
                        break;
                    }
                }
                if (mergesMade)
                    break;
            }
        }
        while (mergesMade);

        //for each interval between beats
        for (i = 0; i < beatIntervals.size(); ++i)
        {
            //check if this is the first time we've encountered an interval like this
            bool beatIsUnique = true;
            for (j = 0; j < uniqueBeatIntervals.size() && beatIsUnique; ++j)
            {
                if (uniqueBeatIntervals[j].matchesThisInterval(beatIntervals[i]))
                {
                    beatIsUnique = false;
                    break;
                }
            }
            //and if so, add it to our LIST of beat intervals
            if (beatIsUnique)
            {
                float length = beatIntervals[i].getLength();
                if (length == 0)
                {
                    editor->outputToConsole(QString("Zero-length interval at index %1.").arg(i), true);
                    editor->outputToConsole(QString("around Timestamp %1\n").arg(beatIntervals[i].startsAtTimestamp()), true);
                }
                uniqueBeatIntervals.append(UniqueBeatInterval(length));
                changesMade = true;
            }
        }
        editor->outputToConsole(QString("%1 unique intervals:").arg(uniqueBeatIntervals.size()));
    }
    while (changesMade && (++mergeIterations < BeatAnalysis::Configuration::maxUniqueIntervalMergeCycles) );
    return true;
}

bool BeatAnalysis::calculateTempo()
{
    reorderUniqueBeatIntervals();
    Configuration::currentBPM = 0;

    if (Configuration::inputTempoProvided) //non-zero BPM has been provided
    {
        Configuration::currentBPM = Configuration::providedInputTempo;
        editor->outputToConsole(QString("\t  (Using provided tempo: %1 BPM)\n").arg(Configuration::currentBPM,0,'f',0));
        Configuration::tempoEstablished = true;
        return Configuration::tempoEstablished;
    }
    else
    {
        for (int i = 0; i < uniqueBeatIntervals.size(); ++i)
        {
            if (Configuration::minExpectedTempoInterval() <= uniqueBeatIntervals[i].length &&
                    uniqueBeatIntervals[i].length <= Configuration::maxExpectedTempoInterval() )
            {
                Configuration::setTempoFromInterval(uniqueBeatIntervals[i]);
                Configuration::tempoEstablished = true;
                break;
            }
        }
    }

    if (Configuration::tempoEstablished == false) //BPM wasn't provided, and doesn't fall within expected range
    {
        editor->outputToConsole("Unable to determine main tempo!\n");
    }
    else
    {
        editor->outputToConsole(QString("\t     (Established tempo: %1 BPM)\n").arg(Configuration::currentBPM,0,'f',2));
    }
    return Configuration::tempoEstablished;
}

void BeatAnalysis::initialiseBeatValues()
{
    //This uses the 'British' musical values. If you want to add American, or other languages, you could change it here.
    beatValues.append(BeatValue(SEMIQUAVER_NUMERATOR,SEMIQUAVER_DENOMINATOR,QString("Semiquaver"),false,true));
    if (Configuration::allowTripletValues)
        beatValues.append(BeatValue(QUAVER_TRIPLET_NUMERATOR,QUAVER_TRIPLET_DENOMINATOR,QString("Quaver Triplet"),true,true));
    beatValues.append(BeatValue(QUAVER_NUMERATOR,QUAVER_DENOMINATOR,QString("Quaver"),true,true));
    if (Configuration::allowTripletValues)
        beatValues.append(BeatValue(CROCHET_TRIPLET_NUMERATOR,CROCHET_TRIPLET_DENOMINATOR,QString("Crochet Triplet"),true,true));
    beatValues.append(BeatValue(THREE_SEMIQUAVERS_NUMERATOR,THREE_SEMIQUAVERS_DENOMINATOR,QString("Three Semiquavers"),false,true));
    beatValues.append(BeatValue(CROCHET_NUMERATOR,CROCHET_DENOMINATOR,QString("Crochet"),true,true));
    beatValues.append(BeatValue(FIVE_SEMIQUAVERS_NUMERATOR,FIVE_SEMIQUAVERS_DENOMINATOR,QString("Five Semiquavers"),false,true));
    if (Configuration::allowTripletValues)
        beatValues.append(BeatValue(TWO_CROCHET_TRIPLETS_NUMERATOR,TWO_CROCHET_TRIPLETS_DENOMINATOR,QString("Two Crochet Triplets"),true,true));
    beatValues.append(BeatValue(THREE_QUAVERS_NUMERATOR,THREE_QUAVERS_DENOMINATOR,QString("Three Quavers"),true,true));
    beatValues.append(BeatValue(SEVEN_SEMIQUAVERS_NUMERATOR,SEVEN_SEMIQUAVERS_DENOMINATOR,QString("Seven Semiquavers"),false,true));
    beatValues.append(BeatValue(TWO_BEATS_NUMERATOR,TWO_BEATS_DENOMINATOR,QString("Two Beats"),true,true));
    beatValues.append(BeatValue(FIVE_QUAVERS_NUMERATOR,FIVE_QUAVERS_DENOMINATOR,QString("Five Quavers"),false,true));
    beatValues.append(BeatValue(THREE_BEATS_NUMERATOR,THREE_BEATS_DENOMINATOR,QString("Three Beats"),true,true));
    beatValues.append(BeatValue(SEVEN_QUAVERS_NUMERATOR,SEVEN_QUAVERS_DENOMINATOR,QString("Seven Quavers"),false,true));
    beatValues.append(BeatValue(FIFTEEN_SEMIQUAVERS_NUMERATOR,FIFTEEN_SEMIQUAVERS_DENOMINATOR,QString("Fifteen Semiquavers"),false,true));
    beatValues.append(BeatValue(FOUR_BEATS_NUMERATOR,FOUR_BEATS_DENOMINATOR,QString("Four Beats"),true,true));
    beatValues.append(BeatValue(SEVENTEEN_SEMIQUAVERS_NUMERATOR,SEVENTEEN_SEMIQUAVERS_DENOMINATOR,QString("Seventeen Semiquavers"),false,true));
    beatValues.append(BeatValue(NINE_QUAVERS_NUMERATOR,NINE_QUAVERS_DENOMINATOR,QString("Nine Quavers"),false,true));
    beatValues.append(BeatValue(FIVE_BEATS_NUMERATOR,FIVE_BEATS_DENOMINATOR,QString("Five Beats"),true,true));
    beatValues.append(BeatValue(FIVE_AND_A_HALF_BEATS_NUMERATOR,FIVE_AND_A_HALF_BEATS_DENOMINATOR,QString("Five-and-a-Half Beats"),true,true));
    beatValues.append(BeatValue(SIX_BEATS_NUMERATOR,SIX_BEATS_DENOMINATOR,QString("Six Beats"),true,true));
    beatValues.append(BeatValue(SEVEN_BEATS_NUMERATOR,SEVEN_BEATS_DENOMINATOR,QString("Seven Beats"),true,true));
    beatValues.append(BeatValue(EIGHT_BEATS_NUMERATOR,EIGHT_BEATS_DENOMINATOR,QString("Eight Beats"),true,true));
    //replaced by 'break value calculation':
    //beatValues.append(BeatValue(TWELVE_BEATS_NUMERATOR,TWELVE_BEATS_DENOMINATOR,QString("Twelve Beats")));
    //beatValues.append(BeatValue(SIXTEEN_BEATS_NUMERATOR,SIXTEEN_BEATS_DENOMINATOR,QString("Sixteen Beats")));
    //beatValues.append(BeatValue(TWENTY_FOUR_BEATS_NUMERATOR,TWENTY_FOUR_BEATS_DENOMINATOR,QString("Twenty Four Beats")));
    //beatValues.append(BeatValue(THIRTY_TWO_BEATS_NUMERATOR,THIRTY_TWO_BEATS_DENOMINATOR,QString("Thirty Two Beats")));
}

void BeatAnalysis::calculateBreakValues()
{
    //for each beat interval
    for (int i = 0; i < beatIntervals.size(); ++i)
    {
        //calculate if it's long enough to classify as a break
        if (beatIntervals[i].getLength() > (Configuration::minNumberOfBeatsToQualifyAsBreak * Configuration::tempoInterval()))
        {
            float numberOfBeats = beatIntervals[i].getLength() / Configuration::tempoInterval();
            QString intervalName;
            //calculate nearest whole/half number of beats
            if (Configuration::allowHalfBeatsInBreaks)
            {
                numberOfBeats *= 2;
                numberOfBeats = qRound(numberOfBeats);
                numberOfBeats /= 2;
                intervalName.sprintf("%.1f-Beat Break",numberOfBeats);
            }
            else
            {
                numberOfBeats = qRound(numberOfBeats);
                intervalName.sprintf("%.0f-Beat Break",numberOfBeats);
            }

            int num, den = 1;
            if (! isWholeNumber(numberOfBeats))
            {
                numberOfBeats *= 2;
                den *= 2;
            }
            num = roundToInt(numberOfBeats);

            //Possibly filter out unlikely break lengths here...

            BeatValue valueToAdd(num,den,intervalName);
            bool alreadyExists = false;
            for (int j = 0; j < beatValues.size(); ++j)
            {
                if (valueToAdd.value() == beatValues[j].value())
                    alreadyExists = true;
            }
            if (alreadyExists == false)
                editor->valueModel->addValue(valueToAdd);
        }
    }
}

void BeatAnalysis::reorderUniqueBeatIntervals()
{
    int biggestNumberOfOccurrences = 0;
    int i = 0, j = 0;

    for (i = 0; i < uniqueBeatIntervals.size(); ++i)
    {
        if (uniqueBeatIntervals.at(i).count > biggestNumberOfOccurrences)
            biggestNumberOfOccurrences = uniqueBeatIntervals.at(i).count;
    }

    QList<UniqueBeatInterval> itemsToMove;
    for (i = biggestNumberOfOccurrences; i > 0; --i)
    {
        for (j = 0; j < uniqueBeatIntervals.size(); ++j)
        {
            if (uniqueBeatIntervals.at(j).count == i)
            {
                itemsToMove.append(uniqueBeatIntervals.takeAt(j));
                --j;
            }
        }
        if (itemsToMove.size() > 0)
        {
            uniqueBeatIntervals.append(itemsToMove);
            itemsToMove.clear();
        }
    }
}

bool BeatAnalysis::analyseUniqueBeatIntervals()
{
    //cycle through uniqueBeatIntervals and try to establish note 'value' in relation to main BPM.
    int i = 0;
    for (i = 0; i < uniqueBeatIntervals.size(); ++i)
    {
        uniqueBeatIntervals[i].calculateValue();
        if (uniqueBeatIntervals[i].isKnownBeatValue())
        {
            editor->outputToConsole(QString("~%1ms = %2 %3% confidence.\n")
                                    .arg(uniqueBeatIntervals[i].length,4,'f',2)
                                    .arg(uniqueBeatIntervals[i].getValue()->name,-20)
                                    .arg(100 - (uniqueBeatIntervals[i].deviationFromNearestKnownBeatValue() * (100.0 / BeatAnalysis::Configuration::maxPercentAcceptableBeatError) ),2,'f',0));
        }
        else
        {
            editor->outputToConsole(QString("~%1ms = ?                    Unrecognised interval!\n").arg(uniqueBeatIntervals[i].length,4,'f',2));
        }
    }
    //fail if too many beat values are unidentifiable.
    short totalUnidentifiableBeatIntervals = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
    {
        if (beatIntervals[i].isKnownBeatValue() == false)
        {
            ++totalUnidentifiableBeatIntervals;
        }
    }
    float percentageBeatIntervalsUnidentifiable = (float)totalUnidentifiableBeatIntervals / (float)beatIntervals.size() * 100;
    editor->outputToConsole(QString("%1 of %2 beat intervals (%3%) unmatched.\n")
                            .arg(totalUnidentifiableBeatIntervals)
                            .arg(beatIntervals.size())
                            .arg(percentageBeatIntervalsUnidentifiable,0,'f',2));
    if ( (percentageBeatIntervalsUnidentifiable / 100.0) > BeatAnalysis::Configuration::maxPermissibleProportionUnmatchedBeats)
    {
        editor->outputToConsole("Too many unmatched beats.\n");
        return false;
    }
    else
        return true;
}

void BeatAnalysis::analysePatterns()
{
    //For each beat interval...
    int i = 0, j = 0, k = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
    {
        //...try identifying patterns of increasing length starting from that beat.
        BeatPattern longestPatternFromThisBeat(i);
        unsigned char longestPatternMatched = 1;
        short beatsMatchedByLongestPattern = 1;
        //For each increasing length of pattern...
        for (j = 1; j <= Configuration::maxPatternLengthInStrokes   &&
          //...check we're not going to overrun any vectors,
               (i + j) <= beatIntervals.size(); ++j)
        {
            //then read in the next 'j' beats starting from 'i'.
            QVector<BeatInterval> thisPattern;
            for (k = 0; k < j; ++k)
                thisPattern.append(beatIntervals.at(i+k));

            //Optionally only allow patterns which are more than one beat and a-power-of-2 beats long
            if ( ! Configuration::allowOddPatternLengths)
            {
                if (j == 1)
                    continue;

                //calculate total pattern length
                float patternLengthInBeats = 0;
                for (k = 0; k < j; ++k)
                {
                    if (thisPattern[k].isKnownBeatValue())
                        patternLengthInBeats += thisPattern[k].getValue()->value();
                    else
                        patternLengthInBeats += ( thisPattern.at(k).getLength() / Configuration::tempoInterval() );
                }
                unsigned short beatsInPattern = roundToInt(patternLengthInBeats);

                // check if patternLengthInBeats is roughly a whole number
                // (non-whole numbers of beats won't loop well!)
                if (fabs(patternLengthInBeats - beatsInPattern) >= (float)SEMIQUAVER_NUMERATOR / (float)SEMIQUAVER_DENOMINATOR)
                    continue;

                if ( ! isPowerOfTwo(beatsInPattern))
                    continue;
            }

            //Iterate forward from 'i', counting how many sequential beats match this pattern when it is repeated
            char positionInPattern = 0;
            short beatsMatchedByThisPattern = 0;
            BeatInterval nextExpectedInterval = thisPattern[positionInPattern];
            while (i + beatsMatchedByThisPattern < beatIntervals.size()/*Don't loop off the end*/)
            {
                bool beatMatches = ( Configuration::matchPatternMembersByActualValue &&
                                     nextExpectedInterval.matchesThisInterval(beatIntervals[i + beatsMatchedByThisPattern]) )
                                 ||
                                   ( Configuration::matchPatternMembersByNearestKnownValue &&
                                     nextExpectedInterval.getValue() == beatIntervals[i + beatsMatchedByThisPattern].getValue() );
                //stop looping when pattern doesn't match...
                if ( ! beatMatches)
                    break;

                //...otherwise:
                ++beatsMatchedByThisPattern;
                ++positionInPattern;
                positionInPattern %= j;//reset to beginning of pattern if we've reached the end
                nextExpectedInterval = thisPattern[positionInPattern];
            }

            //allow patterns that match all except the last beat of the last repetition
            //this is useful if the pattern is followed by a break, or
            //the following pattern doesn't start on the beat (i.e. starts with a rest)
            if ( (beatsMatchedByThisPattern % j) != (j - 1) ||
                  beatsMatchedByThisPattern < 2 * j ) //experimental to stop recognising patterns incorrectly across breaks,
                                                      //which may deplicate or replace logic marked by '**' below
                //otherwise only count whole repetitions
                beatsMatchedByThisPattern -= beatsMatchedByThisPattern % j;

            if ( //  EITHER this pattern must match or exceed the number of beats
                 //  matched by a shorter pattern, but then temper this so that
                 //  a single 'pattern' of 9 assorted, unrepeating beats doesn't match
                 //  better than 2 repetitions of a 4-beat pattern
                 //  i.e. pattern must actually REPEAT in order to count (see '**' above)
                    ( beatsMatchedByThisPattern >= beatsMatchedByLongestPattern &&
                      (beatsMatchedByThisPattern + 1) >= (2 * j) ) // '**'
                ||
                 //  OR, IF preferLongPatterns...
                 //  so long as the pattern repeats at least minLongPatternRepetitionsToPrefer, it may
                 //  _almost_ (i.e. within one value of j) match as many beats.
                 //  On the down side, this means 20 straight beats will be matched
                 //  by two patterns of 8 and two of 2.
                 //  On the up side, it helps to stop a short 2 or 4-beat pattern
                 //  accidently eating up the first beats of a similar following one.
                    (Configuration::preferLongPatterns &&
                     j >= Configuration::minLongPatternLengthInStrokes &&
                     beatsMatchedByThisPattern + 1 >= (Configuration::minLongPatternRepetitionsToPrefer * j) &&
                     beatsMatchedByThisPattern < beatsMatchedByLongestPattern &&
                     beatsMatchedByThisPattern > (beatsMatchedByLongestPattern - j)
                    )
                )
            {

                longestPatternMatched = j;
                beatsMatchedByLongestPattern = beatsMatchedByThisPattern;

                longestPatternFromThisBeat.clear();
                for (k = 0; k < j; ++k)
                    longestPatternFromThisBeat.append(thisPattern[k].getValue());
                longestPatternFromThisBeat.setTotalStrokesCovered(beatsMatchedByThisPattern);
            }
        }
        if (longestPatternFromThisBeat.size() == 0)
        {
            //no repeating patterns were found - this beat is on its own
            longestPatternFromThisBeat.append(beatIntervals[i].getValue());
            longestPatternFromThisBeat.setTotalStrokesCovered(1);
        }
        beatPatterns.append(longestPatternFromThisBeat);

        //print out info!
        editor->outputToConsole(QString("Matched %1 beat(s) with %2 X %3-beat pattern starting from beat %4 (@%5m%6s):\n")
                .arg(beatsMatchedByLongestPattern)
                .arg(beatsMatchedByLongestPattern == 1 ? 1 : (beatsMatchedByLongestPattern + 1) / longestPatternMatched)
                .arg((int)longestPatternMatched)
                .arg(i+1)
                .arg(beatIntervals[i].startsAtTimestamp() / 60000)
                .arg((beatIntervals[i].startsAtTimestamp() % 60000) / 1000)
                ,true);
        for (j = 0; j < longestPatternFromThisBeat.size(); ++j)
        {
            if (longestPatternFromThisBeat[j])
            {
                editor->outputToConsole(QString("%1, ").arg(longestPatternFromThisBeat[j]->name));
            }
            else
            {
                editor->outputToConsole("Unknown, ");
            }
        }

        //if we matched more than 1 beat with this pattern (which will hopefully happen a lot),
        //skip 'i' forward so we're not looking for patterns within patterns
        i += (beatsMatchedByLongestPattern - 1);
    }
}

void BeatAnalysis::consolidatePatterns()
{
    int i = 0;
    for (i = 0; i < beatPatterns.size(); ++i)
    {
        beatPatterns[i].reduceToShortestForm();
        while (i + 1 != beatPatterns.size() &&
               beatPatterns[i] == beatPatterns[i + 1])
        {
            beatPatterns[i].setTotalStrokesCovered(
                        beatPatterns[i].    totalStrokesCovered() +
                        beatPatterns[i + 1].totalStrokesCovered());
            beatPatterns.removeAt(i + 1);
        }
    }
}
