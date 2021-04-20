#define debugMeSomeBPMOptomisation /*(printf("bestErrorMargin: %f\tadjustmentAmount: %.45f\timprovedStartTimestamp: %.8f\timprovedTempoInterval: %.8f\n",bestErrorMargin,adjustmentAmount,improvedStartTimestamp,improvedTempoInterval))*/

#include <math.h>

#include <QVector>

#include "globals.h"
#include "beatoptimisation.h"
#include "beatvalue.h"
#include "beatinterval.h"
#include "editorwindow.h"
#include "beatanalysis.h"
#include "beattimestamp.h"
#include "helperfunctions.h"
#include "beatdatamodel.h"

bool BeatOptimisation::Configuration::outputTempoProvided = false;
double BeatOptimisation::Configuration::providedOutputTempo;
bool BeatOptimisation::Configuration::startingTimestampProvided = false;
long BeatOptimisation::Configuration::providedStartingTimestamp;
bool BeatOptimisation::Configuration::roundToNearestBPM;
bool BeatOptimisation::Configuration::roundToEvenBPM;
float BeatOptimisation::Configuration::outputTempoInterval() {return (60 * 1000.0) / providedOutputTempo;}
void BeatOptimisation::Configuration::setImprovedValuesFromProvided() {improvedStartTimestamp = providedStartingTimestamp; improvedTempoInterval = outputTempoInterval();}

long BeatOptimisation::shiftWhenWorking = 0;
float BeatOptimisation::improvedTempoInterval = 0;
float BeatOptimisation::improvedStartTimestamp = 0;

QVector<float> BeatOptimisation::targetTimestamps;
QVector<float> BeatOptimisation::trialTimestamps;

char BeatOptimisation::lastProgressChar = '.';
short BeatOptimisation::outputCharCounter = 1;

void BeatOptimisation::configureTrialTimestamps(float start, float interval)
{
    double timeCode = trialTimestamps[0] = start;
    int i = 0;
    for (i = 1; i < trialTimestamps.size(); ++i)
    {
        if (beatIntervals[i-1].isKnownBeatValue())
            trialTimestamps[i] = timeCode = timeCode + (beatIntervals[i-1].getValue()->value() * interval);
        else
            trialTimestamps[i] = timeCode = timeCode + beatIntervals[i-1].getLength();
    }
}

double BeatOptimisation::compareTrialTimestamps()
{
    double returnValue = 0;
    int i = 0;
    for (i = 0; i < trialTimestamps.size(); ++i)
    {
        returnValue += fabs(targetTimestamps[i] - trialTimestamps[i]);
    }
    return returnValue;
}

void BeatOptimisation::optimiseBeats()
{
    int lengthOfChunktoBeOptimised = beatTimestamps.last().eventData.timestamp - beatTimestamps[0].eventData.timestamp;
    shiftWhenWorking = beatTimestamps.first().eventData.timestamp + (lengthOfChunktoBeOptimised / 2);

    targetTimestamps.resize(beatTimestamps.size());
    for (int i = 0; i < beatTimestamps.size(); ++i)
    {
        targetTimestamps[i] = beatTimestamps[i].eventData.timestamp - shiftWhenWorking;
    }

    float adjustmentAmount = 64;
    double errorMarginThisTime; //initialised below (before use)...
    double bestErrorMargin; //initialised below (before use)...

    if (Configuration::startingTimestampProvided)
    {
        improvedStartTimestamp = Configuration::providedStartingTimestamp - shiftWhenWorking;
        editor->outputToConsole(QString("Using provided start timestamp: %1ms.").arg(Configuration::providedStartingTimestamp),true);
    }
    else
    {
        improvedStartTimestamp = beatTimestamps[0].eventData.timestamp - shiftWhenWorking;
    }
    if (Configuration::outputTempoProvided)
        editor->outputToConsole(QString("Forcing output tempo to (%1BPM).").arg(Configuration::providedOutputTempo,0,'f',2),true);
    else
        Configuration::providedOutputTempo = BeatAnalysis::Configuration::currentBPM;

    improvedTempoInterval = Configuration::outputTempoInterval();

    trialTimestamps.resize(targetTimestamps.size());

    //work out what the error margin currently is, and use it to initialise bestErrorMargin
    configureTrialTimestamps(improvedStartTimestamp, improvedTempoInterval);
    bestErrorMargin = compareTrialTimestamps();

    editor->outputToConsole("Optimising ",true);
    if ( ! Configuration::startingTimestampProvided )
        editor->outputToConsole("start timestamp");
    if ( ! ( Configuration::startingTimestampProvided || Configuration::outputTempoProvided ) )
        editor->outputToConsole(" and ");
    if ( ! Configuration::outputTempoProvided )
        editor->outputToConsole("tempo");
    editor->outputToConsole("...\n");

    debugMeSomeBPMOptomisation;

    while (adjustmentAmount > 0)
    {
        bool adjustmentMade = false;
        do
        {
            adjustmentMade = false;

            if (Configuration::startingTimestampProvided == false)
            {
                //try moving all beats back
                configureTrialTimestamps(improvedStartTimestamp + adjustmentAmount, improvedTempoInterval);
                errorMarginThisTime = compareTrialTimestamps();
                if (errorMarginThisTime < bestErrorMargin)
                {
                    bestErrorMargin = errorMarginThisTime;
                    improvedStartTimestamp += adjustmentAmount;
                    adjustmentMade = true;
                    outputProgressChar('>');
                    debugMeSomeBPMOptomisation;
                    continue;
                }

                //try moving all beats forward
                configureTrialTimestamps(improvedStartTimestamp - adjustmentAmount, improvedTempoInterval);
                errorMarginThisTime = compareTrialTimestamps();
                if (errorMarginThisTime < bestErrorMargin)
                {
                    bestErrorMargin = errorMarginThisTime;
                    improvedStartTimestamp -= adjustmentAmount;
                    adjustmentMade = true;
                    outputProgressChar('<');
                    debugMeSomeBPMOptomisation;
                    continue;
                }
            } //if (Configuration::startingTimestampProvided == false)

            if (Configuration::outputTempoProvided == false)
            {
                //try moving all beats further apart (slower)
                configureTrialTimestamps(improvedStartTimestamp, improvedTempoInterval + adjustmentAmount);
                errorMarginThisTime = compareTrialTimestamps();
                if (errorMarginThisTime < bestErrorMargin)
                {
                    bestErrorMargin = errorMarginThisTime;
                    improvedTempoInterval += adjustmentAmount;
                    adjustmentMade = true;
                    outputProgressChar('+');
                    debugMeSomeBPMOptomisation;
                    continue;
                }

                //try moving all beats closer together (faster)
                configureTrialTimestamps(improvedStartTimestamp, improvedTempoInterval - adjustmentAmount);
                errorMarginThisTime = compareTrialTimestamps();
                if (errorMarginThisTime < bestErrorMargin)
                {
                    bestErrorMargin = errorMarginThisTime;
                    improvedTempoInterval -= adjustmentAmount;
                    adjustmentMade = true;
                    outputProgressChar('-');
                    debugMeSomeBPMOptomisation;
                    continue;
                }
            } //if (Configuration::outputTempoProvided == false)

        } while (adjustmentMade);
        adjustmentAmount /= 2;
        outputProgressChar('.');
        debugMeSomeBPMOptomisation;
    }
    //editor->outputToConsole("\n");
    if ( ! Configuration::startingTimestampProvided )
    {
        editor->outputToConsole(QString("Optimised start timestamp: %1ms").arg(improvedStartTimestamp + shiftWhenWorking,0,'f',0),true);
    }
    if ( ! Configuration::outputTempoProvided )
    {
        editor->outputToConsole(QString("Optimised tempo: %1 BPM").arg((60 * 1000) / improvedTempoInterval,0,'f',2),true);
    }
}

void BeatOptimisation::outputProgressChar(char progressChar)
{
    if (progressChar != lastProgressChar)
    {
        outputCharCounter = 1;
        lastProgressChar = progressChar;
    }
    else
    {
        ++outputCharCounter;
    }
    if (isPowerOfTwo(outputCharCounter))
    {
        editor->outputToConsole(QString(progressChar));
    }
}

void BeatOptimisation::applyBeats()
{
    double currentTimeStamp = improvedStartTimestamp;

    int i = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
    {
        float currentBeatLength;
        if (beatIntervals[i].getValue())
            currentBeatLength = beatIntervals[i].getValue()->value() * improvedTempoInterval;
        else
            currentBeatLength = beatIntervals[i].getLength();
        editor->beatModel->changeTimestampAt(i, roundToInt(currentTimeStamp) + shiftWhenWorking);
        currentTimeStamp += currentBeatLength;
    }
    editor->beatModel->changeTimestampAt(i, roundToInt(currentTimeStamp) + shiftWhenWorking);
    shiftWhenWorking = 0;
}
