#ifndef FUNSCRIPTWRITER_H
#define FUNSCRIPTWRITER_H

#include <QObject>
#include "syncfilewriter.h"

class FunscriptWriter : public SyncFileWriter
{
    Q_OBJECT
public:
    FunscriptWriter(QFile &, QObject * parent = nullptr);
    virtual void writeHeader();
    virtual void addEvent(long timestamp, int position);
    virtual void writeFooter();
    QString funscriptEntryString(long timestamp, int targetLocation);
private:
    bool needPreceedingComma = false;
};

#endif // FUNSCRIPTWRITER_H
