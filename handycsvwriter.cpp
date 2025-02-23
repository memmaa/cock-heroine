#include "handycsvwriter.h"

HandyCsvWriter::HandyCsvWriter(QFile & file, QObject * parent)
    :
    SyncFileWriter(file, parent)
{

}

void HandyCsvWriter::writeHeader()
{
    QString fileHeader = QString("#Handy sync file. Exported from Cock Heroine\n");
    file.write(fileHeader.toLatin1());
}

void HandyCsvWriter::addEvent(long timestamp, int position)
{
    QString eventString = csvEntryString(timestamp, position);
    file.write(eventString.toLatin1());
}

void HandyCsvWriter::writeFooter()
{
    //A CSV file needs no footer
}

QString HandyCsvWriter::csvEntryString(long timestamp, int targetLocation)
{
    return QString("%1,%2\n").arg(timestamp).arg(targetLocation);
}
