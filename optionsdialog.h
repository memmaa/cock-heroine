#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QAudioDeviceInfo>

enum EstimSourceMode
{
    FROM_FILE,
    ON_THE_FLY,
    PREGENERATED
};

enum EstimSignalType
{
    MONO,
    STEREO,
    TRIPHASE,
    UNKNOWN
};

namespace Ui {
class OptionsDialog;
}

#define PREF_ESTIM_UP_DOWN_BEAT_STROKE_STYLE "Up-Down-Beat"
#define PREF_ESTIM_DOWN_BEAT_UP_STROKE_STYLE "Down-Beat-Up"
#define PREF_ESTIM_STARTS_ON_BEAT_STYLE "Cycle Starts on the Beat"
#define PREF_ESTIM_ENDS_ON_BEAT_STYLE "Cycle Ends on the Beat"

class OptionsDialog : public QDialog
{
    Q_OBJECT

private:
    //! The suffix added to audio devices that are saved as preferences, but unavailable at the time the prefs dialog is displayed
    static const QString unavailableSuffix;
    static QString makeUnavailableName(QString deviceName);
    static QString cleanDeviceName(QString name);
    static bool isAvailableAudioDevice(QString deviceName);
    QAudioDeviceInfo getAudioDeviceInfo(QString);
    QColor nowMarkerColor;
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void populateUi();
    virtual void accept();
    void setControlsFromPreferences();
    void setPreferencesFromControls();

    static void setLastOpenedLocation(const QString path);
    static QString getLastOpenedLocation();
    static bool autoLoadLastSession();
    static bool useDefaultAudioDeviceForVideo();
    static QString getVideoAudioOutputDeviceName();
    static int getVideoSeekInterval();
    static bool syncTimecodeOnPlay();
    static bool syncTimecodeOnSeek();
    static bool syncEstimWithTimecode();
    static int getTimecodeSyncInterval();

    static bool connectToArduino();

    static bool connectToHandy();
    static int handySyncBaseOffset();
    static int handyFullStrokeDuration();
    static int getStrokeLengthProportion();
    static QString getHandyApiBase();
    static bool useFullStrokes();
    static int getRangeTop();
    static int getRangeBottom();
    static int getRangeAnchor();
    static int getBeatMeterHeight();
    static int getBeatMeterWidth();
    static int getBeatMeterSpeed();
    static double getBeatMeterFrameRate();
    void setNowMarkerColorFrame(QColor & color);

    static bool useSquareBeatMarkers();
    static int getBeatMarkerHeight();
    static int getBeatMarkerWidth();

    static bool useSquareNowMarker();
    static int getNowMarkerHeight();
    static int getNowMarkerWidth();
    static int getNowMarkerOutlineWidth();
    static int getNowMarkerPulseAmount();


    static bool emitEstimSignal();
    static QString getEstimOutputDeviceName();
    static bool currentEstimDeviceIsAvailable();
    static QAudioDeviceInfo getEstimOutputDeviceInfo();
    static EstimSourceMode getEstimSourceMode();
    static QString getEstimSourceFilename();
    static int getEstimChannelCount();
    static EstimSignalType getEstimSignalType();
    static int getEstimSamplingRate();
    static QAudioFormat getEstimAudioFormat();
    static int getEstimTriphaseStartingFrequency();
    static int getEstimTriphaseEndingFrequency();
    static int getEstimLeftChannelStartingFrequency();
    static int getEstimLeftChannelEndingFrequency();
    static int getEstimRightChannelStartingFrequency();
    static int getEstimRightChannelEndingFrequency();
    static int getEstimTotalSignalGrowth();
    static int getEstimMaxStrokeLength();
    static int getEstimBeatFadeInTime();
    static int getEstimBeatFadeInAnticipationTime();
    static int getEstimBeatFadeOutTime();
    static int getEstimBeatFadeOutDelay();
    static int getEstimBoostShortStrokes();
    static QString getEstimTriphaseStrokeStyle();
    static QString getEstimLeftChannelStrokeStyle();
    static QString getEstimRightChannelStrokeStyle();
    static bool getEstimInvertStrokes();
    static int getEstimStartPlaybackFadeInTime();
    static int getEstimSignalPan();
    static double getEstimCompressorBiteTime();
    static int getEstimCompressorStrength();
    static double getEstimCompressorRelease();
    static double getEstimLeftChannelPeakPositionInCycle();
    static double getEstimRightChannelPeakPositionInCycle();
    static double getEstimLeftChannelTroughLevel();
    static double getEstimRightChannelTroughLevel();
    static double getEstimLeftChannelFadeTime();
    static double getEstimRightChannelFadeTime();
    static int getEstimDelay();
    //!
    //! \brief getEstimSettingsFilenameSuffix
    //! \return a string, suitable for use in filenames, that uniquely identifies
    //!         the estim preferences that were chosen when it was created. Used to
    //!         work out whether a previously exported stim file can be reused.
    //!
    static QString getEstimSettingsFilenameSuffix();

private slots:
    void on_startingIntensitySpinBox_valueChanged(int value);

    void on_endingIntensitySlider_valueChanged(int value);

    void on_endingIntensitySpinBox_valueChanged(int value);

    void on_startingIntensitySlider_valueChanged(int value);

    void on_obscureBottomScreenPortionWidthSlider_valueChanged(int value);

    void on_obscureBottomScreenPortionWidthSpinBox_valueChanged(double value);

    void on_connectToHandyCheckBox_stateChanged(int checkState);

    void on_syncOffsetSlider_valueChanged(int value);

    void on_syncOffsetSpinBox_valueChanged(int value);

    void on_bottomOfRangeSlider_valueChanged(int value);

    void on_bottomOfRangeSpinBox_valueChanged(int value);

    void on_topOfRangeSlider_valueChanged(int value);

    void on_topOfRangeSpinBox_valueChanged(int value);

    void on_estimEnabledCheckBox_stateChanged(int checkState);

    void on_estimSignalPanHorizontalSlider_valueChanged(int value);

    void on_estimSignalPanHorizontalSlider_sliderMoved(int position);

    void on_estimUpDownBeatRadioButton_toggled(bool checked);

    void on_estimDownBeatUpRadioButton_toggled(bool checked);

    void on_obscureBottomScreenPortionCheckBox_toggled(bool checked);

    void on_useDefaultVideoAudioDeviceCheckBox_toggled(bool checked);

    void on_meterChooseNowMarkerColourButton_clicked();

    void on_estimModeComboBox_currentTextChanged(const QString &arg1);

    void on_estimFilenameBrowseButton_clicked();

private:
    Ui::OptionsDialog *ui;
    QWidget * triggeringWidget;

    //easier than blocking signals
    bool settingPrefs;
};

#endif // OPTIONSDIALOG_H
