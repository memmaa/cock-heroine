#ifndef BEATVALUE_H
#define BEATVALUE_H

#include<QString>
#include<QVector>

struct BeatValue
{
    BeatValue() : active(false) {} //Because of QVector's annoying requirement for a default ctr.
    BeatValue(unsigned short numerator, unsigned char denominator, QString name, bool active = true, bool preset = false) : numerator(numerator), denominator(denominator), name(name), active(active), preset(preset) {}
    unsigned short numerator;
    unsigned char denominator;
    QString name;
    bool active;
    bool preset;
    float value() const;
    float getLength() const;
};

#endif // BEATVALUE_H
