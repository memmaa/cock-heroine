#ifndef STIMSIGNALSOURCE_H
#define STIMSIGNALSOURCE_H

#include <QObject>
#include <QIODevice>

class StimSignalSource
{
public:
    StimSignalSource();
    virtual bool open(QIODevice::OpenMode mode) = 0;
    virtual void close() = 0;
    virtual void setPlayFrom(long timestamp) = 0;

    static QIODevice * createFromPrefs(QObject * parent);
    static QString calculatePregeneratedStimFilename();

private:
    static unsigned int currentEventsHash();
};

#endif // STIMSIGNALSOURCE_H
