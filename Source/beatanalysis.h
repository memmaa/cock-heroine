#ifndef BEATANALYSIS_H
#define BEATANALYSIS_H

#define SEMIQUAVER_NUMERATOR 1
#define SEMIQUAVER_DENOMINATOR 4
#define QUAVER_TRIPLET_NUMERATOR 1
#define QUAVER_TRIPLET_DENOMINATOR 3
#define QUAVER_NUMERATOR 1
#define QUAVER_DENOMINATOR 2
#define CROCHET_TRIPLET_NUMERATOR 2
#define CROCHET_TRIPLET_DENOMINATOR 3
#define THREE_SEMIQUAVERS_NUMERATOR 3
#define THREE_SEMIQUAVERS_DENOMINATOR 4
#define CROCHET_NUMERATOR 1
#define CROCHET_DENOMINATOR 1
#define FIVE_SEMIQUAVERS_NUMERATOR 5
#define FIVE_SEMIQUAVERS_DENOMINATOR 4
#define TWO_CROCHET_TRIPLETS_NUMERATOR 4
#define TWO_CROCHET_TRIPLETS_DENOMINATOR 3
#define THREE_QUAVERS_NUMERATOR 3
#define THREE_QUAVERS_DENOMINATOR 2
#define SEVEN_SEMIQUAVERS_NUMERATOR 7
#define SEVEN_SEMIQUAVERS_DENOMINATOR 4
#define TWO_BEATS_NUMERATOR 2
#define TWO_BEATS_DENOMINATOR 1
#define NINE_SEMIQUAVERS_NUMERATOR 9
#define NINE_SEMIQUAVERS_DENOMINATOR 4
#define FIVE_QUAVERS_NUMERATOR 5
#define FIVE_QUAVERS_DENOMINATOR 2
#define THREE_BEATS_NUMERATOR 3
#define THREE_BEATS_DENOMINATOR 1
#define SEVEN_QUAVERS_NUMERATOR 7
#define SEVEN_QUAVERS_DENOMINATOR 2
#define FIFTEEN_SEMIQUAVERS_NUMERATOR 15
#define FIFTEEN_SEMIQUAVERS_DENOMINATOR 4
#define FOUR_BEATS_NUMERATOR 4
#define FOUR_BEATS_DENOMINATOR 1
#define SEVENTEEN_SEMIQUAVERS_NUMERATOR 17
#define SEVENTEEN_SEMIQUAVERS_DENOMINATOR 4
#define NINE_QUAVERS_NUMERATOR 9
#define NINE_QUAVERS_DENOMINATOR 2
#define FIVE_BEATS_NUMERATOR 5
#define FIVE_BEATS_DENOMINATOR 1
#define FIVE_AND_A_HALF_BEATS_NUMERATOR 11
#define FIVE_AND_A_HALF_BEATS_DENOMINATOR 2
#define SIX_BEATS_NUMERATOR 6
#define SIX_BEATS_DENOMINATOR 1
#define SEVEN_BEATS_NUMERATOR 7
#define SEVEN_BEATS_DENOMINATOR 1
#define EIGHT_BEATS_NUMERATOR 8
#define EIGHT_BEATS_DENOMINATOR 1
//replaced by 'break length detection':
//#define TWELVE_BEATS_NUMERATOR 12
//#define TWELVE_BEATS_DENOMINATOR 1
//#define SIXTEEN_BEATS_NUMERATOR 16
//#define SIXTEEN_BEATS_DENOMINATOR 1
//#define TWENTY_FOUR_BEATS_NUMERATOR 24
//#define TWENTY_FOUR_BEATS_DENOMINATOR 1
//#define THIRTY_TWO_BEATS_NUMERATOR 32
//#define THIRTY_TWO_BEATS_DENOMINATOR 1

class AbstractBeatInterval;

struct BeatAnalysis
{
    struct Configuration
    {
        static unsigned char maxUniqueIntervalMergeCycles;
        static float maxPercentAcceptableBeatError;
        static float badMatchThreshhold;
        static bool allowTripletValues;
        static float minNumberOfBeatsToQualifyAsBreak;
        static bool allowHalfBeatsInBreaks;
        static float maxPermissibleProportionUnmatchedBeats;

        //tempo
        static bool inputTempoProvided;
        static double providedInputTempo;
        static unsigned short maxExpectedBPM;
        static unsigned short minExpectedBPM;
        static bool tempoEstablished;
        static double currentBPM;
        static unsigned short minExpectedTempoInterval();
        static unsigned short maxExpectedTempoInterval();
        static double tempoInterval();
        static void setTempoFromInterval(double interval);
        static void setTempoFromInterval(const AbstractBeatInterval & interval);
        static void expectSlowTempo();
        static void expectNormalTempo();
        static void expectFastTempo();

        //patterns
        static bool matchPatternMembersByNearestKnownValue;
        static bool matchPatternMembersByActualValue;
        static unsigned char maxPatternLengthInStrokes;
        static bool allowOddPatternLengths;
        static bool preferLongPatterns;
        static unsigned char minLongPatternLengthInStrokes;
        static unsigned char minLongPatternRepetitionsToPrefer;
    };

    static bool calculateUniqueBeatIntervals();
    static bool calculateTempo();
    static void initialiseBeatValues();
    static void calculateBreakValues();
    static void reorderUniqueBeatIntervals();
    static bool analyseUniqueBeatIntervals();
    static void analysePatterns();
    static void consolidatePatterns();
};

#endif // BEATANALYSIS_H
