#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QKeyEvent>
#include "globals.h"
#include "wavefileexporter.h"
#include "midifilewriter.h"
#include "midifilereader.h"
#include "eventtabledelegate.h"
#include "editorwindow.h"
#include "helperfunctions.h"
#include <QSettings>
#include "beatanalysis.h"
#include "chmlhandler.h"
#include "optionsdialog.h"
#include "croppedvideosurface.h"
#include "graphicsscenevideodialog.h"
#include <QtWidgets>
#include "keyboardshortcutsdialog.h"
#include <algorithm> //std::min
#include <QtGamepad/QGamepad>
#include "customeventaction.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>
#include <QHttpMultiPart>
#include "handycsvwriter.h"
#include "funscriptwriter.h"
#include "exportbeatmeterdialog.h"

//how many events should we 'buffer' by sending them to the arduino hardware ahead of real time?
//this lets it spin up motors in advance, or enqueue or ramp up its internal events for most accurate timings.
#define MAX_ENQUEUED_EVENTS 5

/*#include <QItemDelegate>
#include <QItemEditorFactory>*/

#include "event.h"

// To be useful for me:
// UI:
// DONE: Sort (or allow sorting of) LIST at suitable times.
// DONE: Facilitate convenient deletion of events
// DONE: Facilitate seeking within video (honestly, clicking a table row would be fine, or click timecode display to enter timecode to seek to)
// DONE: Facilitate convenient insertion of events
// DONE: Allow playing without returning to beginning
// Hardware:
// DONE: Almost everything.
// DONE: Split events into start and stop
// DONE: Implement intensity adjustment controls
// DONE: Implement edge event types
// DONE: Solder 2.5mm jack connectors and breadboard connectors

// Better for me:
// DONE: Assign keyboard shortcuts for use when video is fullscreen (bonus: allow making video fullscreen)
// DONE: Allow opening of video files using UI
// DONE: Better syncing of beats with video if video is laggy to start

// To be useful to others:
// DONE: would be nice to have spin box editors in the table
// DONE: Ability to output rhythm as sound file (bonus: stroke sound selectable)
// DONE: Ability to output to MIDI file (no configuration yet possible)
// DONE: Ability to run beat analysis and correction on selected beats
// DONE: Ability to name patterns (requires further processing of the patterns vector/LIST) (DONE BONUS: by different naming conventions e.g. 1-234-5- or 1-234-5 or 1-123-1 or 1-3-1 are all the same thing)
// ^^^^  Not customisable in UI yet, but edit BeatPattern::name(): justListBeatValues, useExactNaming, useGlobalDenominators, collapseClusters, separateClusterNumbering, omitTrailingSeparators, repeatShortPatternNames, maxPatternLengthToRepeatName

// DONE: compare pattern members by nearest known value as well as/instead of actual value
// DONE: name patterns properly
// DONE: consolidate patterns:
// DONE: reduce pattern to shortest repeating subpattern
// DONE: pattern.operator==
// DONE: pattern.cyclesTheSameAs
// DONE: pattern.numberOfBeatsCovered, add to model and select values AND timestamps accordingly

// DONE: set table column widths from model, by getModel(), for columnCount(), table->setColWidth (headerData(SizeHint)) or something.
// DONE: make selection changes work properly, based on change of selection, not current item (and make sure UI views are updated correctly when selection is changed by code, not user.
// DONE: fix editor console output so cursor is actually taken to end of document before adding text.
// DONE: hide mouse cursor when video is fullscreen
// DONE: implement interval editing
// DONE: improve accuracy of optimisation by working on a copy of the beats starting at or centred around 0
// DONE: enable rounding to closest whole number of beats during optimisation
// DONE: add tap tempo input
//-DONE: make funscript export a separate menu action
//-DONE: add support for setting stroke intensity with a gamepad axis (can use right trigger on gamecube controller)
//-TODO: add support for edging control with a gamepad button
//-TODO: add support for selecting gamepad axis and buttons
//-TODO: optionally unload current events before loading new file
//-TODO: request save before exit
//-TODO: add butt plugin support
//-TODO: add UI support for configuring automatic pattern naming

// To make it faaaar too swanky and gold-plated:
//-DONE: Read those MIDI files back in again (similar amount of configurability)
//-DONE: Change BeatAnalysis::Configuration::tempoInterval() to use floating point values instea of integer division and returning a short
//-TODONE: Graphical keyboard shortcut editor (in progress, but not very useful yet)
//-TODO: allow configurability of MIDI input/output
//-TODO: Visual beat-meter editor allowing adjustment of timestamps and/or beatIntervals
// ^^^^  We're talking click-and-drag on a beat to move it forward/back, click in the space between beats to select the interval on the right
//-TODO: Themable beat meter creation
//-TODO: Pattern editor allowing drag-n-drop creation or editing of both patterns and sequences of patterns

//!why would you turn this off?
bool MainWindow::extraVideoSync = true;
//!how often should we try to re-sync the clock to the video when we first start playing?
quint16 MainWindow::videoSyncDelay = 500;
//!honestly, 50ms is pretty high - a musical ear will notice this and anything higher.
quint8 MainWindow::maxVideoSyncError = 50;
//!chml file type saves the most data
const QString MainWindow::defaultFileExt = ".chml";
//!the event we're currently 'working on' by holding down a key
Event currentTimedEvent;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),

    maskingRectangle(nullptr),
    customShortcuts(new QList<CustomEventAction *>()),

    ui(new Ui::MainWindow),

    eventsModel(new eventDataModel(this)),
    eventsProxyModel(new EventDataProxyModel(this)),
    loadedVideo(QString()),
    loadedScript(QString()),
    strokeSoundSource(QString()),
    strokeSound(this),

    connectedArduinoFound(false),
    arduinoConnection(NULL),

    enqueuedEvents(0),
    scheduledEvent(0,0,0),

    timecodeDialog(new SeekToTimecodeDialog(this)),

    waitingForEdgeRelease(false),
    edgedOut(false),
    waitingForTimedEventRelease(false),

    playbackLatency(0),
    mainWindowActions(new QList<QAction *>())
{
    mainWindow = this;
    ui->setupUi(this);

    ui->graphicsView->installEventFilter(this);
    videoScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(videoScene);
    videoItem = new QGraphicsVideoItem();
    videoScene->addItem(videoItem);
    videoPlayer = new QMediaPlayer(this);
    ui->videoGoesHere->installEventFilter(this);
    //it's important that all video-related widgets are set up before this point, as the
    //below line will access them if required.
    ui->actionObscure_Beat_Meter->setChecked(settings.value("Obscure Beat Meter").toBool());
    //no need to call maskBeatMeter() here, as the above line did it if needed... I think...
    //...unless actionObscure_Beat_Meter was already checked?

    createActions();
    KeyboardShortcutsDialog::applyActionShortcutsFromPrefs();

    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::httpResponseReceived);
    ui->graphicsView->setVisible(false);
    lcdDisplay = ui->lcdNumber;
    BeatAnalysis::initialiseBeatValues();
    eventsProxyModel->setSourceModel(eventsModel);
    ui->eventsTable->setModel(eventsProxyModel);
    ui->eventsTable->setItemDelegate(new EventTableDelegate);
    setColumWidthsFromModel(ui->eventsTable);
    connect(ui->eventsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentItemChanged(const QModelIndex &, const QModelIndex &)));

    setFocusPolicy(Qt::StrongFocus);

    videoPlayer->setVideoOutput(ui->videoGoesHere);
    //ui->videoGoesHere->setAspectRatioMode(Qt::KeepAspectRatioByExpanding);

    if ( ! attemptToSetUpSerial())
    {
        unableToSetUpSerial();
    }

    //set up timers
    timecode = new QTime();

    refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(updateTimerDisplay()));

    edgeReleaseTimer = new QTimer(this);
    edgeReleaseTimer->setInterval(edgeReleaseTime);
    edgeReleaseTimer->setSingleShot(true);
    connect(edgeReleaseTimer, SIGNAL(timeout()), this, SLOT(stopEdging()));

    edgedOutTimer = new QTimer(this);
    edgedOutTimer->setInterval(edgedOutTime);
    edgedOutTimer->setSingleShot(true);
    connect(edgedOutTimer, SIGNAL(timeout()), this, SLOT(resumeEdging()));

    timedEventReleaseTimer = new QTimer(this);
    timedEventReleaseTimer->setInterval(timedEventReleaseTime);
    timedEventReleaseTimer->setSingleShot(true);
    connect(timedEventReleaseTimer, SIGNAL(timeout()), this, SLOT(completeTimedEvent()));

    scheduledEventTimer = new QTimer(this);
    scheduledEventTimer->setTimerType(Qt::PreciseTimer);
    scheduledEventTimer->setSingleShot(true);
    connect(scheduledEventTimer, SIGNAL(timeout()), this, SLOT(triggerScheduledEvent()));

    motorIdleTimer = new QTimer(this);
    motorIdleTimer->setTimerType(Qt::PreciseTimer);
    motorIdleTimer->setSingleShot(true);
    connect(motorIdleTimer, SIGNAL(timeout()), this, SLOT(stopMotorIdling()));

    setMinIntensityTimer = new QTimer(this);
    setMinIntensityTimer->setTimerType(Qt::PreciseTimer);
    setMinIntensityTimer->setSingleShot(true);
    connect(setMinIntensityTimer, SIGNAL(timeout()), this, SLOT(setMinIntensity()));

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (!gamepads.isEmpty()) {
        gamepad = new QGamepad(*gamepads.begin(), this);
        connect(gamepad, &QGamepad::axisRightYChanged, this, [](double value){
            mainWindow->on_shoulderTriggerValueChanged(value);
        });
    }

    temporaryHandyCsvFile.setFileTemplate(QString("%1/CockHeroineHandyTempXXXXXX.csv").arg(QDir::tempPath()));
}

void MainWindow::createActions()
{
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("Clear Eve&nts"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Clear the events list, ready to record or load a new set."));
    connect(newAct, &QAction::triggered, this, &MainWindow::clearEventsList);
    auto firstAction = ui->menuFile->actions().at(0);
    ui->menuFile->insertAction(firstAction, newAct);
    mainWindowActions->append(newAct);

    QAction * openAct = ui->actionOpen;
    openAct->setShortcuts(QKeySequence::Open); //TODONE: Tried to set shortcut in ui editor, but couldn't find system shortcut.
    //connected by slot name
    mainWindowActions->append(openAct);

    QAction * saveAct = ui->actionSave_As;
    saveAct->setShortcuts(QKeySequence::Save);
    //connected by slot name
    mainWindowActions->append(saveAct);

    QAction * exportToMidiAct = ui->actionExport_To_MIDI_File;
    //set from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(exportToMidiAct);

    QAction * exportToAudioAct = ui->actionExport_To_Audio_File;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(exportToAudioAct);

    QAction * launchEditorAct = ui->actionLaunchEditor;
    mainWindowActions->append(launchEditorAct);

    QAction * playAct = ui->actionPlay;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(playAct);

    QAction * pauseAct = ui->actionPause;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(pauseAct);

    QAction * stopAct = ui->actionStop;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(stopAct);

    QAction * jumpToTimeAct = ui->actionJump_To_Time;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(jumpToTimeAct);

    QAction * playStrokeSoundAct = ui->actionPlay_Stroke_Sound_s;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(playStrokeSoundAct);

    QAction * configureStrokeSoundsAct = ui->actionConfigure_Stroke_Sound_s;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(configureStrokeSoundsAct);

    QAction * configureKeyboardShortcuts = ui->actionConfigure_Keyboard_Shortcuts;
    mainWindowActions->append(configureKeyboardShortcuts);

    QAction * preferencesAct = ui->actionPreferences;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(preferencesAct);

    QAction * obscureBeatMeterAct = ui->actionObscure_Beat_Meter;
    //st from preferences?
    //openAct->setShortcuts(something from prefs);
    //connected by slot name
    mainWindowActions->append(obscureBeatMeterAct);
}

MainWindow::~MainWindow()
{
    stopMotorIdling();

    if (arduinoConnection->isOpen())
        arduinoConnection->close();

    delete timecode;
    delete customShortcuts;

    delete ui;
}

void MainWindow::takeVideoOutput()
{
    videoPlayer->stop();
    if (ui->actionObscure_Beat_Meter->isChecked())
    {
        videoPlayer->setVideoOutput(videoItem);
        handleResize();
    }
    else
        videoPlayer->setVideoOutput(ui->videoGoesHere);
    videoPlayer->pause();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    handleResize();
}

void MainWindow::handleResize()
{
    if (ui->graphicsView->parent() == NULL)
    {
        const QRect screenGeometry = QApplication::desktop()->screenGeometry(this);
        ui->graphicsView->setFrameRect(screenGeometry);
        ui->graphicsView->resize(QApplication::desktop()->size());
    }
    videoItem->setSize(ui->graphicsView->size());
    videoScene->setSceneRect(videoItem->boundingRect());
    updateBeatMeterMask();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ( (keyEvent->key() == Qt::Key_Q ||
              keyEvent->key() == Qt::Key_W ||
              keyEvent->key() == Qt::Key_X ||
                keyEvent->key() == Qt::Key_A ||
                keyEvent->key() == Qt::Key_S ||
                keyEvent->key() == Qt::Key_Z ||
                keyEvent->key() == Qt::Key_D ||
                keyEvent->key() == Qt::Key_F ||
              keyEvent->key() == Qt::Key_L ||
              keyEvent->key() == Qt::Key_1 ||
              keyEvent->key() == Qt::Key_2 ||
              keyEvent->key() == Qt::Key_3 ||
                keyEvent->key() == Qt::Key_4 ||
                keyEvent->key() == Qt::Key_5 ||
                keyEvent->key() == Qt::Key_6 ||
              keyEvent->key() == Qt::Key_7 ||
              keyEvent->key() == Qt::Key_8 ||
              keyEvent->key() == Qt::Key_9 ||
              keyEvent->key() == Qt::Key_Left ||
              keyEvent->key() == Qt::Key_Right ||
              keyEvent->key() == Qt::Key_9 ||
              keyEvent->key() == Qt::Key_Plus ||
              keyEvent->key() == Qt::Key_Minus ||
              keyEvent->key() == Qt::Key_Period ||
              keyEvent->key() == Qt::Key_Backslash ||
                keyEvent->key() == Qt::Key_Space ||
                keyEvent->key() == Qt::Key_Escape) ||

                (keyEvent->key() == Qt::Key_E &&
                 waitingForEdgeRelease == false)
             )
        {
            keyPressEvent(keyEvent);
            return true;
        }
        /*if (keyEvent->key() == Qt::Key_Enter ||
            keyEvent->key() == Qt::Key_Return)
        {
            return launchEditor();
        }
        */
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if ( (keyEvent->key() == Qt::Key_E &&
              waitingForEdgeRelease == true) ||
             keyEvent->key() == Qt::Key_L ||
             keyEvent->key() == Qt::Key_Backslash)
        {
            keyReleaseEvent(keyEvent);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
     if (   event->key() == Qt::Key_Delete &&
          ( ! currentlyPlaying ) &&
            eventsModel->rowCount() > 0 )
     {
         QVector<int> rows(1);
         foreach (QModelIndex index, ui->eventsTable->selectionModel()->selectedRows())
         {
             if (!rows.contains(index.row()))
                 rows.append(index.row());
         }
         std::sort(rows.begin(), rows.end(), qGreater<int>());
         foreach (int row, rows)
         {
             eventsProxyModel->removeRows(row,1);
         }
         //this worked before, but was clunky...
//         eventsModel->removeRows(ui->eventsTable->currentIndex().row(),1);
     }
     else if (event->key() == Qt::Key_A ||
              event->key() == Qt::Key_6 )
     {
        on_triggerButton_pressed();
     }
     else if (event->key() == Qt::Key_S ||
              event->key() == Qt::Key_5 )
     {
        on_triggerAndRecordButton_pressed();
     }
     else if (event->key() == Qt::Key_D ||
              event->key() == Qt::Key_4 )
     {
        on_recordButton_pressed();
     }
     else if (event->key() == Qt::Key_1)
     {
        Event smallWandPulse(currentTimecode(),1,0);
        triggerEvent(smallWandPulse);
        addEventToTable(smallWandPulse);
     }
     else if (event->key() == Qt::Key_2)
     {
        Event smallWandPulse(currentTimecode(),1,64);
        triggerEvent(smallWandPulse);
        addEventToTable(smallWandPulse);
     }
     else if (event->key() == Qt::Key_3)
     {
        Event smallWandPulse(currentTimecode(),1,96);
        triggerEvent(smallWandPulse);
        addEventToTable(smallWandPulse);
     }
     else if (event->key() == Qt::Key_7)
     {
        Event bigWandPulse(currentTimecode(),1,191);
        triggerEvent(bigWandPulse);
        addEventToTable(bigWandPulse);
     }
     else if (event->key() == Qt::Key_8)
     {
        Event bigWandPulse(currentTimecode(),1,223);
        triggerEvent(bigWandPulse);
        addEventToTable(bigWandPulse);
     }
     else if (event->key() == Qt::Key_9)
     {
        Event bigWandPulse(currentTimecode(),1,255);
        triggerEvent(bigWandPulse);
        addEventToTable(bigWandPulse);
     }
     else if (event->key() == Qt::Key_Plus)
     {
        playbackLatency += 50;
        sendSerialTimecodeSync();
        if (OptionsDialog::connectToHandy())
            setHandySyncOffset(OptionsDialog::handySyncBaseOffset() + playbackLatency);
     }
     else if (event->key() == Qt::Key_Minus)
     {
        playbackLatency -= 50;
        sendSerialTimecodeSync();
        if (OptionsDialog::connectToHandy())
            setHandySyncOffset(OptionsDialog::handySyncBaseOffset() + playbackLatency);
     }
     else if (event->key() == Qt::Key_Period)
     {
        playbackLatency = 0;
        sendSerialTimecodeSync();
        if (OptionsDialog::connectToHandy())
            setHandySyncOffset(OptionsDialog::handySyncBaseOffset() + playbackLatency);
     }
     else if (event->key() == Qt::Key_F)
     {
         toggleFullscreen();
     }
     else if (event->key() == Qt::Key_Escape)
     {
        if (ui->videoGoesHere->isFullScreen() ||
                ui->graphicsView->isFullScreen())
            toggleFullscreen();
     }
     else if (event->key() == Qt::Key_Space)
     {
        if (currentlyPlaying)
            on_stopButton_pressed();
        else
            on_startButton_pressed();
     }
     else if (event->key() == Qt::Key_E &&
              waitingForEdgeRelease == false)
     {
         Event eventToTrigAndRecord(currentTimecode(),
                                    EVENT_EDGE_BEGIN,
                                    0);
         triggerEvent(eventToTrigAndRecord);
         addEventToTable(eventToTrigAndRecord);
         waitingForEdgeRelease = true;
     }
     else if (event->key() == Qt::Key_Backslash &&
              edgedOut == false)
     {
         Event eventToTrigger(currentTimecode(),
                                    EVENT_EDGE_END,
                                    USER_EDGED_OUT_NOTIFY);
         triggerEvent(eventToTrigger);
         edgedOut = true;
     }
     else if (event->key() == Qt::Key_Z)
     {
        Event smallWandPulse(currentTimecode(),1,64);
        triggerEvent(smallWandPulse);
        addEventToTable(smallWandPulse);
     }
     else if (event->key() == Qt::Key_Q)
     {
        Event bigAnalPulse(currentTimecode(),7,255);
        triggerEvent(bigAnalPulse);
     }
     else if (event->key() == Qt::Key_W)
          {
             Event analPulse(currentTimecode(),7,128);
             triggerEvent(analPulse);
             addEventToTable(analPulse);
          }
     else if (event->key() == Qt::Key_X)
     {
         Event wandPulse(currentTimecode(),1,128);
         Event analPulse(currentTimecode(),7,128);
         triggerEvent(wandPulse);
         addEventToTable(wandPulse);
         triggerEvent(analPulse);
         addEventToTable(analPulse);
     }
     else if (event->key() == Qt::Key_L &&
              waitingForTimedEventRelease == false)
     {
         currentTimedEvent = Event(currentTimecode(), EVENT_TIMED_WAND_PULSE, 0);
         Event eventToTrigger(currentTimecode(),
                                    EVENT_EDGE_BEGIN,
                                    0);
         triggerEvent(eventToTrigger);
         waitingForTimedEventRelease = true;
     }
     else if (event->key() == Qt::Key_Left)
     {
         skipBackward();
     }
     else if (event->key() == Qt::Key_Right)
     {
         skipForward();
     }
     else if (event->key() == Qt::Key_Enter ||
              event->key() == Qt::Key_Return )
     {
         launchEditor();
     }
     else
     {
         event->ignore();
     }
}

void MainWindow::toggleFullscreen()
{
    if (ui->actionObscure_Beat_Meter->isChecked())
    {
        //using graphics scene
        if (ui->graphicsView->isFullScreen())
        {
            ui->dockLayout->addWidget(ui->graphicsView);
            //graphicsView->setParent(ui->verticalLayout);
            ui->graphicsView->showNormal();
            handleResize();
            while (QApplication::overrideCursor())
                QApplication::restoreOverrideCursor();
        }
        else
        {
            ui->graphicsView->setParent(NULL);
            ui->graphicsView->showFullScreen();
            handleResize();
            updateBeatMeterMask(true);
            QApplication::setOverrideCursor(Qt::BlankCursor);
        }
    }
    else
    {
        //just using normal video widget
        if (!ui->videoGoesHere->isFullScreen())
        {
            ui->videoGoesHere->setFullScreen(true);
            QApplication::setOverrideCursor(Qt::BlankCursor);
        }
        else
        {
            ui->videoGoesHere->setFullScreen(false);
            while (QApplication::overrideCursor())
                QApplication::restoreOverrideCursor();
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
     if (event->key() == Qt::Key_E)
     {
         edgeReleaseTimer->start();
         waitingForEdgeRelease = true;
     }
     else if (event->key() == Qt::Key_L)
     {
         currentTimedEvent.value = currentTimecode() - currentTimedEvent.timestamp;
         timedEventReleaseTimer->start();
         waitingForTimedEventRelease = true;
     }
     //TODO: Uncomment this, does it work? It was meant to allow the user to notify that they're close to the edge without
     //      needing the button wired up to the arduino
//     else if (event->key() == Qt::Key_Backslash)
//     {
//         edgedOutTimer->start();
//         edgedOut = false;
//     }
     else
     {
         event->ignore();
     }
}

void MainWindow::mousePressEvent (QMouseEvent * ev)
{
    QWidget * thingClicked = qApp->widgetAt(QCursor::pos());
    if (thingClicked == ui->lcdNumber)
    {
        jumpToTime();
        return;
    }
    else if (thingClicked == ui->videoGoesHere ||
             thingClicked->objectName() == "") //this is cludgy :-(
        on_startButton_pressed();
    else
        ev->ignore();
}

void MainWindow::skipForward()
{
    seekToTimestamp(currentTimecode() + 5000);
}

void MainWindow::skipBackward()
{
    long targetTimecode = currentTimecode() - 5000;
    if (targetTimecode < 0)
        targetTimecode = 0;
    seekToTimestamp(targetTimecode);
}

bool MainWindow::launchEditor()
{
    if (ui->eventsTable->selectionModel()->hasSelection() == false)
        //we won't handle the keypress if there's no selection
        return false;

    if (multipleRowsSelected(ui->eventsTable->selectionModel()->selectedRows()) == false)
        return false;

    //we have more than row selected. Let's do this!
    if (!editor)
        editor = new EditorWindow(this);

    editor->clear();
    editor->takeVideoOutput();
    editor->show();
    editor->setBeatTimestamps(eventsProxyModel,ui->eventsTable->selectionModel()->selectedIndexes());

    return true;
}

void MainWindow::removeDuplicateEvents()
{
    for (int i = 0; i < (events.length()-1); ++i)
    {
        for (int j = i + 1; j < events.length(); ++j)
        {
            QModelIndex index1 = eventsModel->index(i,0);
            QModelIndex index2 = eventsModel->index(j,0);
            Event first  = eventsModel->eventFromIndex(index1);
            Event second = eventsModel->eventFromIndex(index2);
            if (first == second)
            {
                eventsModel->removeRow(j);
                --j;
            }
        }
        if (i % 10 == 0)
            QCoreApplication::processEvents();
    }
}

void MainWindow::clearEventsList()
{
    int totalRows = eventsModel->rowCount();
    for (int i = 0; i < totalRows; ++i)
        eventsModel->removeRow(0);
}

void MainWindow::jumpToTime()
{
    long tempTimecode;
    timecodeDialog->updateTimecode(currentTimecode());
    tempTimecode = timecodeDialog->exec();
    if (tempTimecode >= 0)
        seekToTimestamp(tempTimecode);
}

void MainWindow::stopEdging()
{
    long releaseTime = currentTimecode();
    if (currentlyPlaying)
        releaseTime -= edgeReleaseTime;
    if (releaseTime < 0)
        releaseTime = 0;
    Event eventToTrigAndRecord(releaseTime,
                               EVENT_EDGE_END,
                               0);
    triggerEvent(eventToTrigAndRecord);
    addEventToTable(eventToTrigAndRecord);
    waitingForEdgeRelease = false;
}

void MainWindow::resumeEdging()
{
    Event eventToTrigger(currentTimecode(),
                               EVENT_EDGE_BEGIN,
                               USER_EDGED_OUT_NOTIFY);
    triggerEvent(eventToTrigger);
    edgedOut = false;
}

void MainWindow::completeTimedEvent()
{
    Event eventToTrigger(currentTimecode(),
                               EVENT_EDGE_END,
                               0);
    triggerEvent(eventToTrigger);
    addEventToTable(currentTimedEvent);
    waitingForTimedEventRelease = false;
}

bool MainWindow::attemptToSetUpSerial()
{
    //create serial connection if needed
    if (NULL == arduinoConnection)
        arduinoConnection = new QSerialPort(this);

    //find correct serial port
    connectedArduinoFound = false;
    foreach (const QSerialPortInfo &thisSerialPort, QSerialPortInfo::availablePorts())
    {
        if ( thisSerialPort.hasVendorIdentifier() &&
             thisSerialPort.hasProductIdentifier() &&
             //thisSerialPort.vendorIdentifier() == quint16(9025) && //arduino vendor id
             //thisSerialPort.productIdentifier() == quint16(1) // (my) arduino uno product id - online tutorial suggests it's 63
             thisSerialPort.vendorIdentifier() == quint16(6790) && //fake nano vendor id
             thisSerialPort.productIdentifier() == quint16(29987) // fake nano product id
            )
        {
            connectedArduinoFound = true;
            arduinoConnection->setPortName(thisSerialPort.portName());
        }
    }

    //set up connection
    if (connectedArduinoFound)
    {
        arduinoConnection->open(QSerialPort::ReadWrite);
        arduinoConnection->setBaudRate(QSerialPort::Baud115200);
        arduinoConnection->setDataBits(QSerialPort::Data8);
        arduinoConnection->setParity(QSerialPort::NoParity);
        arduinoConnection->setStopBits(QSerialPort::OneStop);
        arduinoConnection->setFlowControl(QSerialPort::NoFlowControl);
        if (!arduinoConnection->isWritable()) {
            goto printdebug;
        }
        //TODO: check Serial connection successfully established
        return true;
    }
    else
    {
        printdebug:
        static int when = 1, counter = 1;
        if (when++ >= counter)
        {
            when *= 2;
            qDebug() << "Did not connect to arduino. Possibilities: \n";
            foreach (const QSerialPortInfo &thisSerialPort, QSerialPortInfo::availablePorts())
            {
                if ( thisSerialPort.hasVendorIdentifier() &&
                 thisSerialPort.hasProductIdentifier() )
                {
                qDebug() << "VendorId: "   << thisSerialPort.vendorIdentifier();
                qDebug() << " ProductId: " << thisSerialPort.productIdentifier();
                qDebug() << " PortName: "  << thisSerialPort.portName() << "\n";
                }
            }
        }
    }
    return false;// <-- should be
    //return true; // <-- can be used to silence warning window, but probably not The Right Way(TM) to do it (see below)
}

void MainWindow::unableToSetUpSerial()
{
    //set this to true if you don't care about connecting to an arduino to control hitachi wand, inflatable butt blug, vibrator, etc.
    static bool alreadyWarned = false;
    if ( ! alreadyWarned)
    {
        QMessageBox::warning(this,
                             tr("Arduino not found"),
                             tr("Could not find Arduino board attached to a serial port. Sadface."));
        alreadyWarned = true;

    }
}

void MainWindow::updateTimerDisplay()
{
    setLcdDisplay(timecode->elapsed() + (int)timecodeLastStopped);
}

void MainWindow::addEventToTable(Event & eventToAdd)
{
    eventsModel->addEvent(eventToAdd);
    if (currentlyPlaying)
    {
        motorIdleTimer->start(180 * 1000);
        startMotorIdling();
    }

    registerUnsyncedChanges();
}

Event MainWindow::createEventNow()
{
    return Event(currentTimecode(),
                 ui->eventTypeBox->value(),
                 ui->eventValueBox->value());
}

Event MainWindow::eventFromTableRow(int row)
{
    QModelIndex index = eventsProxyModel->index(row,0);
    return eventFromIndex(index);
}

Event MainWindow::eventFromIndex(QModelIndex index)
{
    return eventsProxyModel->eventFromIndex(index);
}

bool MainWindow::triggerEvent(Event & eventToTrigger)
{
    //TODO: Not sure why the null check is commented...
    if (/*strokeSound != NULL &&*/
        ui->actionPlay_Stroke_Sound_s->isChecked())
    {
        strokeSound.play();
    }
    if ((arduinoConnection != NULL && arduinoConnection->isWritable()) || attemptToSetUpSerial())
    {
        arduinoConnection->write(eventToTrigger.toAsciiString().toLatin1());
        return true;
    }
    else
    {
        unableToSetUpSerial();
        return false;
    }
}

void MainWindow::on_triggerButton_pressed()
{
    Event newEvent = createEventNow();
    triggerEvent(newEvent);
}

void MainWindow::on_triggerAndRecordButton_pressed()
{
    Event newEvent = createEventNow();
    triggerEvent(newEvent);
    addEventToTable(newEvent);
}

void MainWindow::on_recordButton_pressed()
{
    Event newEvent = createEventNow();
    addEventToTable(newEvent);
}

void MainWindow::on_startButton_pressed()
{
    if (currentlyPlaying)
        pause();
    else
        play();
}

void MainWindow::pressPlay()
{
    on_startButton_pressed();
}

void MainWindow::play()
{
    ui->startButton->setText(tr("Pause"));
    ui->stopButton->setText(tr("Pause"));
    videoPlayer->setPosition(currentTimecode());//IMPORTANT: Due to the way currentTimecode works, and the way it interacts with syncToVideo() this MUST occur BEFORE startTimer()
    videoPlayer->play();
    currentlyPlaying = true; //IMPORTANT: This MUST occur BEFORE startTimer() but after videoPlayer->setPosition(currentTimecode());
    startTimer();
    if (extraVideoSync &&
            loadedVideo.isEmpty() == false)
        scheduleVideoSync();
    if (OptionsDialog::connectToHandy())
    {
        if (completedHandyPingRequests <= 0)
            calculateHandyServerTimeOffset();
        else
            prepareHandySync();
    }
}

void MainWindow::syncToVideo()
{
    QNetworkRequest handySyncRequest(QUrl(OptionsDialog::getHandyApiBase() + "syncAdjustTimestamp"));
    if (! currentlyPlaying)
        return;
    if (abs(currentTimecode() - videoPlayer->position()) > maxVideoSyncError)
        scheduleVideoSync();

    timecodeLastStopped = videoPlayer->position();
    timecode->restart();
    sendSerialTimecodeSync();
    if (OptionsDialog::connectToHandy())
        sendHandyTimecodeSync(true);
}

void MainWindow::scheduleVideoSync()
{
    QTimer::singleShot(videoSyncDelay,this,SLOT(syncToVideo()));
}

void MainWindow::pause()
{
    currentlyPlaying = false;
    videoPlayer->pause();
    stopTimer();
    ui->stopButton->setText(tr("Reset"));
    ui->startButton->setText(tr("Play"));
    cancelScheduledEvent();
    if (motorIdleTimer->isActive())
        motorIdleTimer->start(10 * 1000);
    if (arduinoConnection != NULL && arduinoConnection->isWritable())
        arduinoConnection->write("r.");
    if (OptionsDialog::connectToHandy())
        stopHandySync();
}

void MainWindow::stop()
{
    if (currentlyPlaying)
        pause();

    setLcdDisplay(0);
    timecodeLastStopped = 0;
    videoPlayer->stop();
    ui->startButton->setText(tr("Start"));
    ui->stopButton->setText(tr("Stop"));
    stopMotorIdling();
    if (OptionsDialog::connectToHandy() || handyIsPlaying)
        stopHandySync();
}

void MainWindow::startTimer()
{
    timecode->start();
    refreshTimer->start(37);

    if ( ((arduinoConnection != NULL && arduinoConnection->isWritable()) || attemptToSetUpSerial()) == false)
        unableToSetUpSerial();

    sendSerialTimecodeSync();
    setMinIntensity();
    scheduleEvent();
}

void MainWindow::sendSerialTimecodeSync()
{
    if (arduinoConnection != NULL && arduinoConnection->isWritable())
    {
    qDebug() <<"Serial Syncing...";
        arduinoConnection->write("r");
        arduinoConnection->write(QString("%1").arg(currentTimecode() + playbackLatency).toLocal8Bit());
        arduinoConnection->write(".");
    }
}

void MainWindow::on_stopButton_pressed()
{
    if (currentlyPlaying)
        pause();
    else
        stop();
}

void MainWindow::stopTimer()
{
    timecodeLastStopped += timecode->elapsed();
    refreshTimer->stop();
    setLcdDisplay(currentTimecode());
}

void MainWindow::scheduleEvent()
{
    QTableView * table = ui->eventsTable;
    int totalRows = table->model()->rowCount();
    if (totalRows == 0)
        return;

    //find the next item to schedule
    //This logic assumes a sorted list, so sort it:
    table->sortByColumn(1,Qt::AscendingOrder);
    //We also need something selected to begin with, or it won't work, might crash
    if (table->selectionModel()->hasSelection() == false)
        table->selectRow(0);
    int thisRow = table->selectionModel()->selectedIndexes().at(0).row();
    //if the row _before_ the current one is in the future,
    //then we need to move back...
    while (thisRow != 0 && //otherwise we could check item -1
           eventFromTableRow(thisRow - 1).timestamp > currentTimecode() )
    {
        --thisRow;
    }

    //coversely, if the current row is in the past, then we'll need to move forward
    while (eventFromTableRow(thisRow).timestamp < currentTimecode() &&
           thisRow + 1 < totalRows ) //prevent advancing past the end of the table)
    {
        ++thisRow;
    }

    //we now know that the current item is either right now, or first in line, or the last event.
    //If it's not in the future, it could be one we just triggered this millisecond,
    //so check that this row isn't the same as the last event we triggered to:
    //a) avoid triggering the same event twice
    //b) skip any duplicates...
    while (eventFromTableRow(thisRow) == scheduledEvent &&
           thisRow + 1 < totalRows ) //prevent advancing past the end of the table
    {
        ++thisRow;
    }

    //if this is the last row, and we already triggered it, or it's in the past,
    //abort to prevent infinite loop
    if (thisRow + 1 >= totalRows
            &&
            (   eventFromTableRow(thisRow) == scheduledEvent
                ||
                eventFromTableRow(thisRow).timestamp < currentTimecode()
             )
        )
    {
        cancelScheduledEvent();
        table->selectRow(thisRow);
        return;
    }

    //We now know we have an event which is present or future, which we
    //haven't already triggered, so we should schedule it to be triggered...
    scheduledEvent = eventFromTableRow(thisRow);
    long timerLength = scheduledEvent.timestamp - currentTimecode();
    timerLength -= playbackLatency;

    if (timerLength < 0)
        timerLength = 0;
    scheduledEventTimer->start(timerLength);
    //select the previous row, because that's probably the one that was just triggered.
    if (thisRow != 0)
        table->selectRow(thisRow - 1);
    else
        table->selectRow(0);

    int targetRow;
    while (enqueuedEvents < MAX_ENQUEUED_EVENTS)
    {
        targetRow = thisRow + enqueuedEvents;
        if (targetRow < totalRows)
        {
            Event triggerMe = eventFromTableRow(targetRow);
            triggerEvent(triggerMe);
        }
        ++enqueuedEvents;
    }
    targetRow = thisRow + enqueuedEvents;
    if (!(targetRow % 32))
    {
        sendSerialTimecodeSync();
        if (OptionsDialog::connectToHandy())
            sendHandyTimecodeSync();
    }
    if (targetRow < totalRows)
    {
        Event triggerMe = eventFromTableRow(targetRow);
        triggerEvent(triggerMe);
    }
}

void MainWindow::triggerScheduledEvent()
{
    //XXX: Mysteriously, things seem to work fine despite the fact the below line is commented out...
    //     Meaning... this function is probably mis-named. TODO: Work out what's going on here?
    //triggerEvent(scheduledEvent);
    scheduleEvent();
}

void MainWindow::cancelScheduledEvent()
{
    scheduledEventTimer->stop();
    scheduledEvent = Event(0,0,0);
    enqueuedEvents = 0;
}

void MainWindow::on_saveButton_clicked()
{
    if (ui->eventsTable->model()->rowCount() == 0)
        return; //don't save empty file

    QString filenameToSave = QFileDialog::getSaveFileName(this, tr("Save Wand Events"),
                                                "~/Documents",
                                                tr("Cock Hero Markup Language file - includes metadata (*.chml);;Wand Controller ASCII file - human readable (*.wca);;Wand Controller Binary file - smaller (*.wcb))"));
    if (filenameToSave.isEmpty())
        return; //save cancelled

    if (filenameToSave.contains('.') == false)
        filenameToSave.append(defaultFileExt);

    QFileInfo fileToSaveInfo(filenameToSave);
    //based on file extension, call appropriate saving routine
    if      (fileToSaveInfo.suffix() == "wca")
        saveWCA(filenameToSave);
    else if (fileToSaveInfo.suffix() == "chml")
        Event::writeAllToXml(filenameToSave);
    //else if (fileToSaveInfo.suffix() == "funscript")
    //    exportFunscript(filenameToSave); //
    else
        //TODO: implement WCB format
        QMessageBox::warning(this,
                             tr("File format not implemented"),
                             tr("Sorry, Sillyface hasn't programmed that file type yet"));

    if ( ! loadedVideo.isEmpty())
    {
        associateFiles(loadedVideo, filenameToSave);
    }
}

QString MainWindow::getAssociatedFile(const QString &filename)
{
    settings.beginGroup("associatedFiles");
    QString associatedFile = settings.value(filename,QString()).toString();
    settings.endGroup();
    return associatedFile;
}

QString MainWindow::getMatchingFileByName(const QString &filename)
{
    QFileInfo info(filename);
    int iLikeThat = 0;
    QString retVal("");
    for (QFileInfo possibleMatch : info.dir().entryInfoList())
    {
        if (possibleMatch.isFile() && possibleMatch.baseName() == info.baseName())
        {
            //we have a file that matches on baseName
            if (isVideoFileSuffix(info.suffix()))
            {
                //we're looking for a matching script
                int iLikeThis = isScriptFileSuffix(possibleMatch.suffix());
                if (iLikeThis > iLikeThat)
                {
                    retVal = possibleMatch.filePath();
                    iLikeThat = iLikeThis;
                }
            }
            else
            {
                //we're looking for a video file
                if (isVideoFileSuffix(possibleMatch.suffix()))
                    return possibleMatch.filePath();
            }
        }
    }
    return retVal;
}

void MainWindow::associateFiles(const QString & video, const QString & script)
{
    settings.beginGroup("associatedFiles");
    settings.setValue(script, video);
    settings.setValue(video, script);
    settings.endGroup();
}

void MainWindow::saveWCA(QString filenameToSave)
{
    QFile fileToSave(filenameToSave);
    if ( ! fileToSave.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,
                             tr("Save failed"),
                             tr("Could not open file for writing."));
        return;
    }
    for (int i = 0; i < ui->eventsTable->model()->rowCount(); ++i)
    {
        Event eventToWrite = eventFromTableRow(i);
        fileToSave.write(eventToWrite.toAsciiString().toLatin1());
        fileToSave.write("\n");
    }
    fileToSave.close();
    return;
}

QString funscriptEntryString(int targetLocation, long timestamp)
{
    return QString("{\"pos\":%1,\"at\":%2}").arg(targetLocation).arg(timestamp);
}

void MainWindow::exportFunscript(bool fullSpeed)
{
    if (ui->eventsTable->model()->rowCount() == 0)
        return; //don't save empty file

    QString exportFilename = QFileDialog::getSaveFileName(this, tr("Export to FunScript"),
                                                "~/Documents",
                                                tr("Funscripts (*.funscript)"));
    if (exportFilename.isEmpty())
        return; //save cancelled

    QFile fileToSave(exportFilename);
    if ( ! fileToSave.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,
                             tr("Save failed"),
                             tr("Save failed"));
        return;
    }

    FunscriptWriter writer(fileToSave);

    if (fullSpeed)
        exportFullSpeed(writer);
    else
        exportHalfSpeed(writer);

    fileToSave.close();
}

int combineFullAndProportionalStrokeLengths(float fullLength, float proportionalLength)
{
    int fixedSpeedProportion = OptionsDialog::getStrokeLengthProportion();
    int fullLengthProportion = 100 - fixedSpeedProportion;
    return (fullLengthProportion * fullLength + fixedSpeedProportion * proportionalLength) / 100;
}

void MainWindow::exportHalfSpeed(SyncFileWriter & writer)
{
    bool up = false;
    const int topOfRange = OptionsDialog::getRangeTop();
    const int bottomOfRange = OptionsDialog::getRangeBottom();
    const int availableRange = topOfRange - bottomOfRange;
    const int midRange = (topOfRange + bottomOfRange) / 2;
    const float intensityScale = 2.0;
    const int halfRange = availableRange / 2;

    writer.writeHeader();

    for (int i = 0; i < ui->eventsTable->model()->rowCount(); ++i)
    {
        Event eventToWrite = eventFromTableRow(i);
        if (eventToWrite.type() == 15)
        {
            //writing funcript events to funscripts is easy :-)
            writer.addEvent(eventToWrite.timestamp, eventToWrite.value);
            continue;
        }
        else if (eventToWrite.type() != 1)
        {
            continue;
        }
        //A proportion (0..1) indicating how far from the centre of the range the stroke should move (after scaling)
        //Without scaling, an event with a value of 255 would go all the way to the edge of the range, 128 would go about half way, 64 about a quarter etc.
        //With a scaling factor of 2.0, all events of value 128 and above would use the full range (it's capped - you can't go off the end!), 64 is about half range etc.
        float strokeIntensity = std::min( ((double) eventToWrite.value * intensityScale) / 255, 1.0);
        //This is the full length (from the centre point) a stroke of this intensity would travel (assuming there is time for it)
        float fullStrokeDistance = strokeIntensity * halfRange;

        //when proportional stroke length is enabled, the distance of the stroke (from the centre) is proportional to the duration of beat
        //e.g. longer beats tend to allow time for the hand to travel, so it's more natural for the long beat of a 123 - 123 to stroke further.
        //while for a shorter stroke, there might only be time for a quick 'flick of the wrist'
        float proportionalMultiplier = calculateProportionOfFullStrokeDuration(i, i+1);
        float proportionalDistance = fullStrokeDistance * proportionalMultiplier;
        int finalStrokeDistanceFromCentre = combineFullAndProportionalStrokeLengths(fullStrokeDistance, proportionalDistance);

        //without 'anchoring', we can calculate exactly where to send the stroker at this point:
        int targetLocation = midRange;
        if (up)
            targetLocation = midRange + finalStrokeDistanceFromCentre;
        else
            targetLocation = midRange - finalStrokeDistanceFromCentre;
        //'anchoring' means that all up strokes would go all the way to the top or bottom of the stroker, and points away from the anchor
        //are calculated relative to the anchor instead of the mid-point. However, imagine the following:
        //we're anchored to the bottom and must do a soft stroke. As we're already at the bottom, where the anchor is, we move just
        //a small distance from the anchor. Then the next stroke is a strong stroke. We need to move back towards the anchor (as the
        //strokes alternate direction in half speed mode), but we've only moved a small distance away from it, so we can't move very far
        //or we'd go off the bottom of the stroker.
        //Thus we can't really honor the anchor settings when in half speed mode.

        writer.addEvent(eventToWrite.timestamp, targetLocation);
        up = !up;
    }
    writer.writeFooter();
    return;
}

void MainWindow::exportHandyCsv()
{
    if (ui->eventsTable->model()->rowCount() == 0)
        return; //don't save empty file

    QString exportFilename = QFileDialog::getSaveFileName(this, tr("Export to Handy CSV"),
                                                "~/Documents",
                                                tr("Comma-separated values (*.csv)"));
    if (exportFilename.isEmpty())
        return; //save cancelled

    QFile fileToSave(exportFilename);
    if ( ! fileToSave.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this,
                             tr("Save failed"),
                             tr("Unable to open file for writing."));
        return;
    }

    HandyCsvWriter writer (fileToSave);

    if (OptionsDialog::useFullStrokes())
        exportFullSpeed(writer);
    else
        exportHalfSpeed(writer);

    fileToSave.close();
}
void MainWindow::exportTemporaryHandyCsv()
{
    temporaryHandyCsvFile.open();
    temporaryHandyCsvFile.resize(0);
    HandyCsvWriter writer (temporaryHandyCsvFile);
    if (OptionsDialog::useFullStrokes())
        exportFullSpeed(writer);
    else
        exportHalfSpeed(writer);
    ++tempHandyScriptExportCounter;
    temporaryHandyCsvFile.close();
}

//!
//! \brief calculateProportionOfFullStrokeDuration will calculate how much of a full stroke can be achieved in
//!        the space between the given events, assuming the stroker moves as ''normal' speed
//! \param startEventIndex the index of the beginning event
//! \param endEventIndex the index of the ending event
//! \return a proportion of a full stroke
//!
float MainWindow::calculateProportionOfFullStrokeDuration(int startEventIndex, int endEventIndex)
{
    int fullStrokeLength = OptionsDialog::handyFullStrokeDuration();
    if (startEventIndex < 0)
    {
        //we're at the beginning of the range, so assume 0 is earlier bound
        long endingBound = eventFromTableRow(endEventIndex).timestamp;
        return std::min ((float) endingBound / (float) fullStrokeLength, 1.0f);
    } else if (endEventIndex >= ui->eventsTable->model()->rowCount())
        //we're at the end of the range, assume 1
        return 1.0f;

    //normal case
    long start = eventFromTableRow(startEventIndex).timestamp;
    long end = eventFromTableRow(endEventIndex).timestamp;
    long length = end - start;
    return std::min(1.0f, (float) length / (float) fullStrokeLength);
}

void MainWindow::exportFullSpeed(SyncFileWriter & writer)
{
    const int topOfRange = OptionsDialog::getRangeTop();
    const int bottomOfRange = OptionsDialog::getRangeBottom();
    const int availableRange = topOfRange - bottomOfRange;
    const float intensityScale = 2.0;
    int holdLocation = bottomOfRange + (availableRange / 2);

    writer.writeHeader();
    for (int i = 0; i < ui->eventsTable->model()->rowCount(); ++i)
    {
        Event eventToWrite = eventFromTableRow(i);
        if (eventToWrite.type() == 15)
        {
            //writing funcript events to funscripts is easy :-)
            writer.addEvent(eventToWrite.timestamp, eventToWrite.value);
            continue;
        }
        else if (eventToWrite.type() != 1)
        {
            continue;
        }

        //A proportion (0..1) indicating how much of the available range the stroke should use (after scaling)
        //Without scaling, an event with a value of 255 would use the full range, 128 would use about half, 64 about a quarter etc.
        //With a scaling factor of 2.0, all events of value 128 and above would use the full range (it's capped - you can't go off the end!), 64 is about half range etc.
        float strokeIntensity = std::min( ((double) eventToWrite.value * intensityScale) / 255, 1.0);
        //This is the full length of a stroke of this intensity (assuming there is time for it)
        float fullStrokeDistance = strokeIntensity * availableRange;

        //when proportional stroke length is enabled, the height (i.e. top) of a bottom-anchored stroke is proportional to the duration of beat leading upto it.
        //e.g. for a whole beat, there might be time for a full up-and-down stroke, but for a quarter of a beat, there
        //might only be time for a quick 'flick of the wrist'
        float baProportionalMultiplier = calculateProportionOfFullStrokeDuration(i-1, i);
        float baProportionalDistance = fullStrokeDistance * baProportionalMultiplier;
        int finalBaStrokeDistance = combineFullAndProportionalStrokeLengths(fullStrokeDistance, baProportionalDistance);

        //when proportional stroke length is enabled, the height (i.e. bottom) of a top-anchored stroke is proportional to the duration of beat following it.
        //e.g. longer beats tend to allow time for the hand to travel, so it's more natural for the long beat of a 123 - 123 to stroke further down the shaft.
        //For a shorter beat, there might only be time for a quick 'flick of the wrist'
        float taProportionalMultiplier = calculateProportionOfFullStrokeDuration(i, i+1);
        float taProportionalDistance = fullStrokeDistance * taProportionalMultiplier;
        int finalTaStrokeDistance = combineFullAndProportionalStrokeLengths(fullStrokeDistance, taProportionalDistance);

        //first, calculate the bounds of a 'bottom-anchored' (ba) stroke:
        int baBottom = bottomOfRange;
        int baTop = bottomOfRange + finalBaStrokeDistance;

        //not work out the bounds of a 'top-anchored' (ta) stroke:
        int taTop = topOfRange;
        int taBottom = topOfRange - finalTaStrokeDistance;

        //then combine them according to the preferences:
        int topAnchoredAmount = OptionsDialog::getRangeAnchor();
        int bottomAnchoredAmount = 100 - topAnchoredAmount;
        int topOfStroke = ( (topAnchoredAmount * taTop) + (bottomAnchoredAmount * baTop) ) / (topAnchoredAmount + bottomAnchoredAmount);
        int bottomOfStroke = ( (topAnchoredAmount * taBottom) + (bottomAnchoredAmount * baBottom) ) / (topAnchoredAmount + bottomAnchoredAmount);

        addExtraCsvEvents(i, holdLocation, topOfStroke, bottomOfStroke, writer);
        holdLocation = bottomOfStroke;
    }
    writer.writeFooter();
    return;
}

void MainWindow::addExtraFunscriptEvents(int eventIndex, int upLocation, int downLocation, QFile& fileToSave) {
    Event event = eventFromTableRow(eventIndex);
    bool eventNeededForEndOfHold = false;
    long endOfHoldtimestamp = event.timestamp - OptionsDialog::handyFullStrokeDuration();
    if (eventIndex == 0) {
        //special case for the first beat
        if (event.timestamp < 2) {
            return; //not long enough to work with
        }
        if (endOfHoldtimestamp > 0) {
            eventNeededForEndOfHold = true;
        } else {
            endOfHoldtimestamp = 0;
        }
    }
    else
    {
        //normal sitch
        Event olderEvent = eventFromTableRow(eventIndex - 1);
        if (endOfHoldtimestamp > olderEvent.timestamp) {
            eventNeededForEndOfHold = true;
        } else {
            endOfHoldtimestamp = olderEvent.timestamp;
        }
    }
    long overallStrokeLength = event.timestamp - endOfHoldtimestamp;
    long upTimestamp = endOfHoldtimestamp + (overallStrokeLength / 2);
    if (eventNeededForEndOfHold)
    {
        QString endOfHoldEventString = funscriptEntryString(downLocation, endOfHoldtimestamp) + ",";
        fileToSave.write(endOfHoldEventString.toLatin1());
    }
    QString upEventString = funscriptEntryString(upLocation, upTimestamp) + ",";
    fileToSave.write(upEventString.toLatin1());
}

void MainWindow::addExtraCsvEvents(int eventIndex, int holdLocation, int upLocation, int downLocation, SyncFileWriter & writer) {
    Event event = eventFromTableRow(eventIndex);
    bool eventNeededForEndOfHold = false;
    long endOfHoldtimestamp = event.timestamp - OptionsDialog::handyFullStrokeDuration();
    if (eventIndex == 0) {
        //special case for the first beat
        if (event.timestamp < 2) {
            writer.addEvent(event.timestamp, downLocation);
            return; //not long enough to work with
        }
        if (endOfHoldtimestamp > 0) {
            eventNeededForEndOfHold = true;
        } else {
            endOfHoldtimestamp = 0;
            //could set eventNeededForEndOfHold to true here, but it's so close to the beginning of the script, what's the point?
        }
    }
    else
    {
        //normal sitch
        //this logic is pretty basic - it assumes the previous event was of the same type, and that there are no simultaneous events
        Event olderEvent = eventFromTableRow(eventIndex - 1);
        if (endOfHoldtimestamp > olderEvent.timestamp) {
            eventNeededForEndOfHold = true;
        } else {
            endOfHoldtimestamp = olderEvent.timestamp;
        }
    }
    long overallStrokeDuration = event.timestamp - endOfHoldtimestamp;
    long upTimestamp = endOfHoldtimestamp + (overallStrokeDuration / 2);

    if (eventNeededForEndOfHold)
        writer.addEvent(endOfHoldtimestamp, holdLocation);
    writer.addEvent(upTimestamp, upLocation);
    writer.addEvent(event.timestamp, downLocation);
}

void MainWindow::on_loadButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                            tr("Load File"),
                                            QDir::toNativeSeparators(QDir::homePath()), //TODO: remember last load location and use that instead?
                                            tr("All Openable Files (*.chml *.wca *.wcb *.mid *.funscript *.wmv *.mov *.avi *.mpg *.mp4 *.flv *.m4v *.wav);;All Wand Controller Event Files (*.chml *.wca *.wcb);;Cock Hero Markup Language files (*.chml);;Wand Controller ASCII files (*.wca);;Wand Controller Binary Files (*.wcb);;MIDI Files (*.mid);;Funscripts (*.funscripts);;Movie Files (*.wmv *.mov *.avi *.mpg *.mp4 *.flv *.m4v);;Sound Files (*.wav)"));

    if (filename.isEmpty())
        return;

    loadFile(filename);
}

void MainWindow::on_actionOpen_triggered()
{
    on_loadButton_clicked();
}

//!
//! \brief MainWindow::isScriptFileSuffix will return a non-zero integer if the given suffix is a known script file type
//! \param suffix
//! \return 0 if this isn't a script file, or a bigger number otherwise. The bigger the number, the more we prefer that filetype.
//!
int MainWindow::isScriptFileSuffix(const QString & suffix)
{
    int retVal = 1;

    if (suffix == "funscript")
        return retVal;
    ++retVal;

    if (suffix == "wcb")
        return retVal;
    ++retVal;

    if (suffix == "wca")
        return retVal;
    ++retVal;

    if (suffix == "chml")
        return retVal;
    ++retVal;

    return 0;
}

bool MainWindow::isVideoFileSuffix(const QString & suffix)
{
    return suffix == "wmv" ||
            suffix == "mov" ||
             suffix == "avi" ||
             suffix == "mpg" ||
             suffix == "mp4" ||
             suffix == "flv" ||
             suffix == "m4v" ;
}

void MainWindow::loadFile(QString filename, bool isAssociatedFile)
{
    if (! isAssociatedFile)
    {
        QString associatedFile = getAssociatedFile(filename);
        if ( ! associatedFile.isEmpty() )
        {
            if (QMessageBox::Yes == QMessageBox::question(this,
                                                       tr("Open Associated File?"),
                                                       tr("Would you also like to open ") + associatedFile + " ?",
                                                       QMessageBox::Yes,
                                                       QMessageBox::No))
                loadFile(associatedFile,true);
        }
        else
        {
            QString nameMatch = getMatchingFileByName(filename);
            if ( ! nameMatch.isEmpty() &&
                 QMessageBox::Yes == QMessageBox::question(this,
                                       tr("Open Matching File?"),
                                       tr("Would you also like to open ") + nameMatch + " ?",
                                       QMessageBox::Yes,
                                       QMessageBox::No))
                loadFile(nameMatch,true);
        }
        //TODO: Consider fuzzy filename matching to suggest best match - sometimes script names don't match the video
        //      Especially if the video contains bits like '720p' or '3dlr'
    }

    QFileInfo fileToLoad(filename);
    //based on file extension, call appropriate loader
    if      (fileToLoad.suffix() == "wca")
        loadWCA(filename);
    else if (fileToLoad.suffix() == "chml")
        loadCHML(filename);
    else if (fileToLoad.suffix() == "mid")
        importFromMidi(filename);
    else if (fileToLoad.suffix() == "funscript")
        importFunscript(filename);
    else if (fileToLoad.suffix() == "wav")
        loadStrokeSound(filename);
    else if (isVideoFileSuffix(fileToLoad.suffix()))
        loadVideo(filename);
    else
        //TODO: implement WCB format
        QMessageBox::warning(this,
                             tr("File format not implemented"),
                             tr("Sorry, Sillyface hasn't programmed that file type yet"));

    registerUnsyncedChanges();
}

void MainWindow::loadWCA(QString filenameToLoad)
{
    char guiRefreshDivisor = 0;
    QFile inputFile(filenameToLoad);
    inputFile.open(QFile::ReadOnly | QFile::Text);
    QTextStream input(&inputFile);
    QString thisLine = input.readLine(20);
    while ( ! thisLine.isNull() )
    {
        if (thisLine.startsWith('a', Qt::CaseInsensitive))
        {
            QStringList eventParts = thisLine.split(',',QString::SkipEmptyParts);
            if (eventParts.size() == 3      &&
                eventParts[2].endsWith('.') )
            {
                long timestamp = eventParts[0].remove(QChar('a'),Qt::CaseInsensitive).toLong();
                char type = (char) eventParts[1].toShort();
                short value = eventParts[2].remove('.').toShort();
                Event newEvent(timestamp,type,value);
                addEventToTable(newEvent);
                if (++guiRefreshDivisor % 32 == 0)
                    QCoreApplication::processEvents();
            }
        }
        thisLine = input.readLine(20);
    }
    loadedScript = filenameToLoad;
    updateAssociatedScriptIfAppropriate();
}

void MainWindow::loadCHML(QString filename)
{
    //use CHMLHandler to parse the CHML file element by element
    CHMLHandler handler;
    QFile inputFile (filename);
    QXmlInputSource chmlSource (&inputFile);

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);

    reader.parse(chmlSource);
    loadedScript = filename;
    updateAssociatedScriptIfAppropriate();
}

void MainWindow::loadVideo(QString video)
{
    loadedVideo = video;
    videoPlayer->setMedia(QUrl::fromLocalFile(video));
    updateAssociatedVideoIfAppropriate();
}

void MainWindow::updateAssociatedScriptIfAppropriate()
{
    if ( ! loadedVideo.isEmpty()) {
        QString associatedScript = getAssociatedFile(loadedVideo);
        if (associatedScript != loadedScript &&
                (associatedScript.isEmpty() ||
                 QMessageBox::Yes == QMessageBox::question(this,
                                                       tr("Update Associated Script?"),
                                                       QString(tr("Would you like to change the script associated with %1?\n\n Existing:\t%2\nNew:\t%3"))
                                                                                                                     .arg(loadedVideo)
                                                                                                                                       .arg(associatedScript)
                                                                                                                                                 .arg(loadedScript),
                                                       QMessageBox::Yes,
                                                       QMessageBox::No)
                 )
                )
        {
            associateFiles(loadedVideo, loadedScript);
        }
    }
}

void MainWindow::updateAssociatedVideoIfAppropriate()
{
    if ( ! loadedScript.isEmpty()) {
        QString associatedVideo = getAssociatedFile(loadedScript);
        if (associatedVideo != loadedVideo &&
                (associatedVideo.isEmpty() ||
                 QMessageBox::Yes == QMessageBox::question(this,
                                                       tr("Update Associated Video?"),
                                                       QString(tr("Would you like to change the video associated with %1?\n\n Existing: %2\nNew: %3"))
                                                                                                                     .arg(loadedScript)
                                                                                                                                       .arg(associatedVideo)
                                                                                                                                                 .arg(loadedVideo),
                                                       QMessageBox::Yes,
                                                       QMessageBox::No)
                 )
                )
        {
            associateFiles(loadedVideo, loadedScript);
        }
    }
}

void MainWindow::loadStrokeSound()
{
    strokeSoundSource = QFileDialog::getOpenFileName(this,
                                                       tr("Load Stroke Sound File"),
                                                       "~/Documents",
                                                       tr("Uncompressed PCM Wave Files (*.wav)"));
    if (strokeSoundSource.isEmpty())
        return;

    loadStrokeSound(strokeSoundSource);
}

void MainWindow::loadStrokeSound(QString sound)
{
    strokeSoundSource = sound;
    strokeSound.setSource(QUrl::fromLocalFile(strokeSoundSource));
}

void MainWindow::importFromMidi(QString filename)
{
    MidiFileReader * reader = new MidiFileReader(filename,this);
    reader->readFile();
}

void MainWindow::importFunscript(QString filename)
{
    QMessageBox importTypeQuestionBox;
    importTypeQuestionBox.setWindowTitle("Import Events as Beats?");
    importTypeQuestionBox.setText(tr("Cock Heroine is designed to work with beats and rhythms, which is how cock heroes work!\n\n"

                                      "Funscript files usually contain instructions for a particular type of 'stroker' hardware like the 'Handy', 'OSR', 'Launch' etc.\n"
                                      "Funscripts do not always work so well with other sorts of hardware, and they don't honour the settings in the preferences dialog.\n\n"

                                      "You should import the funscript as 'beats' if:\n"
                                      "- You want to use Cock Heroine to play, edit or optimise the beat patterns in a cock hero funscript, or\n"
                                      "- You want to convert a half speed script to a full speed script, or a full speed script to a half speed script\n\n"

                                      "Select 'half speed' if your funscript moves up on one beat and down on the next beat\n"
                                      "Select 'full speed' if your funscript moves up and down once for each beat\n\n"

                                      "You should import the 'raw data' if:\n"
                                      "- You just want to use Cock Heroine to play a non-cock-hero video with a script file\n\n"

                                      "How would you like to import the funscript?"));
    importTypeQuestionBox.addButton(tr("Beats (half speed)"),QMessageBox::AcceptRole);
    QPushButton * fullStrokesButton = importTypeQuestionBox.addButton(tr("Beats (full speed)"),QMessageBox::AcceptRole);
    QPushButton * rawDataButton = importTypeQuestionBox.addButton(tr("Raw Data"),QMessageBox::RejectRole);
    importTypeQuestionBox.setDefaultButton(fullStrokesButton);
    importTypeQuestionBox.exec();

    bool importActualFunscriptEvents = (importTypeQuestionBox.clickedButton() == rawDataButton);
    bool importFullStrokes = (importTypeQuestionBox.clickedButton() == fullStrokesButton);
    QFile funscript(filename, this);
    funscript.open(QIODevice::ReadOnly);
    if (importFullStrokes)
    {
        importFunscriptFullStrokes(funscript);
        return;
    }
    QString content = funscript.readAll();
    content = content.simplified();
    QRegularExpression re("{\\s*\"pos\"\\s*:\\s*(\\d+)\\s*,\\s*\"at\"\\s*:\\s*(\\d+)\\s*}");
    QRegularExpressionMatchIterator i = re.globalMatch(content);
    int counter = 0;
    int lastPos = 50;
    int comingFrom = 50;
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        int pos = match.captured(1).toInt();
        int diff = pos - lastPos;
        bool thisStrokeAscending = diff > 0;
        bool lastStrokeAscending = lastPos > comingFrom;
        bool continuingStroke = thisStrokeAscending == lastStrokeAscending;
        if (continuingStroke && ui->eventsTable->model()->rowCount() > 0)
        {
            eventsModel->removeRows(eventsModel->rowCount()-1,1);
            diff = pos - comingFrom;
        }
        else
        {
            comingFrom = lastPos;
        }
        int posDiff = abs(diff);
        int posValue = convertStrokeLengthToIntensity(posDiff);
        int absoluteValue = 128;
//        int relativeBias = 7;
//        int absoluteBias = 1;
        int relativeBias = 0;
        int absoluteBias = 1;
        int value = ( (posValue * relativeBias) + (absoluteValue * absoluteBias) ) / (relativeBias + absoluteBias);

        QString timestampString = match.captured(2);
        long timestamp = timestampString.toLong();
        Event newEvent;
        if (importActualFunscriptEvents)
        {
            newEvent = Event(timestamp,
                             15, /*(sure, why not? don't think it's taken)*/
                             pos);
        }
        else
        {
            newEvent = Event(timestamp,
                     1,
                     value);
        }
        addEventToTable(newEvent);
        lastPos = pos;
        if (++counter % 100 == 0)
            QCoreApplication::processEvents();
    }
    if (eventsModel->rowCount() == 0)
    {
        //Repeat the process with the 'pos' and 'at' switched
        QRegularExpression reswitch("{\\s*\"at\"\\s*:\\s*(\\d+)\\s*,\\s*\"pos\"\\s*:\\s*(\\d+)\\s*}");
        QRegularExpressionMatchIterator i2 = reswitch.globalMatch(content);
        while (i2.hasNext())
        {
            QRegularExpressionMatch match = i2.next();
            int pos = match.captured(2).toInt();
            int diff = pos - lastPos;
            bool thisStrokeAscending = diff > 0;
            bool lastStrokeAscending = lastPos > comingFrom;
            bool continuingStroke = thisStrokeAscending == lastStrokeAscending;
            if (continuingStroke && ui->eventsTable->model()->rowCount() > 0)
            {
                eventsModel->removeRows(eventsModel->rowCount()-1,1);
                diff = pos - comingFrom;
            }
            else
            {
                comingFrom = lastPos;
            }
            int posDiff = abs(diff);
            int posValue = convertStrokeLengthToIntensity(posDiff);
            int absoluteValue = 128;
            int relativeBias = 7;
            int absoluteBias = 1;
            int value = ( (posValue * relativeBias) + (absoluteValue * absoluteBias) ) / (relativeBias + absoluteBias);

            QString timestampString = match.captured(1);
            long timestamp = timestampString.toLong();
            Event newEvent;
            if (importActualFunscriptEvents)
            {
                newEvent = Event(timestamp,
                                 15, /*(sure, why not? don't think it's taken)*/
                                 pos);
            }
            else
            {
                newEvent = Event(timestamp,
                                 1,
                                 value);
            }
            addEventToTable(newEvent);
            lastPos = pos;
            if (++counter % 100 == 0)
                QCoreApplication::processEvents();
        }
    }
    removeDuplicateEvents();
}

int MainWindow::convertStrokeLengthToIntensity(int length)
{
    return (length * 255) / 100;
}

void MainWindow::importFunscriptFullStrokes(QFile &funscript)
{
    auto fsContent = funscript.readAll();
    QJsonDocument fsDoc = QJsonDocument::fromJson(fsContent);
    if (!fsDoc.isObject()) {
        QMessageBox::warning(this,"Malformed funscript", "Funscript content was not a Json Object");
        return;
    }
    QJsonObject fsObj = fsDoc.object();
    QJsonValue actionsVal = fsObj.value("actions");
    if (actionsVal.isUndefined() || !actionsVal.isArray()) {
        QMessageBox::warning(this,"Malformed funscript", "Funscript actions absent or not a Json array");
        return;
    }
    QJsonArray actionsArr = actionsVal.toArray();

    int lastPeak = 0;
    int lastPos = 0;
    int nextPos = 0;
    for (int i = 0; i < actionsArr.size(); ++i)
    {
        QJsonValue actionVal = actionsArr[i];
        if (!actionVal.isObject()) {
            QMessageBox::warning(this,"Malformed funscript", "Funscript action was not a json object");
            return;
        }
        QJsonValue atVal = actionVal.toObject().value("at");
        QJsonValue posVal = actionVal.toObject().value("pos");
        if (atVal.isUndefined() || posVal.isUndefined() || !atVal.isDouble() || !posVal.isDouble()) {
            QMessageBox::warning(this,"Malformed funscript", "Funscript action did not contain 'pos' and 'at'");
            return;
        }
        bool isLastWaypoint = (i >= actionsArr.size() - 1);
        QJsonValue nextPosVal;
        if (!isLastWaypoint)
        {
            QJsonValue nextActionVal = actionsArr[i + 1];
            if (!nextActionVal.isObject()) {
                QMessageBox::warning(this,"Malformed funscript", "Funscript action was not a json object");
                return;
            }
            nextPosVal = nextActionVal.toObject().value("pos");
            if (nextPosVal.isUndefined() || !nextPosVal.isDouble()) {
                QMessageBox::warning(this,"Malformed funscript", "Funscript action did not contain 'pos'");
                return;
            }
        }
        //now actually do the logic...
        long at = roundToInt(atVal.toDouble());
        int pos = roundToInt(posVal.toDouble());
        if (!isLastWaypoint)
        {
            nextPos = roundToInt(nextPosVal.toDouble());
        }

        if (i == 0)
        {
            //first waypoint - just set...
            lastPeak = pos;
            lastPos = pos;
            continue;
        }
        else
        {
            if (
                    lastPos > pos
                    &&
                    (isLastWaypoint || nextPos >= pos)
                )
            {
                //we've reached the end of a down stroke
                int strokeLength = lastPeak - pos;
                int strokeIntensity = convertStrokeLengthToIntensity(strokeLength);
                Event newEvent(at, 1, strokeIntensity);
                addEventToTable(newEvent);
                lastPeak = pos;
            }

            if (pos > lastPeak)
                lastPeak = pos;

        }
        lastPos = pos;
        pos = nextPos;
    }
}

void MainWindow::on_currentItemChanged(const QModelIndex & current, const QModelIndex &)
{
    if ( ! currentlyPlaying &&
         current != QModelIndex() &&
         current.column() == 1)
    {
        QModelIndex sourceIndex = eventsProxyModel->mapToSource(current);
        Event goHere = eventsModel->eventFromIndex(sourceIndex);
        seekToTimestamp(goHere.timestamp);
    }
}

void MainWindow::on_lcdNumber_overflow()
{
    //have a party?
}

void MainWindow::on_actionJump_To_Time_triggered()
{
    jumpToTime();
}

void MainWindow::exportToWav()
{
    if (strokeSound.status() != QSoundEffect::Ready)
        loadStrokeSound();

    if (strokeSound.source().isEmpty())
        return;

    if (ui->eventsTable->model()->rowCount() == 0)
        return; //don't save empty file

    QString exportFilename = QFileDialog::getSaveFileName(this, tr("Export to Wav File"),
                                                "~/Documents",
                                                tr("Uncompressed PCM Wave Files (*.wav)"));
    if (exportFilename.isEmpty())
        return; //save cancelled

    videoPlayer->stop();
    //temporarily unload the sound effect so we can access the file...
    strokeSound.setSource(QUrl());

    WaveFileExporter wavExporter(strokeSoundSource,exportFilename);
    if (wavExporter.inputFileIsValid())
    {
        int currentRow = 0;
        wavExporter.printDebugInformation();
        wavExporter.writeOutputHeader();
        while(currentRow < ui->eventsTable->model()->rowCount())
        {
            wavExporter.writeSoundAt(eventFromTableRow(currentRow).timestamp);
            ui->eventsTable->selectRow(currentRow);
            QCoreApplication::processEvents();
            ++currentRow;
        }
        wavExporter.closeFile();
    }
    //restore stroke sound
    loadStrokeSound(strokeSoundSource);
}

void MainWindow::on_actionExport_To_Audio_File_triggered()
{
    exportToWav();
}

#define FILLER_SECONDS 3
void MainWindow::exportToMidi()
{
    if (ui->eventsTable->model()->rowCount() == 0)
        return; //don't save empty file

    if (!midiCanUseTempo() &&
    QMessageBox::Cancel == QMessageBox::question(this,
                                              tr("Incomplete Tempo Data"),
                                              tr("There are gaps in the tempo map, so tempo cannot be correctly exported.\n"
                                                 "Output tempo will be constant, and strokes will likely not relate to the MIDI beat.\n"
                                                 "Press OK to continue, or Cancel to continue editing data."),
                                              QMessageBox::Ok,
                                              QMessageBox::Cancel))
    {
        reportTempoGaps();
        return; //user chose not to continue without tempo info
    }

    QString exportFilename = QFileDialog::getSaveFileName(this, tr("Export to MIDI File"),
                                                "~/Documents",
                                                tr("MIDI Files (*.mid)"));
    if (exportFilename.isEmpty())
        return; //save cancelled

    MidiFileWriter midiWriter(exportFilename);
    if (midiCanUseTempo())
    {
        int valueResolution = /*std::max(*/lowestCommonMultiple(listOfMetadataDenominators())/*,24)*/;
        midiWriter.writeHeader(false, valueResolution);
        midiWriter.writeTitle("Wand Controller Output");
        //midiWriter.writePatchChange(10,0);
        midiWriter.writeTimeSignature(4,4);
        //midiWriter.writeTempo((quint16)60);
        int currentRow = 0;
        float currentTempo = 0;
        int deltaTime = 0;
        if (events[currentRow].timestamp != 0)
        {
            //insert some time
            int timeToKill = events[currentRow].timestamp;
            if (timeToKill > FILLER_SECONDS * 1000)
            {
                midiWriter.writeTempo_usPerBeat(FILLER_SECONDS * 1000 * 1000);
                while (timeToKill > FILLER_SECONDS * 1000)
                {
                    deltaTime += valueResolution;
                    timeToKill -= FILLER_SECONDS * 1000;
                }
            }
            midiWriter.writeTempo_usPerBeat(timeToKill * 1000, deltaTime);
            deltaTime = valueResolution;
        }
        while(currentRow < ui->eventsTable->model()->rowCount())
        {
            int timeToKill = 0;
            bool nextBeatIsSimultaneous = (currentRow < ui->eventsTable->model()->rowCount() - 1 &&
                                           events[currentRow + 1].timestamp == events[currentRow].timestamp);

            if (currentRow < ui->eventsTable->model()->rowCount() - 1) //all except last beat
            {
                if (nextBeatIsSimultaneous)
                {
                    //don't do any tempo changing
                }
                else if (events[currentRow].metadata == NULL ||
                        events[currentRow].metadata->tempo == 0 ||
                        events[currentRow].metadata->valueNumerator == 0)
                {
                    //tempo unknown, prepare for filler beat(s)
                    timeToKill = events[currentRow + 1].timestamp - events[currentRow].timestamp;
                    if (timeToKill > FILLER_SECONDS * 1000)
                    {
                        midiWriter.writeTempo_usPerBeat(FILLER_SECONDS * 1000 * 1000, deltaTime);
                    }
                    else
                    {
                        midiWriter.writeTempo_usPerBeat(timeToKill * 1000, deltaTime);
                    }
                    deltaTime = 0;
                }
                else if (events[currentRow].metadata->tempo != currentTempo)
                {
                    //changing tempo...
                    currentTempo = events[currentRow].metadata->tempo;
                    midiWriter.writeTempo(events[currentRow].metadata->tempo, deltaTime);
                    deltaTime = 0;
                }
                //else tempo is the same as last time, no change needed
            }

            //actually write beat
            midiWriter.writeBeat_frameDelta(deltaTime);

            if (currentRow < ui->eventsTable->model()->rowCount() - 1) //once again - all except last beat
            {
                if (nextBeatIsSimultaneous)
                {
                    deltaTime = 0;
                }
                else if (timeToKill) //tempo unknown
                {
                    if (timeToKill > FILLER_SECONDS * 1000)
                    {
                        //tempo already set to 10s per beat, output tempo again after sufficient sets of 10s
                        deltaTime = 0;
                        while (timeToKill > FILLER_SECONDS * 1000)
                        {
                            deltaTime += valueResolution;
                            timeToKill -= FILLER_SECONDS * 1000;
                        }
                        midiWriter.writeTempo_usPerBeat(timeToKill * 1000, deltaTime);
                    }
                    //we're now at the beginning of a whole beat which lasts the amount of time until the next beat
                    deltaTime = valueResolution;
                }
                else //tempo is known, set the next thing to happen after the correct number of frames for this beat
                {
                    deltaTime = events[currentRow].metadata->valueNumerator * (valueResolution / events[currentRow].metadata->valueDenominator);
                }
            }
            ui->eventsTable->selectRow(currentRow);
            QCoreApplication::processEvents();
            ++currentRow;
        }
    }
    else //don't try to do the fancy schmancy tempo stuff.
    {
        midiWriter.writeHeader(true);
        midiWriter.writeTitle("Wand Controller Output");
        //midiWriter.writePatchChange(10,0);
        midiWriter.writeTimeSignature(4,4);
        midiWriter.writeTempo((quint16)60);
        int currentRow = 0;
        while(currentRow < ui->eventsTable->model()->rowCount())
        {
            midiWriter.writeBeat(eventFromTableRow(currentRow).timestamp);
            ui->eventsTable->selectRow(currentRow);
            QCoreApplication::processEvents();
            ++currentRow;
        }
    }
    midiWriter.writeEndOfTrack();
}

void MainWindow::on_actionSave_As_triggered()
{
    on_saveButton_clicked();
}

void MainWindow::on_actionExport_To_MIDI_File_triggered()
{
    exportToMidi();
}

void MainWindow::startMotorIdling()
{
    if ((arduinoConnection != NULL && arduinoConnection->isWritable()) || attemptToSetUpSerial())
    {
        arduinoConnection->write("i1");
    }
    else
    {
        unableToSetUpSerial();
    }
}

void MainWindow::stopMotorIdling()
{
    if (arduinoConnection != NULL && arduinoConnection->isWritable())
    {
        arduinoConnection->write("i0");
    }
}

//these are the values that work well for the setup with the original hitachi magic wand, but they can now be set through the UI.
#define STARTING_MIN_INTENSITY 0 //must be between 0 and 1023
#define ENDING_MIN_INTENSITY 414 //must be between 0 and 1023, and should be greater than STARTING_MIN_INTENSITY

short MainWindow::startingMinIntensity = STARTING_MIN_INTENSITY;
short MainWindow::endingMinIntensity = ENDING_MIN_INTENSITY;

void MainWindow::setMinIntensity()
{
    int intensity;
    qint64 duration = 0;
    if (!currentlyPlaying || endingMinIntensity <= startingMinIntensity)
    {
        intensity = startingMinIntensity;
    }
    else
    {
        duration = videoPlayer->duration();
        if (duration <= 0 && !events.isEmpty())
            duration = events.last().timestamp;
        if (duration <= 0)
            intensity = startingMinIntensity;
        else
        {
            int difference = endingMinIntensity - startingMinIntensity;
            long intermediate = currentTimecode() * difference;
            intensity = intermediate / duration;
            intensity += startingMinIntensity;
        }
    }
    if (arduinoConnection != NULL && arduinoConnection->isWritable())
    {
        QString intensityString = QString("n%1.").arg(intensity);
        qDebug() << "Intensity: " << intensityString;
        arduinoConnection->write(intensityString.toLatin1());
    }
    if (currentlyPlaying && endingMinIntensity > startingMinIntensity)
    {
        int timerLength = 500;
        long divisionLength = duration / (endingMinIntensity - startingMinIntensity);
        if (divisionLength)
        {
            long timeTilNextDivision = divisionLength - (currentTimecode() % divisionLength);
            timerLength = timeTilNextDivision;
        }
        setMinIntensityTimer->start(timerLength);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    OptionsDialog opts(this);
    opts.exec();
}

void MainWindow::on_actionObscure_Beat_Meter_toggled(bool obscure)
{
    settings.setValue("Obscure Beat Meter", obscure);

    if (obscure)
    {
        ui->videoGoesHere->setVisible(false);
        ui->graphicsView->setVisible(true);
        takeVideoOutput();
        updateBeatMeterMask();
    }
    else
    {
        ui->graphicsView->setVisible(false);
        ui->videoGoesHere->setVisible(true);
        takeVideoOutput();
    }

    //emit obscureMeterToggled(ui->actionObscure_Beat_Meter->isChecked());
}

void MainWindow::on_actionDo_a_thing_triggered()
{
    settings.setValue("Obscure Beat Meter", true);
    static GraphicsSceneVideoDialog * glag = new GraphicsSceneVideoDialog(this);
    glag->takeVideoOutput();
    //glag->showFullScreen();
    glag->show();
}

void MainWindow::updateBeatMeterMask(bool dark)
{
    double heightPercentage = settings.value("Beat Meter Width").toDouble();

    qreal width = videoScene->width();
    qreal totalHeight = videoScene->height();
    qreal height = (totalHeight * heightPercentage) / 100.0;
    if (maskingRectangle != NULL)
    {
        videoScene->removeItem(maskingRectangle);
        delete maskingRectangle;
        maskingRectangle = NULL;
    }

    if (settings.value("Obscure Beat Meter").toBool())
    {
        QColor color = dark ? QColor(Qt::black) : QColor(Qt::white);
        maskingRectangle = new QGraphicsRectItem(videoItem->boundingRect().left(),(videoItem->boundingRect().top() + totalHeight) - height,width,height,videoItem);
        QPen pen(color);
        pen.setWidth(1);
        maskingRectangle->setPen(pen);
        QBrush brush(color);
        maskingRectangle->setBrush(brush);
        maskingRectangle->setRect(videoItem->boundingRect().left(),(videoItem->boundingRect().top() + totalHeight) - height,width,height);
    }
}

QList<QAction *> * MainWindow::getActionsList() const
{
    return mainWindowActions;
}

//QList<CustomEventAction *> * MainWindow::getCustomShotcutsList()
//{
//    return customShortcuts;
//}

CustomEventAction * MainWindow::getExistingCustomAction(CustomEventAction * matchMe)
{
    for (CustomEventAction * existingAction : *customShortcuts)
        if (*existingAction == *matchMe)
            return existingAction;
    return nullptr;
}

void MainWindow::reregisterAllCustomShortcuts()
{
    for (CustomEventAction * action : *customShortcuts) {
        removeAction(action);
        delete(action);
    }
    customShortcuts->clear();
    QSettings settings;
    settings.beginGroup("CustomShortcuts");
    foreach (const QString &key, settings.childKeys()) {
        CustomEventAction * cea = CustomEventAction::fromString(key, this);
        if (cea)
        {
            QKeySequence sequence(settings.value(key).toString());
            CustomEventAction * existingAction = getExistingCustomAction(cea);
            if (!existingAction)
            {
                customShortcuts->append(cea);
                addAction(cea);
                existingAction = cea;
            }
            existingAction->addShortcut(sequence);
        }
    }
    settings.endGroup();
}

void MainWindow::on_actionConfigure_Keyboard_Shortcuts_triggered()
{
    KeyboardShortcutsDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionExport_To_FunScript_triggered()
{
    exportFunscript(true);
}

void MainWindow::on_actionExport_To_FunScript_half_speed_for_Launch_triggered()
{
    exportFunscript(false);
}

void MainWindow::on_shoulderTriggerValueChanged(double value)
{
    int intValue = (value + 1) * 512;
    int inverse = 1024 - intValue;
    qDebug() << "Requesting intensity" << inverse << ".";
    if (arduinoConnection != NULL && arduinoConnection->isWritable())
    {
        arduinoConnection->write("v");
        arduinoConnection->write(QString("%1").arg(inverse).toLocal8Bit());
        arduinoConnection->write(".");
    }
}


void MainWindow::on_actionInsert_interim_beats_triggered()
{
    for (int i = eventsModel->rowCount() - 1; i > 0; --i) {
        Event earlierEvent = eventFromTableRow(i-1);
        Event laterEvent = eventFromTableRow(i);
        long intervalLength = laterEvent.timestamp - earlierEvent.timestamp;
        long intermediateTimestamp;
//        if (intervalLength > MAXIMUM_MECHANICAL_STROKER_FULL_STROKE_TIME)
//            intermediateTimestamp = laterEvent.timestamp - (MAXIMUM_MECHANICAL_STROKER_FULL_STROKE_TIME / 2);
//        else
            intermediateTimestamp = earlierEvent.timestamp + (intervalLength / 2);

        Event intermediateEvent(intermediateTimestamp, earlierEvent.type(), earlierEvent.value);
        addEventToTable(intermediateEvent);
    }
}


void MainWindow::on_actionExport_to_Handy_CSV_triggered()
{
    exportHandyCsv();
}

void MainWindow::calculateHandyServerTimeOffset()
{
    totalHandyTimeDifferences = 0;
    completedHandyPingRequests = 0;
    requestHandyServerTime();
}

void MainWindow::requestHandyServerTime()
{
    handyPingTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    QNetworkRequest serverTimeRequest(QUrl(OptionsDialog::getHandyApiBase() + "getServerTime"));
    networkManager->get(serverTimeRequest);
}

void MainWindow::registerUnsyncedChanges()
{
    unsyncedChangesFlag = true;
}

bool MainWindow::unsyncedChangesMade()
{
    if (unsyncedChangesFlag)
    {
        unsyncedChangesFlag = false;
        return true;
    }
    return false;
}

#define HANDY_SCRIPT_UPLOAD_ADDRESS "https://www.handyfeeling.com/api/sync/upload"

void MainWindow::prepareHandySync()
{
    if (unsyncedChangesMade() || serverSideHandyScriptAddress.isEmpty())
    {
        exportAndUploadHandyCsv();
    }
    else
    {
        QString requestAddress = QString("%1syncPrepare?url=%2&name=%3&size=%4&timeout=60000")
                                         .arg(OptionsDialog::getHandyApiBase())
                                                            .arg(serverSideHandyScriptAddress)
                                                                    .arg(QString("CockHeroineTempFile%1").arg(tempHandyScriptExportCounter))
                                                                            .arg(serverSideHandyScriptSize);
        QNetworkRequest syncPrepareRequest((QUrl(requestAddress)));
        networkManager->get(syncPrepareRequest);
        setHandySyncOffset(OptionsDialog::handySyncBaseOffset() + playbackLatency);
    }
}

void MainWindow::exportAndUploadHandyCsv()
{
    exportTemporaryHandyCsv();
    serverSideHandyScriptAddress = QString();
    //need to get csv on server...
    temporaryHandyCsvFile.open();
    QNetworkRequest csvUploadRequest(QUrl(HANDY_SCRIPT_UPLOAD_ADDRESS));
    QHttpMultiPart * multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    csvUploadRequest.setHeader(QNetworkRequest::ContentTypeHeader,QByteArray("multipart/form-data; boundary=" + multiPart->boundary()));
    QHttpPart filePart;
    filePart.setBodyDevice(&temporaryHandyCsvFile);
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("application/vnd.ms-excel"));
    //name must=syncFile
    QString contentDisposition = QString("form-data; name=\"syncFile\"; filename=\"%1\"").arg(temporaryHandyCsvFile.fileName());
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, contentDisposition.toLatin1());
    multiPart->append(filePart);

    networkManager->post(csvUploadRequest, multiPart);
}

void MainWindow::startHandySync()
{
    QString requestAddress = QString("%1syncPlay?play=true&serverTime=%2&time=%3")
                                      .arg(OptionsDialog::getHandyApiBase())
                                                                      .arg(estimatedHandyServerTime())
                                                                              .arg(currentTimecode());
    QNetworkRequest syncPlayRequest((QUrl(requestAddress)));
    networkManager->get(syncPlayRequest);
}

void MainWindow::sendHandyTimecodeSync(bool absolute)
{
    if (!handyIsPlaying)
        return;

    QString requestAddress = QString("%1syncAdjustTimestamp?currentTime=%2&serverTime=%3&filter=%4")
                                      .arg(OptionsDialog::getHandyApiBase())
                                                                       .arg(currentTimecode())
                                                                                      .arg(estimatedHandyServerTime())
                                                                                                .arg(absolute? "1" : "0.5");
    QNetworkRequest syncAdjustTimestampRequest((QUrl(requestAddress)));
    networkManager->get(syncAdjustTimestampRequest);
}

void MainWindow::setHandySyncOffset(int offset)
{
    QString requestAddress = QString("%1syncOffset?offset=%2")
                                      .arg(OptionsDialog::getHandyApiBase())
                                                          .arg(offset);
    QNetworkRequest syncOffsetRequest((QUrl(requestAddress)));
    networkManager->get(syncOffsetRequest);
}

void MainWindow::stopHandySync()
{
    QString requestAddress = QString("%1syncPlay?play=false")
                                      .arg(OptionsDialog::getHandyApiBase());
    QNetworkRequest syncStopRequest((QUrl(requestAddress)));
    networkManager->get(syncStopRequest);
}

void MainWindow::httpResponseReceived(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    if (status != 200)
    {
        qDebug() << QString("Response received from %1 with code %2").arg(reply->url().toString()).arg(status);
    }
    if (reply->url().toString().contains("getServerTime"))
        processHandyServerTime(reply);
    else if (reply->url().toString().contains(HANDY_SCRIPT_UPLOAD_ADDRESS))
        storeServerSideHandyScriptLocation(reply);
    else if (reply->url().toString().contains("syncPrepare"))
        handleSyncPrepareResponse(reply);
    else if (reply->url().toString().contains("syncPlay"))
        handleSyncPlayResponse(reply);
    reply->deleteLater();
}

void MainWindow::processHandyServerTime(QNetworkReply * reply)
{
    qint64 handyPongTime = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    QByteArray bytes = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    double givenServerTime = doc.object().value("serverTime").toDouble();
    int roundTripTime = handyPongTime - handyPingTime;
    double estimatedServerTime = givenServerTime + (roundTripTime / 2);
    double difference = estimatedServerTime - handyPongTime;
    if (++completedHandyPingRequests)
    {
        totalHandyTimeDifferences += difference;

        averageHandyServerTimeOffset = totalHandyTimeDifferences / completedHandyPingRequests;
//        qDebug() << QString("This difference: %1 (average difference: %2)").arg(difference).arg(averageHandyServerTimeOffset);
    }
    if (shouldPingHandyAgain())
        requestHandyServerTime();
    else if (currentlyPlaying)
        prepareHandySync();
}

qint64 MainWindow::estimatedHandyServerTime()
{
    qint64 now = QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
    return now + averageHandyServerTimeOffset;
}

void MainWindow::storeServerSideHandyScriptLocation(QNetworkReply * reply)
{
    static int failedUploads = 0;
    QByteArray bytes = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    bool success = !doc.isNull();
    if (success)
        success = doc.object().value("success").toBool();
    if (!success)
    {
        qDebug() << "Handy script upload failure. Response received: " << bytes;
        if (++failedUploads <= 5)
            exportAndUploadHandyCsv();
        else
            qDebug() << "Too many failed upload attempts. Giving up. :-(";
        return;
    }

    failedUploads = 0;

    serverSideHandyScriptAddress = doc.object().value("url").toString();
    qDebug() << "script stored on server at " << serverSideHandyScriptAddress;
    serverSideHandyScriptSize = doc.object().value("size").toDouble();
//    qDebug() << "script size on server is " << serverSideHandyScriptSize;
    if (currentlyPlaying)
        prepareHandySync();
}

void MainWindow::handleSyncPrepareResponse(QNetworkReply * reply)
{
    QByteArray bytes = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    bool success = !doc.isNull();
    if (success)
        success = doc.object().value("success").toBool();
    if (!success)
    {
        qDebug() << "Failure. Response received: " << bytes;
        return;
    }
    //not much to do here...
    if (currentlyPlaying)
        startHandySync();
}

void MainWindow::handleSyncPlayResponse(QNetworkReply * reply)
{
    QByteArray bytes = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    bool success = !doc.isNull();
    if (success)
        success = doc.object().value("success").toBool();
    if (!success)
    {
        qDebug() << "Failure. Response received: " << bytes;
        return;
    }
    //not much to do here...
    handyIsPlaying = doc.object().value("playing").toBool();
}

void MainWindow::on_actionRecalculate_Handy_Server_Time_triggered()
{
    calculateHandyServerTimeOffset();
}

void MainWindow::on_actionLaunchEditor_triggered()
{
    launchEditor();
}

void MainWindow::on_actionExport_Beat_Meter_triggered()
{
    ExportBeatMeterDialog * ebmDialog = new ExportBeatMeterDialog(this);
    ebmDialog->exec();
    delete ebmDialog;
}
