#ifndef SYNCFILEWRITER_H
#define SYNCFILEWRITER_H

class QFile;

class SyncFileWriter : public QObject
{
    Q_OBJECT
public:
    explicit SyncFileWriter(QFile &, QObject *parent = nullptr);
    virtual void writeHeader() = 0;
    virtual void addEvent(long timestamp, int position) = 0;
    virtual void writeFooter() = 0;

protected:
    QFile & file;
signals:

};

#endif // SYNCFILEWRITER_H
