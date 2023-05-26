#ifndef ESTIMWAVFILEWRITER_H
#define ESTIMWAVFILEWRITER_H

#include <QObject>
#include <QFile>
class QProgressDialog;

class EstimWavFileWriter : public QObject
{
    Q_OBJECT
public:
    explicit EstimWavFileWriter(QString filePath, QObject *parent = nullptr);
    bool writeFile();
signals:

public slots:
    void requestCancel();

private:
    QFile file;
    bool cancelRequested = false;
};

#endif // ESTIMWAVFILEWRITER_H
