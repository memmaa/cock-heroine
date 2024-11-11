#include "preplaybackactionmanager.h"
#include "optionsdialog.h"
#include <QMessageBox>
#include "mainwindow.h"
#include <QPushButton>
#include "playbackassociatedactions/pregenerateestimsignalaction.h"
#include <QCoreApplication>
#include "preplaybackinfodialog.h"

PrePlaybackActionManager::PrePlaybackActionManager(QObject *parent)
    :
        QObject(parent),
        someFailed(false),
        userRequestedAbort(false),
        allDone(false),
        progressInfoDialog(new PrePlaybackInfoDialog(mainWindow))
{
    connect(progressInfoDialog, SIGNAL(accepted()), this, SIGNAL(abortAllRemainingJobs()));
    connect(progressInfoDialog, SIGNAL(rejected()), this, SLOT(dialogCancelled()));
}

PrePlaybackActionManager::~PrePlaybackActionManager()
{
    progressInfoDialog->deleteLater();
}

bool PrePlaybackActionManager::waitForCompletion()
{
    if (actionMap.isEmpty())
    {
        someFailed = false;
        userRequestedAbort = false;
        allDone = true;
        return true;
    }
    while (1)
    {
        if (userRequestedAbort)
        {
//            progressInfoDialog->hide();
            return false;
        }
        else if (allDone)
        {
//            progressInfoDialog->hide();
            return true;
        }
    }
}

void PrePlaybackActionManager::startAll()
{
    someFailed = false;
    userRequestedAbort = false;
    allDone = false;
    if (OptionsDialog::emitEstimSignal() && OptionsDialog::getEstimSourceMode() == EstimSourceMode::PREGENERATED)
    {
        PlaybackAssociatedAction * estimGeneration = new PregenerateEstimSignalAction();
        connectUp(estimGeneration);
    }
    if (!actionMap.isEmpty())
    {
        updateDialogContent();
        progressInfoDialog->show();
        emit startAllJobs();
    }
}

void PrePlaybackActionManager::handleActionStarted(QString uniqueActionName)
{
    currentlyRunning.append(uniqueActionName);
    updateDialogContent();
}

void PrePlaybackActionManager::handleActionCompleted(QString uniqueActionName)
{
    currentlyRunning.removeOne(uniqueActionName);
    if (currentlyRunning.isEmpty())
        allDone = true;
    else
        updateDialogContent();
}

void PrePlaybackActionManager::handleActionFailed(QString uniqueActionName, QString errorMessage)
{
    someFailed = true;
    currentlyRunning.removeOne(uniqueActionName);
    updateDialogContent();
    QString dialogContent = QString("The action '%1' failed with the message:\n\n%2\n\nWould you like to continue with playback?").arg(uniqueActionName).arg(errorMessage);
    QMessageBox::StandardButton button = QMessageBox::question(mainWindow, tr("Playback Preparation Failed"), dialogContent);
    if (button == QMessageBox::No)
    {
        userRequestedAbort = true;
        emit abortAllRemainingJobs();
    }
    else if (currentlyRunning.isEmpty())
        allDone = true;
}

void PrePlaybackActionManager::dialogCancelled()
{
    userRequestedAbort = true;
    emit abortAllRemainingJobs();
}

void PrePlaybackActionManager::connectUp(PlaybackAssociatedAction *action)
{
    actionMap.insert(action->getName(), action);
    connect(action, SIGNAL(started(QString)), this, SLOT(handleActionStarted(QString)));
    connect(action, SIGNAL(completed(QString)), this, SLOT(handleActionCompleted(QString)));
    connect(action, SIGNAL(failed(QString, QString)), this, SLOT(handleActionFailed(QString, QString)));
    connect(this, SIGNAL(startAllJobs()), action, SLOT(kickOff()));
    connect(this, SIGNAL(abortAllRemainingJobs()), action, SLOT(abort()));
}

void PrePlaybackActionManager::updateDialogContent()
{
    progressInfoDialog->displayList(currentlyRunning);
}

QString PrePlaybackActionManager::createDialogMessage()
{
    QString list;
    for (auto job : currentlyRunning)
    {
        list += "\n";
        list += job;
    }
    return QString(tr("Waiting for the following jobs to complete:\n%1")).arg(list);
}
