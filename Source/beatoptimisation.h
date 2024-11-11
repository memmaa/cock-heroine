#ifndef BEATOPTIMISATION_H
#define BEATOPTIMISATION_H

struct BeatOptimisation
{
    static void optimiseBeats();
    static void applyBeats();

    struct Configuration
    {
        static bool outputTempoProvided;
        static double providedOutputTempo;
        static bool startingTimestampProvided;
        static long providedStartingTimestamp;
        static bool roundToNearestBPM;
        static bool roundToEvenBPM;
        static float outputTempoInterval();
        static void setImprovedValuesFromProvided();
    };

private:
    static long shiftWhenWorking;
    static float improvedStartTimestamp;
    static float improvedTempoInterval;
    static QVector<float> targetTimestamps;
    static QVector<float> trialTimestamps;
    static char lastProgressChar;
    static short outputCharCounter;

    static void configureTrialTimestamps(float start, float interval);
    static double compareTrialTimestamps();
    static void outputProgressChar(char progressChar);

    friend class BeatDataModel;
    friend class EditorWindow;
};

#endif // BEATOPTIMISATION_H
