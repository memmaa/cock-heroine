#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "event.h"
#include <QMainWindow>
#include <QSerialPort>
#include <QTime>
#include <QTimer>
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QTableWidgetItem>
#include "seektotimecodedialog.h"
#include "eventdatamodel.h"
#include "eventdataproxymodel.h"
#include <QSettings>
#include <QGraphicsScene>
#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include "buttplug/buttpluginterface.h"
#include "QAudioFormat"

QT_BEGIN_NAMESPACE
class QGamepad;
QT_END_NAMESPACE

#define EDGE_RELEASE_TIME 100
#define USER_EDGED_OUT_NOTIFY 2

class EditorWindow;
class CustomEventAction;
class SyncFileWriter;
class QAudioOutput;
class StimSignalGenerator;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent*);
    void handleResize();

    Event eventFromTableRow(int);
    Event eventFromIndex(QModelIndex index);
    Event createEventNow();

    void scheduleEvent();
    void cancelScheduledEvent();
    bool triggerEvent(Event &);
    void addEventToTable(Event &);
    void registerUnsyncedChanges();
    void scheduleVideoSync();

    long totalPlayTime();
    double progressThroughGame(long timestamp = currentTimecode());

    Event * getLastEventBefore(long timestamp, bool includeCurrent = true);
    //didn't work - need a global intervals list
//    BeatInterval * getIntervalAtTimestamp(long timestamp, bool preferNext = true);
    Event * getNextEventAfter(long timestamp, bool includeCurrent = false);

    QList<QAction *> *getActionsList() const;
    void reregisterAllCustomShortcuts();

    ButtplugInterface * getButtplugInterface();

    //used to increase intensity (such as strength of vibration, amplitude of estim) as time goes on
    static short startingMinIntensity;
    static short endingMinIntensity;

    QGraphicsScene * videoScene;
    QGraphicsVideoItem * videoItem;
    //!Used to hide the beat meter if you don't want to see it
    QGraphicsRectItem * maskingRectangle;
    QList<CustomEventAction *> * customShortcuts;

public slots:
    void skipForward();
    void skipBackward();
    void updateBeatMeterMask(bool dark = false);
    void takeVideoOutput();
    void toggleFullscreen();
    void httpResponseReceived(QNetworkReply *);
    void pressPlay();

signals:
    void obscureMeterToggled(bool enabled);


protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent (QMouseEvent * ev );

    Ui::MainWindow *ui;

private:

    bool attemptToSetUpSerial();
    void unableToSetUpSerial();

    void setVideoOutputDevice();
    void play();
    void pause();
    void stop();
    void startTimer();
    void sendSerialTimecodeSync();
    void stopTimer();
    void jumpToTime();
    bool launchEditor();
    QAudioFormat getEstimAudioFormat();
public:
    //literally only public so that globals.c:seekToTimestamp can reallign estim signal
    void startEstimSignal();
    void stopEstimSignal();

private:
    void createActions();
    CustomEventAction * getExistingCustomAction(CustomEventAction *);
    void removeDuplicateEvents();
    void clearEventsList();

    void setLastOpenedLocation(const QString path);
    QString getLastOpenedLocation();
    static bool isVideoFileSuffix(const QString & suffix);
    static int isScriptFileSuffix(const QString & suffix);
    QString getAssociatedFile(const QString & filename);
    QString getMatchingFileByName(const QString & filename);
    void associateFiles(const QString & video, const QString & script);
    void loadFile(QString, bool isAssociatedFile = false);
    void saveWCA(QString);
    void loadWCA(QString);
    void loadCHML(QString filename);
    void loadVideo(QString video);
    void loadStrokeSound(QString sound);
    void loadStrokeSound();
    void updateAssociatedScriptIfAppropriate();
    void updateAssociatedVideoIfAppropriate();
    void exportToWav();
    void exportToMidi();
    void importFromMidi(QString filename);
    void exportFunscript(bool fullSpeed);
    float calculateProportionOfFullStrokeDuration(int, int);
    void addExtraFunscriptEvents(int eventIndex, int upLocation, int downLocation, QFile& fileToSave);
    void addExtraCsvEvents(int eventIndex, int holdLocation, int upLocation, int downLocation, SyncFileWriter & writer);
    void exportHandyCsv();
    void exportTemporaryHandyCsv();
    void exportFullSpeed(SyncFileWriter &writer);
    void exportHalfSpeed(SyncFileWriter &writer);
    void importFunscript(QString filename);
    void importFunscriptFullStrokes(QFile & funscript);
    int convertStrokeLengthToIntensity(int);

    void calculateHandyServerTimeOffset();
    void requestHandyServerTime();
    void processHandyServerTime(QNetworkReply *);
    qint64 estimatedHandyServerTime();
    bool shouldPingHandyAgain(){return completedHandyPingRequests < 30;}
    bool unsyncedChangesMade();
    void prepareHandySync();
    void exportAndUploadHandyCsv();
    void startHandySync();
    void sendHandyTimecodeSync(bool absolute = false);
    void setHandySyncOffset(int);
    void stopHandySync();
    void storeServerSideHandyScriptLocation(QNetworkReply *);
    void handleSyncPrepareResponse(QNetworkReply *);
    void handleSyncPlayResponse(QNetworkReply *);

    QSettings settings;

    eventDataModel * eventsModel;
    EventDataProxyModel * eventsProxyModel;
    QString loadedVideo;
    QString loadedScript;
    QString strokeSoundSource;
    QSoundEffect strokeSound;
    const static QString defaultFileExt;

    bool connectedArduinoFound;
    QSerialPort * arduinoConnection;

    int enqueuedEvents;
    QTimer * scheduledEventTimer;
    Event scheduledEvent;

    SeekToTimecodeDialog * timecodeDialog;

    bool waitingForEdgeRelease;
    bool edgedOut;
    bool waitingForTimedEventRelease;
    QTimer * edgeReleaseTimer;
    QTimer * edgedOutTimer;
    QTimer * timedEventReleaseTimer;
    QTimer * motorIdleTimer;
    QTimer * setMinIntensityTimer;
    const static short edgeReleaseTime = EDGE_RELEASE_TIME;
    const static short edgedOutTime = EDGE_RELEASE_TIME;
    const static short timedEventReleaseTime = EDGE_RELEASE_TIME;

    QGamepad * gamepad;

    static bool extraVideoSync;
    static quint16 videoSyncDelay;
    static quint8 maxVideoSyncError;
    int playbackLatency;

    QList<QAction *> * mainWindowActions;

    qint64 handyPingTime;
    bool handyIsPlaying = false;
    char completedHandyPingRequests = 0;
    double totalHandyTimeDifferences = 0;
    int averageHandyServerTimeOffset = 0;
    bool unsyncedChangesFlag = false;
    QTemporaryFile temporaryHandyCsvFile;
    int tempHandyScriptExportCounter;
    QString serverSideHandyScriptAddress;
    int serverSideHandyScriptSize;
    QNetworkAccessManager * networkManager = new QNetworkAccessManager(this);
    ButtplugInterface * buttplugIF;

    StimSignalGenerator * stimSignalGenerator;
    QAudioOutput * stimAudio;

private slots:

    void updateTimerDisplay();
    void triggerScheduledEvent();

    void on_triggerButton_pressed();

    void on_triggerAndRecordButton_pressed();

    void on_recordButton_pressed();

    void on_startButton_pressed();

    void syncToVideo();

    void on_stopButton_pressed();

    void on_saveButton_clicked();

    void on_loadButton_clicked();

    void on_currentItemChanged(const QModelIndex &, const QModelIndex &);

    void on_lcdNumber_overflow();

    void stopEdging();
    void resumeEdging();
    void completeTimedEvent();
    void on_actionOpen_triggered();

    void on_actionJump_To_Time_triggered();

    void on_actionExport_To_Audio_File_triggered();

    void on_actionSave_As_triggered();

    void on_actionExport_To_MIDI_File_triggered();
    void startMotorIdling();
    void stopMotorIdling();
    void setMinIntensity();

    void on_actionPreferences_triggered();

    void on_actionObscure_Beat_Meter_toggled(bool obscure);

    void on_actionDo_a_thing_triggered();

    void on_actionConfigure_Keyboard_Shortcuts_triggered();

    void on_actionExport_To_FunScript_triggered();
    void on_shoulderTriggerValueChanged(double value);
    void on_actionInsert_interim_beats_triggered();
    void on_actionExport_To_FunScript_half_speed_for_Launch_triggered();
    void on_actionExport_to_Handy_CSV_triggered();
    void on_actionRecalculate_Handy_Server_Time_triggered();
    void on_actionLaunchEditor_triggered();
    void on_actionExport_Beat_Meter_triggered();
    void on_actionConnect_to_buttplug_server_triggered();
    void on_actionSearch_for_buttplug_devices_triggered();
    void on_actionDisconnect_from_Buttplug_Server_triggered();
    void on_actionConfigure_buttplug_devices_triggered();
    void on_actionNope_triggered();
    void handleEstimAudioStateChanged(QAudio::State state);
    void on_actionExport_E_Stim_Track_triggered();
};

#endif // MAINWINDOW_H
