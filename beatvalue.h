#ifndef BEATVALUE_H
#define BEATVALUE_H

#include<QString>
#include<QVector>
#include "beatanalysis.h"

class BeatValue
{
public:

    BeatValue() : active(false) {} //Because of QVector's annoying requirement for a default ctr.
    BeatValue(unsigned short numerator,
              unsigned char denominator,
              QString name,
              bool active = true,
              bool preset = false)
                : numerator(numerator),
                  denominator(denominator),
                  name(name),
                  active(active),
                  preset(preset)
            {
        //nothing
    }
    unsigned short numerator;
    unsigned char denominator;
    QString name;
    bool active;
    bool preset;
    //!
    //! \brief value the number of beats in this value. For example, 'Four Beats' is 4.0, 'Five Semisquavers' is 1.25.
    //! \return
    //!
    float value() const;
    float getLength(double atTempo = BeatAnalysis::Configuration::currentBPM) const;
    //!
    //! \brief isBestEnabledMatchFor is this value the one that would be given if asked for the best match for the given length of time
    //! \param intervalInMs
    //! \return true if it's the best match
    //!
    bool isBestEnabledMatchFor(int intervalInMs, double tempo = BeatAnalysis::Configuration::currentBPM);
    //!
    //! \brief isBestPossibleMatchFor is this value the best match for the given length of time (even if it's not active/enabled)
    //! \param intervalInMs
    //! \return true if it's the best match
    //!
    bool isBestPossibleMatchFor(int intervalInMs, double tempo = BeatAnalysis::Configuration::currentBPM);
    //!
    //! \brief isBestPossibleMatchFor would this value match the given length of time (if it were enabled and unmasked by any other better matches)
    //! \param intervalInMs
    //! \return true if it's a possible match
    //!
    bool isPossibleMatchFor(int intervalInMs, double tempo = BeatAnalysis::Configuration::currentBPM);

    bool operator==(const BeatValue & other);
};

#endif // BEATVALUE_H
