#ifndef HANDYCSVWRITER_H
#define HANDYCSVWRITER_H

#include <QObject>
#include "syncfilewriter.h"

class HandyCsvWriter : public SyncFileWriter
{
    Q_OBJECT
public:
    HandyCsvWriter(QFile &, QObject * parent = nullptr);
    virtual void writeHeader();
    virtual void addEvent(long timestamp, int position);
    virtual void writeFooter();
    QString csvEntryString(long timestamp, int targetLocation);
};

#endif // HANDYCSVWRITER_H
