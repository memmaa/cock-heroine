#ifndef PREPLAYBACKACTIONMANAGER_H
#define PREPLAYBACKACTIONMANAGER_H

#include <QObject>
#include <QMap>
#include "playbackassociatedaction.h"
#include "preplaybackinfodialog.h"
class QAbstractButton;

class PrePlaybackActionManager : public QObject
{
    Q_OBJECT
public:
    explicit PrePlaybackActionManager(QObject *parent = nullptr);
    ~PrePlaybackActionManager();
    bool waitForCompletion();

signals:

public slots:
    void startAll();
    void handleActionStarted(QString uniqueActionName);
    void handleActionCompleted(QString uniqueActionName);
    void handleActionFailed(QString uniqueActionName, QString errorMessage);
    void dialogCancelled();

signals:
    void startAllJobs();
    void abortAllRemainingJobs();

private:
    QMap<QString, PlaybackAssociatedAction *> actionMap;
    QList<QString> currentlyRunning;
    void connectUp(PlaybackAssociatedAction * action);
    bool someFailed = false;
    bool userRequestedAbort = false;
    bool allDone = false;
    void updateDialogContent();
    QString createDialogMessage();
    PrePlaybackInfoDialog * progressInfoDialog;
    QAbstractButton * cancelButton;
};

#endif // PREPLAYBACKACTIONMANAGER_H
