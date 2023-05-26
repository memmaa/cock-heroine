#ifndef PLAYBACKASSOCIATEDACTION_H
#define PLAYBACKASSOCIATEDACTION_H

#include <QObject>

class PlaybackAssociatedAction : public QObject
{
    Q_OBJECT
public:
    explicit PlaybackAssociatedAction(QString name, QObject *parent = nullptr);
    QString getName() {return name;};

signals:
    void started(QString uniqueActionName);
    void completed(QString uniqueActionName);
    void failed(QString uniqueActionName, QString errorMessage);

public slots:
    virtual void kickOff() = 0;
    virtual void abort() = 0;

protected:
    QString name;
};

#endif // PLAYBACKASSOCIATEDACTION_H
