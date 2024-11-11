#include "syncfilewriter.h"

SyncFileWriter::SyncFileWriter(QFile & file, QObject *parent)
    :
    QObject(parent),
    file(file)
{
    //nothing
}
