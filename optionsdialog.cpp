#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"
#include <QAudioDeviceInfo>
#include <QColorDialog>

#define DEFAULT_PREFS_PAGE 0
#define CURRENT_PREFS_PAGE "Current Preferences Dialog Tab Index"

#define PREF_BEAT_METER_HEIGHT "Beat Meter Height"
#define DEFAULT_PREF_BEAT_METER_HEIGHT 100
#define PREF_BEAT_METER_WIDTH "Beat Meter Width"
#define DEFAULT_PREF_BEAT_METER_WIDTH 1920
#define PREF_BEAT_METER_SPEED "Beat Meter Speed"
#define DEFAULT_PREF_BEAT_METER_SPEED 167
#define PREF_BEAT_METER_FRAME_RATE "Beat Meter Frame Rate"
#define DEFAULT_PREF_BEAT_METER_FRAME_RATE 29.97

#define BEAT_MARKER_SHAPE_RECTANGLE "Rectangle"
#define BEAT_MARKER_SHAPE_ELLIPSE "Ellipse"

#define PREF_NOW_MARKER_SHAPE "Now Marker Shape"
#define DEFAULT_PREF_NOW_MARKER_SHAPE BEAT_MARKER_SHAPE_RECTANGLE
#define PREF_NOW_MARKER_HEIGHT "Now Marker Height"
#define DEFAULT_PREF_NOW_MARKER_HEIGHT 28
#define PREF_NOW_MARKER_WIDTH "Now Marker Width"
#define DEFAULT_PREF_NOW_MARKER_WIDTH 28
#define PREF_NOW_MARKER_OUTLINE_WIDTH "Now Marker Outline Width"
#define DEFAULT_PREF_NOW_MARKER_OUTLINE_WIDTH 2
#define PREF_NOW_MARKER_COLOR "Now Marker Color"
#define DEFAULT_PREF_NOW_MARKER_COLOR "Red"
#define PREF_PULSE_NOW_MARKER "Pulse Now Marker"
#define DEFAULT_PREF_PULSE_NOW_MARKER 10

#define PREF_BEAT_MARKER_SHAPE "Beat Marker Shape"
#define DEFAULT_PREF_BEAT_MARKER_SHAPE BEAT_MARKER_SHAPE_ELLIPSE
#define PREF_BEAT_MARKER_HEIGHT "Beat Marker Height"
#define DEFAULT_PREF_BEAT_MARKER_HEIGHT 20
#define PREF_BEAT_MARKER_WIDTH "Beat Marker Width"
#define DEFAULT_PREF_BEAT_MARKER_WIDTH 20

//Video page
#define PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE "Use Default Audio Device for Video Output"
#define DEFAULT_PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE true
#define PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME "Alternative Audio Device for Video Output"
#define DEFAULT_PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME ""
#define PREF_VIDEO_SEEK_INTERVAL "Video Seek Interval In Seconds"
#define DEFAULT_PREF_VIDEO_SEEK_INTERVAL 5
#define PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY "Perform Extra Timecode Sync On Play"
#define DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY true
#define PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK "Perform Extra Timecode Sync On Seek"
#define DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK false
#define PREF_SYNC_ESTIM_WITH_TIMECODE "Perform Estim Sync When Syncing Timecode"
#define DEFAULT_PREF_SYNC_ESTIM_WITH_TIMECODE true
#define PREF_TIMECODE_SYNC_INTERVAL "Timecode Sync Interval In Milliseconds"
#define DEFAULT_PREF_TIMECODE_SYNC_INTERVAL 200
#define PREF_LOAD_PREVIOUS_SESSION "Load Previous Session on Start-up"
#define DEFAULT_PREF_LOAD_PREVIOUS_SESSION true

//Arduino page
#define PREF_CONNECT_TO_ARDUINO "Connect To Arduino"
#define DEFAULT_PREF_CONNECT_TO_ARDUINO true

#define PREF_ESTIM_ENABLED "Enable Estim"
#define DEFAULT_PREF_ESTIM_ENABLED false
#define PREF_ESTIM_OUTPUT_DEVICE "Estim Output Device"
#define DEFAULT_PREF_ESTIM_OUTPUT_DEVICE ""
#define PREF_ESTIM_SAMPLE_RATE "Estim Sample Rate"
#define DEFAULT_PREF_ESTIM_SAMPLE_RATE "11025"
#define PREF_ESTIM_FREQUENCY_START "Estim Frequency At Start"
#define DEFAULT_PREF_ESTIM_FREQUENCY_START 500
#define PREF_ESTIM_FREQUENCY_END "Estim Frequency At End"
#define DEFAULT_PREF_ESTIM_FREQUENCY_END 400
#define PREF_ESTIM_VOL_INCREASE "Estim Overall Volume Increase"
#define DEFAULT_PREF_ESTIM_VOL_INCREASE 50
#define PREF_ESTIM_MAX_STROKE_DURATION "Estim Max Stroke Duration"
#define DEFAULT_PREF_ESTIM_MAX_STROKE_DURATION 500
#define PREF_ESTIM_BEAT_FADE_IN_TIME "Estim Beat Fade-In Time"
#define DEFAULT_PREF_ESTIM_BEAT_FADE_IN_TIME 1000
#define PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION "Estim Beat Fade-In Anticipation"
#define DEFAULT_PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION 500
#define PREF_ESTIM_BEAT_FADE_OUT_TIME "Estim Beat Fade-Out Time"
#define DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_TIME 1000
#define PREF_ESTIM_BEAT_FADE_OUT_DELAY "Estim Beat Fade-Out Delay"
#define DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_DELAY 0
#define PREF_ESTIM_SHORT_STROKES_BOOST "Estim Short Strokes Boost Amount"
#define DEFAULT_PREF_ESTIM_SHORT_STROKES_BOOST 5
#define PREF_ESTIM_STROKE_STYLE "Estim Stroke Style"
#define DEFAULT_PREF_ESTIM_STROKE_STYLE PREF_ESTIM_UP_DOWN_BEAT_STROKE_STYLE
#define PREF_ESTIM_INVERT_STROKES "Invert Estim Strokes"
#define DEFAULT_PREF_ESTIM_INVERT_STROKES false
#define PREF_ESTIM_START_FADE_IN_TIME "Estim Fade-In Time On Playback Start"
#define DEFAULT_PREF_ESTIM_START_FADE_IN_TIME 1500
#define PREF_ESTIM_PANNING "Estim L-R Panning Value"
#define DEFAULT_PREF_ESTIM_PANNING 0
#define PREF_ESTIM_COMPRESSOR_BITE_TIME "Estim Compressor Bite Time"
#define DEFAULT_PREF_ESTIM_COMPRESSOR_BITE_TIME 10.0
#define PREF_ESTIM_COMPRESSOR_STRENGTH "Estim Compressor Strength"
#define DEFAULT_PREF_ESTIM_COMPRESSOR_STRENGTH 5
#define PREF_ESTIM_COMPRESSOR_RELEASE_TIME "Estim Compressor Release Time"
#define DEFAULT_PREF_ESTIM_COMPRESSOR_RELEASE_TIME 10.0

QString OptionsDialog::cleanDeviceName(QString name)
{
    if (name.endsWith(unavailableSuffix))
        name.chop(unavailableSuffix.length());
    return name;
}

bool OptionsDialog::currentEstimDeviceIsAvailable()
{
    return isAvailableAudioDevice(getEstimOutputDeviceName());
}

bool OptionsDialog::isAvailableAudioDevice(QString deviceName)
{
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &deviceInfo : deviceInfos)
    {
        QString thisName = deviceInfo.deviceName();
        if (thisName == deviceName)
            return true;
    }
    return false;
}

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    triggeringWidget(NULL)
{
    ui->setupUi(this);
    populateUi();
    setControlsFromPreferences();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

const QString OptionsDialog::unavailableSuffix = QObject::tr(" (unavailable)");

QString OptionsDialog::makeUnavailableName(QString deviceName)
{
    return deviceName + unavailableSuffix;
}

void OptionsDialog::populateUi()
{
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &deviceInfo : deviceInfos)
    {
        ui->estimDeviceComboBox->addItem(deviceInfo.deviceName());
        ui->alternativeVideoAudioDeviceComboBox->addItem(deviceInfo.deviceName());
    }
    QString savedDeviceName = getEstimOutputDeviceName();
    if (!isAvailableAudioDevice(savedDeviceName))
    {
        ui->estimDeviceComboBox->addItem(makeUnavailableName(savedDeviceName));
    }
    ui->estimSampleRateComboBox->addItem("8000");
    ui->estimSampleRateComboBox->addItem("11025");
    ui->estimSampleRateComboBox->addItem("16000");
    ui->estimSampleRateComboBox->addItem("22050");
    ui->estimSampleRateComboBox->addItem("44100");
    ui->estimSampleRateComboBox->addItem("48000");
}

void OptionsDialog::setControlsFromPreferences()
{
    settingPrefs = true;
    QSettings settings;
    //video page
    ui->obscureBottomScreenPortionWidthSlider->setValue(settings.value("Obscure Bottom Screen Portion").toDouble() * 100);
    ui->obscureBottomScreenPortionWidthSpinBox->setValue(settings.value("Obscure Bottom Screen Portion").toDouble());
    ui->useDefaultVideoAudioDeviceCheckBox->setChecked(settings.value(PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE, DEFAULT_PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE).toBool());
    ui->alternativeVideoAudioDeviceComboBox->setCurrentText(settings.value(PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME, DEFAULT_PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME).toString());
    ui->seekIntervalSpinBox->setValue(settings.value(PREF_VIDEO_SEEK_INTERVAL, DEFAULT_PREF_VIDEO_SEEK_INTERVAL).toInt());
    ui->performExtraTimecodeSyncOnPlayCheckBox->setChecked(settings.value(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY, DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY).toBool());
    ui->performExtraTimecodeSyncOnSeekCheckBox->setChecked(settings.value(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK, DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK).toBool());
    ui->performExtraEstimSyncCheckBox->setChecked(settings.value(PREF_SYNC_ESTIM_WITH_TIMECODE, DEFAULT_PREF_SYNC_ESTIM_WITH_TIMECODE).toBool());
    ui->syncIntervalSpinBox->setValue(settings.value(PREF_TIMECODE_SYNC_INTERVAL, DEFAULT_PREF_TIMECODE_SYNC_INTERVAL).toInt());
    ui->autoLoadLastCheckBox->setChecked(settings.value(PREF_LOAD_PREVIOUS_SESSION, DEFAULT_PREF_LOAD_PREVIOUS_SESSION).toBool());

    //arduiuno page
    ui->connectToArduinoCheckBox->setChecked(settings.value(PREF_CONNECT_TO_ARDUINO, DEFAULT_PREF_CONNECT_TO_ARDUINO).toBool());
    ui->startingIntensitySlider->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->startingIntensitySpinBox->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->endingIntensitySlider->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());
    ui->endingIntensitySpinBox->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());

    //handy page
    ui->handyKeyEdit->setText(settings.value("Handy Connection Key", "NO_KEY").toString());
    ui->syncOffsetSpinBox->setValue(settings.value("Handy Sync Offset", 0).toInt());
    ui->syncOffsetSlider->setValue(settings.value("Handy Sync Offset", 0).toInt());
    ui->topOfRangeSlider->setValue(settings.value("Handy Range Top", 100).toInt());
    ui->bottomOfRangeSlider->setValue(settings.value("Handy Range Bottom", 0).toInt());
    ui->rangeAnchorSlider->setValue(settings.value("Handy Range Anchor", 0).toInt());
    ui->handyStrokeDurationSpinBox->setValue(settings.value("Handy Full Stroke Duration", 420).toInt());
    ui->handyScalingProportionSlider->setValue(settings.value("Handy Scaling Proportion", 0).toInt());
    ui->connectToHandyCheckBox->setChecked(settings.value("Connect to Handy",false).toBool());
    ui->fullStrokeRadioButton->setChecked(settings.value("Full Strokes", true).toBool());
    ui->halfStrokeRadioButton->setChecked( ! ui->fullStrokeRadioButton->isChecked());

    //beat meter page
    ui->meterHeightSpinBox->setValue(settings.value(PREF_BEAT_METER_HEIGHT, DEFAULT_PREF_BEAT_METER_HEIGHT).toInt());
    ui->meterWidthSpinBox->setValue(settings.value(PREF_BEAT_METER_WIDTH, DEFAULT_PREF_BEAT_METER_WIDTH).toInt());
    ui->meterSpeedSpinBox->setValue(settings.value(PREF_BEAT_METER_SPEED, DEFAULT_PREF_BEAT_METER_SPEED).toInt());
    ui->meterFrameRateSpinBox->setValue(settings.value(PREF_BEAT_METER_FRAME_RATE, DEFAULT_PREF_BEAT_METER_FRAME_RATE).toDouble());

    ui->meterMarkerRoundRadioButton->setChecked(settings.value(PREF_BEAT_MARKER_SHAPE, DEFAULT_PREF_BEAT_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_ELLIPSE);
    ui->meterMarkerSquareRadioButton->setChecked(settings.value(PREF_BEAT_MARKER_SHAPE, DEFAULT_PREF_BEAT_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_RECTANGLE);
    ui->meterMarkerWidthSpinBox->setValue(settings.value(PREF_BEAT_MARKER_WIDTH, DEFAULT_PREF_BEAT_MARKER_WIDTH).toInt());
    ui->meterMarkerHeightSpinBox->setValue(settings.value(PREF_BEAT_MARKER_HEIGHT, DEFAULT_PREF_BEAT_MARKER_HEIGHT).toInt());

    ui->meterNowMarkerRoundRadioButton->setChecked(settings.value(PREF_NOW_MARKER_SHAPE, DEFAULT_PREF_NOW_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_ELLIPSE);
    ui->meterNowMarkerSquareRadioButton->setChecked(settings.value(PREF_NOW_MARKER_SHAPE, DEFAULT_PREF_NOW_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_RECTANGLE);
    ui->meterNowMarkerWidthSpinBox->setValue(settings.value(PREF_NOW_MARKER_WIDTH, DEFAULT_PREF_NOW_MARKER_WIDTH).toInt());
    ui->meterNowMarkerHeightSpinBox->setValue(settings.value(PREF_NOW_MARKER_HEIGHT, DEFAULT_PREF_NOW_MARKER_HEIGHT).toInt());
    nowMarkerColor.setNamedColor(settings.value(PREF_NOW_MARKER_COLOR, DEFAULT_PREF_NOW_MARKER_COLOR).toString());
    setNowMarkerColorFrame(nowMarkerColor);
    ui->meterPulseNowMarkerSpinBox->setValue(settings.value(PREF_PULSE_NOW_MARKER, DEFAULT_PREF_PULSE_NOW_MARKER).toInt());

    //estim page
    ui->estimEnabledCheckBox->setChecked(settings.value(PREF_ESTIM_ENABLED, DEFAULT_PREF_ESTIM_ENABLED).toBool());
    QString deviceName = settings.value(PREF_ESTIM_OUTPUT_DEVICE, DEFAULT_PREF_ESTIM_OUTPUT_DEVICE).toString();
    if (!isAvailableAudioDevice(deviceName))
        deviceName = makeUnavailableName(deviceName);
    ui->estimDeviceComboBox->setCurrentText(deviceName);
    int sampleRate = settings.value(PREF_ESTIM_SAMPLE_RATE, DEFAULT_PREF_ESTIM_SAMPLE_RATE).toInt();
    QString sampleRateString = QString::number(sampleRate);
    ui->estimSampleRateComboBox->setCurrentText(sampleRateString);
    ui->estimStartingFrequencySpinbox->setValue(settings.value(PREF_ESTIM_FREQUENCY_START, DEFAULT_PREF_ESTIM_FREQUENCY_START).toInt());
    ui->estimEndingFrequencySpinbox->setValue(settings.value(PREF_ESTIM_FREQUENCY_END, DEFAULT_PREF_ESTIM_FREQUENCY_END).toInt());
    ui->estimTotalSignalGrowthSpinBox->setValue(settings.value(PREF_ESTIM_VOL_INCREASE, DEFAULT_PREF_ESTIM_VOL_INCREASE).toInt());
    ui->estimMaxStrokeLengthSpinBox->setValue(settings.value(PREF_ESTIM_MAX_STROKE_DURATION, DEFAULT_PREF_ESTIM_MAX_STROKE_DURATION).toInt());
    ui->estimBeatFadeInTimeSpinBox->setValue(settings.value(PREF_ESTIM_BEAT_FADE_IN_TIME, DEFAULT_PREF_ESTIM_BEAT_FADE_IN_TIME).toInt());
    ui->estimBeatFadeInAnticipationTimeSpinBox->setValue(settings.value(PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION, DEFAULT_PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION).toInt());
    ui->estimBeatFadeOutTimeSpinBox->setValue(settings.value(PREF_ESTIM_BEAT_FADE_OUT_TIME, DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_TIME).toInt());
    ui->estimBeatFadeOutDelaySpinBox->setValue(settings.value(PREF_ESTIM_BEAT_FADE_OUT_DELAY, DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_DELAY).toInt());
    ui->estimBoostShortStrokesSpinBox->setValue(settings.value(PREF_ESTIM_SHORT_STROKES_BOOST, DEFAULT_PREF_ESTIM_SHORT_STROKES_BOOST).toInt());
    if (settings.value(PREF_ESTIM_STROKE_STYLE, DEFAULT_PREF_ESTIM_STROKE_STYLE).toString() == PREF_ESTIM_UP_DOWN_BEAT_STROKE_STYLE)
    {
        ui->estimUpDownBeatRadioButton->setChecked(true);
        ui->estimDownBeatUpRadioButton->setChecked(false);
    }
    else
    {
        ui->estimUpDownBeatRadioButton->setChecked(false);
        ui->estimDownBeatUpRadioButton->setChecked(true);
    }
    ui->estimInvertStrokesCheckBox->setChecked(settings.value(PREF_ESTIM_INVERT_STROKES, DEFAULT_PREF_ESTIM_INVERT_STROKES).toBool());
    ui->estimStartPlaybackFadeInTimeSpinBox->setValue(settings.value(PREF_ESTIM_START_FADE_IN_TIME, DEFAULT_PREF_ESTIM_START_FADE_IN_TIME).toInt());
    ui->estimSignalPanHorizontalSlider->setValue(settings.value(PREF_ESTIM_PANNING, DEFAULT_PREF_ESTIM_PANNING).toInt());
    ui->estimCompressorBiteTimeSpinBox->setValue(settings.value(PREF_ESTIM_COMPRESSOR_BITE_TIME, DEFAULT_PREF_ESTIM_COMPRESSOR_BITE_TIME).toDouble());
    ui->estimCompressorStrengthSpinBox->setValue(settings.value(PREF_ESTIM_COMPRESSOR_STRENGTH, DEFAULT_PREF_ESTIM_COMPRESSOR_STRENGTH).toInt());
    ui->estimCompressorReleaseSpinBox->setValue(settings.value(PREF_ESTIM_COMPRESSOR_RELEASE_TIME, DEFAULT_PREF_ESTIM_COMPRESSOR_RELEASE_TIME).toDouble());

    ui->tabWidget->setCurrentIndex(settings.value(CURRENT_PREFS_PAGE, DEFAULT_PREFS_PAGE).toInt());
    settingPrefs = false;
}

void OptionsDialog::setPreferencesFromControls()
{
    QSettings settings;

    settings.setValue(CURRENT_PREFS_PAGE, ui->tabWidget->currentIndex());

    //video page
    settings.setValue(PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE, ui->useDefaultVideoAudioDeviceCheckBox->isChecked());
    settings.setValue(PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME, ui->alternativeVideoAudioDeviceComboBox->currentText());
    settings.setValue("Obscure Bottom Screen Portion", ui->obscureBottomScreenPortionWidthSpinBox->value());
    settings.setValue(PREF_VIDEO_SEEK_INTERVAL, ui->seekIntervalSpinBox->value());
    //!why would you turn this off?
    settings.setValue(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY, ui->performExtraTimecodeSyncOnPlayCheckBox->isChecked());
    settings.setValue(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK, ui->performExtraTimecodeSyncOnSeekCheckBox->isChecked());
    settings.setValue(PREF_SYNC_ESTIM_WITH_TIMECODE, ui->performExtraEstimSyncCheckBox->isChecked());
    //!how often should we try to re-sync the clock to the video when we first start playing?
    settings.setValue(PREF_TIMECODE_SYNC_INTERVAL, ui->syncIntervalSpinBox->value());
    settings.setValue(PREF_LOAD_PREVIOUS_SESSION, ui->autoLoadLastCheckBox->isChecked());

    //arduino page
    settings.setValue(PREF_CONNECT_TO_ARDUINO, ui->connectToArduinoCheckBox->isChecked());
    settings.setValue("Starting Min Intensity", ui->startingIntensitySpinBox->value());
    settings.setValue("Ending Min Intensity", ui->endingIntensitySlider->value());

    //handy page
    settings.setValue("Connect To Handy", ui->connectToHandyCheckBox->isChecked());
    settings.setValue("Handy Connection Key", ui->handyKeyEdit->text());
    settings.setValue("Handy Sync Offset", ui->syncOffsetSpinBox->value());
    settings.setValue("Handy Range Top", ui->topOfRangeSlider->value());
    settings.setValue("Handy Range Bottom", ui->bottomOfRangeSlider->value());
    settings.setValue("Handy Range Anchor", ui->rangeAnchorSlider->value());
    settings.setValue("Handy Full Stroke Duration", ui->handyStrokeDurationSpinBox->value());
    settings.setValue("Handy Scaling Proportion", ui->handyScalingProportionSlider->value());
    settings.setValue("Full Strokes", ui->fullStrokeRadioButton->isChecked());

    //beat meter page
    settings.setValue(PREF_BEAT_METER_HEIGHT, ui->meterHeightSpinBox->value());
    settings.setValue(PREF_BEAT_METER_WIDTH, ui->meterWidthSpinBox->value());
    settings.setValue(PREF_BEAT_METER_SPEED, ui->meterSpeedSpinBox->value());
    settings.setValue(PREF_BEAT_METER_FRAME_RATE, ui->meterFrameRateSpinBox->value());

    if (ui->meterMarkerSquareRadioButton->isChecked())
        settings.setValue(PREF_BEAT_MARKER_SHAPE, BEAT_MARKER_SHAPE_RECTANGLE);
    else
        settings.setValue(PREF_BEAT_MARKER_SHAPE, BEAT_MARKER_SHAPE_ELLIPSE);
    settings.setValue(PREF_BEAT_MARKER_WIDTH, ui->meterMarkerWidthSpinBox->value());
    settings.setValue(PREF_BEAT_MARKER_HEIGHT, ui->meterMarkerHeightSpinBox->value());

    if (ui->meterNowMarkerSquareRadioButton->isChecked())
        settings.setValue(PREF_NOW_MARKER_SHAPE, BEAT_MARKER_SHAPE_RECTANGLE);
    else
        settings.setValue(PREF_NOW_MARKER_SHAPE, BEAT_MARKER_SHAPE_ELLIPSE);
    settings.setValue(PREF_NOW_MARKER_WIDTH, ui->meterNowMarkerWidthSpinBox->value());
    settings.setValue(PREF_NOW_MARKER_HEIGHT, ui->meterNowMarkerHeightSpinBox->value());
    settings.setValue(PREF_NOW_MARKER_COLOR, nowMarkerColor);
    settings.setValue(PREF_PULSE_NOW_MARKER, ui->meterPulseNowMarkerSpinBox->value());

    //estim page
    settings.setValue(PREF_ESTIM_ENABLED, ui->estimEnabledCheckBox->isChecked());
    settings.setValue(PREF_ESTIM_OUTPUT_DEVICE, cleanDeviceName(ui->estimDeviceComboBox->currentText()));
    QString sampleRate = ui->estimSampleRateComboBox->currentText();
    settings.setValue(PREF_ESTIM_SAMPLE_RATE, sampleRate.toInt());
    settings.setValue(PREF_ESTIM_FREQUENCY_START, ui->estimStartingFrequencySpinbox->value());
    settings.setValue(PREF_ESTIM_FREQUENCY_END, ui->estimEndingFrequencySpinbox->value());
    settings.setValue(PREF_ESTIM_VOL_INCREASE, ui->estimTotalSignalGrowthSpinBox->value());
    settings.setValue(PREF_ESTIM_MAX_STROKE_DURATION, ui->estimMaxStrokeLengthSpinBox->value());
    settings.setValue(PREF_ESTIM_BEAT_FADE_IN_TIME, ui->estimBeatFadeInTimeSpinBox->value());
    settings.setValue(PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION, ui->estimBeatFadeInAnticipationTimeSpinBox->value());
    settings.setValue(PREF_ESTIM_BEAT_FADE_OUT_TIME, ui->estimBeatFadeOutTimeSpinBox->value());
    settings.setValue(PREF_ESTIM_BEAT_FADE_OUT_DELAY, ui->estimBeatFadeOutDelaySpinBox->value());
    settings.setValue(PREF_ESTIM_SHORT_STROKES_BOOST, ui->estimBoostShortStrokesSpinBox->value());
    if (ui->estimUpDownBeatRadioButton->isChecked())
    {
        settings.setValue(PREF_ESTIM_STROKE_STYLE, PREF_ESTIM_UP_DOWN_BEAT_STROKE_STYLE);
    }
    else
    {
        settings.setValue(PREF_ESTIM_STROKE_STYLE, PREF_ESTIM_DOWN_BEAT_UP_STROKE_STYLE);
    }
    settings.setValue(PREF_ESTIM_INVERT_STROKES, ui->estimInvertStrokesCheckBox->isChecked());
    settings.setValue(PREF_ESTIM_START_FADE_IN_TIME, ui->estimStartPlaybackFadeInTimeSpinBox->value());
    settings.setValue(PREF_ESTIM_PANNING, ui->estimSignalPanHorizontalSlider->value());
    settings.setValue(PREF_ESTIM_COMPRESSOR_BITE_TIME, ui->estimCompressorBiteTimeSpinBox->value());
    settings.setValue(PREF_ESTIM_COMPRESSOR_STRENGTH, ui->estimCompressorStrengthSpinBox->value());
    settings.setValue(PREF_ESTIM_COMPRESSOR_RELEASE_TIME, ui->estimCompressorReleaseSpinBox->value());
}

bool OptionsDialog::autoLoadLastSession()
{
    QSettings settings;
    return settings.value(PREF_LOAD_PREVIOUS_SESSION, DEFAULT_PREF_LOAD_PREVIOUS_SESSION).toBool();
}


void OptionsDialog::accept()
{
    MainWindow::startingMinIntensity = ui->startingIntensitySpinBox->value();
    MainWindow::endingMinIntensity = ui->endingIntensitySpinBox->value();
    setPreferencesFromControls();
    mainWindow->registerUnsyncedChanges();
    qDebug() << "Proportion set to " << getStrokeLengthProportion();
    QDialog::accept();
}

void OptionsDialog::on_startingIntensitySpinBox_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->startingIntensitySpinBox;
        ui->startingIntensitySlider->setValue(value);
        if (ui->endingIntensitySlider->value() <= value)
        {
            ui->endingIntensitySlider->setValue(value + 1);
            ui->endingIntensitySpinBox->setValue(value + 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_endingIntensitySlider_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->endingIntensitySlider;
        ui->endingIntensitySpinBox->setValue(value);
        if (ui->startingIntensitySlider->value() >= value)
        {
            ui->startingIntensitySlider->setValue(value - 1);
            ui->startingIntensitySpinBox->setValue(value - 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_endingIntensitySpinBox_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->endingIntensitySpinBox;
        ui->endingIntensitySlider->setValue(value);
        if (ui->startingIntensitySlider->value() >= value)
        {
            ui->startingIntensitySlider->setValue(value - 1);
            ui->startingIntensitySpinBox->setValue(value - 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_startingIntensitySlider_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->startingIntensitySlider;
        ui->startingIntensitySpinBox->setValue(value);
        if (ui->endingIntensitySlider->value() <= value)
        {
            ui->endingIntensitySlider->setValue(value + 1);
            ui->endingIntensitySpinBox->setValue(value + 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_obscureBottomScreenPortionWidthSlider_valueChanged(int value)
{
    ui->obscureBottomScreenPortionWidthSpinBox->setValue(value / 100.0);
}

void OptionsDialog::on_obscureBottomScreenPortionWidthSpinBox_valueChanged(double value)
{
    ui->obscureBottomScreenPortionWidthSlider->setValue(value * 100);
}

bool OptionsDialog::connectToHandy()
{
    QSettings settings;
    return settings.value("Connect To Handy", false).toBool();
}

int OptionsDialog::handySyncBaseOffset()
{
    QSettings settings;
    return settings.value("Handy Sync Offset", 0).toInt();
}

int OptionsDialog::handyFullStrokeDuration()
{
    QSettings settings;
    return settings.value("Handy Full Stroke Duration", 420).toInt();
}
//!
//! \brief OptionsDialog::getStrokeLengthProportion
//! \return integer percentage - 0 is full stroke length (and try to increase speed to maintain this),
//!         					 100 is constant speed, reducing stroke length as needed
//!
int OptionsDialog::getStrokeLengthProportion()
{
    QSettings settings;
    return settings.value("Handy Scaling Proportion", 0).toInt();
}

QString OptionsDialog::getHandyApiBase()
{
    QSettings settings;
    QString serverLoc = settings.value("Handy Server Location", QString("https://www.handyfeeling.com/api/v1")).toString();
    QString connectionKey = settings.value("Handy Connection Key", QString("DEFAULT")).toString();
    return QString("%1/%2/").arg(serverLoc).arg(connectionKey);
}

bool OptionsDialog::useFullStrokes()
{
    QSettings settings;
    return settings.value("Full Strokes", true).toBool();
}

int OptionsDialog::getRangeTop()
{
    QSettings settings;
    return settings.value("Handy Range Top", 100).toInt();
}

int OptionsDialog::getRangeBottom()
{
    QSettings settings;
    return settings.value("Handy Range Bottom", 0).toInt();
}

int OptionsDialog::getRangeAnchor()
{
    QSettings settings;
    return settings.value("Handy Range Anchor", 0).toInt();
}

void OptionsDialog::on_connectToHandyCheckBox_stateChanged(int checkState)
{
    bool enabled = (checkState == Qt::Checked);
    ui->handyKeyEdit->setEnabled(enabled);
    ui->syncOffsetSlider->setEnabled(enabled);
    ui->syncOffsetSpinBox->setEnabled(enabled);
    ui->handyScalingProportionSlider->setEnabled(enabled);
    ui->handyStrokeDurationSpinBox->setEnabled(enabled);

}

void OptionsDialog::on_syncOffsetSlider_valueChanged(int value)
{
    ui->syncOffsetSpinBox->setValue(value);
}

void OptionsDialog::on_syncOffsetSpinBox_valueChanged(int value)
{
    ui->syncOffsetSlider->setValue(value);
}

void OptionsDialog::on_bottomOfRangeSlider_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->bottomOfRangeSlider;
        ui->bottomOfRangeSpinBox->setValue(value);
        if (ui->topOfRangeSlider->value() <= value)
        {
            ui->topOfRangeSlider->setValue(value + 1);
            ui->topOfRangeSpinBox->setValue(value + 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_bottomOfRangeSpinBox_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->bottomOfRangeSpinBox;
        ui->bottomOfRangeSlider->setValue(value);
        if (ui->topOfRangeSlider->value() <= value)
        {
            ui->topOfRangeSlider->setValue(value + 1);
            ui->topOfRangeSpinBox->setValue(value + 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_topOfRangeSlider_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->topOfRangeSlider;
        ui->topOfRangeSpinBox->setValue(value);
        if (ui->bottomOfRangeSlider->value() >= value)
        {
            ui->bottomOfRangeSlider->setValue(value - 1);
            ui->bottomOfRangeSpinBox->setValue(value - 1);
        }
        triggeringWidget = NULL;
    }
}

void OptionsDialog::on_topOfRangeSpinBox_valueChanged(int value)
{
    if (triggeringWidget == NULL) {
        triggeringWidget = ui->topOfRangeSpinBox;
        ui->topOfRangeSlider->setValue(value);
        if (ui->bottomOfRangeSlider->value() >= value)
        {
            ui->bottomOfRangeSlider->setValue(value - 1);
            ui->bottomOfRangeSpinBox->setValue(value - 1);
        }
        triggeringWidget = NULL;
    }
}

int OptionsDialog::getBeatMeterHeight()
{
    QSettings settings;
    return settings.value(PREF_BEAT_METER_HEIGHT, DEFAULT_PREF_BEAT_METER_HEIGHT).toInt();
}

int OptionsDialog::getBeatMeterWidth()
{
    QSettings settings;
    return settings.value(PREF_BEAT_METER_WIDTH, DEFAULT_PREF_BEAT_METER_WIDTH).toInt();
}

int OptionsDialog::getBeatMeterSpeed()
{
    QSettings settings;
    return settings.value(PREF_BEAT_METER_SPEED, DEFAULT_PREF_BEAT_METER_SPEED).toInt();
}

double OptionsDialog::getBeatMeterFrameRate()
{
    QSettings settings;
    return settings.value(PREF_BEAT_METER_FRAME_RATE, DEFAULT_PREF_BEAT_METER_FRAME_RATE).toDouble();
}

bool OptionsDialog::useSquareBeatMarkers()
{
    QSettings settings;
    return settings.value(PREF_BEAT_MARKER_SHAPE, DEFAULT_PREF_BEAT_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_RECTANGLE;
}

int OptionsDialog::getBeatMarkerWidth()
{
    QSettings settings;
    return settings.value(PREF_BEAT_MARKER_WIDTH, DEFAULT_PREF_BEAT_MARKER_WIDTH).toInt();
}

int OptionsDialog::getBeatMarkerHeight()
{
    QSettings settings;
    return settings.value(PREF_BEAT_MARKER_HEIGHT, DEFAULT_PREF_BEAT_MARKER_HEIGHT).toInt();
}

bool OptionsDialog::useSquareNowMarker()
{
    QSettings settings;
    return settings.value(PREF_NOW_MARKER_SHAPE, DEFAULT_PREF_NOW_MARKER_SHAPE).toString() == BEAT_MARKER_SHAPE_RECTANGLE;
}

int OptionsDialog::getNowMarkerWidth()
{
    QSettings settings;
    return settings.value(PREF_NOW_MARKER_WIDTH, DEFAULT_PREF_NOW_MARKER_WIDTH).toInt();
}

int OptionsDialog::getNowMarkerOutlineWidth()
{
    QSettings settings;
    return settings.value(PREF_NOW_MARKER_OUTLINE_WIDTH, DEFAULT_PREF_NOW_MARKER_OUTLINE_WIDTH).toInt();
}

int OptionsDialog::getNowMarkerHeight()
{
    QSettings settings;
    return settings.value(PREF_NOW_MARKER_HEIGHT, DEFAULT_PREF_NOW_MARKER_HEIGHT).toInt();
}

int OptionsDialog::getNowMarkerPulseAmount()
{
    QSettings settings;
    return settings.value(PREF_PULSE_NOW_MARKER, DEFAULT_PREF_PULSE_NOW_MARKER).toInt();
}

bool OptionsDialog::emitEstimSignal()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_ENABLED, DEFAULT_PREF_ESTIM_ENABLED).toBool();
}

bool OptionsDialog::useDefaultAudioDeviceForVideo()
{
    QSettings settings;
    return settings.value(PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE, DEFAULT_PREF_USE_DEFAULT_VIDEO_AUDIO_OUTPUT_DEVICE).toBool();
}

QString OptionsDialog::getVideoAudioOutputDeviceName()
{
    QSettings settings;
    return settings.value(PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME, DEFAULT_PREF_ALTERNATIVE_VIDEO_AUDIO_OUTPUT_DEVICE_NAME).toString();
}

int OptionsDialog::getVideoSeekInterval()
{
    QSettings settings;
    return settings.value(PREF_VIDEO_SEEK_INTERVAL, DEFAULT_PREF_VIDEO_SEEK_INTERVAL).toInt();
}

bool OptionsDialog::syncTimecodeOnPlay()
{
    QSettings settings;
    return settings.value(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY, DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_PLAY).toBool();
}

bool OptionsDialog::syncTimecodeOnSeek()
{
    QSettings settings;
    return settings.value(PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK, DEFAULT_PREF_PERFORM_EXTRA_VIDEO_SYNC_ON_SEEK).toBool();
}

bool OptionsDialog::syncEstimWithTimecode()
{
    QSettings settings;
    return settings.value(PREF_SYNC_ESTIM_WITH_TIMECODE, DEFAULT_PREF_SYNC_ESTIM_WITH_TIMECODE).toBool();
}

int OptionsDialog::getTimecodeSyncInterval()
{
    QSettings settings;
    return settings.value(PREF_TIMECODE_SYNC_INTERVAL, DEFAULT_PREF_TIMECODE_SYNC_INTERVAL).toInt();
}

bool OptionsDialog::connectToArduino()
{
    QSettings settings;
    return settings.value(PREF_CONNECT_TO_ARDUINO, DEFAULT_PREF_CONNECT_TO_ARDUINO).toBool();
}

QString OptionsDialog::getEstimOutputDeviceName()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_OUTPUT_DEVICE, DEFAULT_PREF_ESTIM_OUTPUT_DEVICE).toString();
}

//!
//! \brief OptionsDialog::getEstimOutputDeviceInfo Call currentEstimDeviceIsAvailable to check that return value will be useful
//! \return
//!
QAudioDeviceInfo OptionsDialog::getEstimOutputDeviceInfo()
{
    QString deviceName = getEstimOutputDeviceName();
    QAudioDeviceInfo retVal;
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &deviceInfo : deviceInfos)
    {
        QString thisName = deviceInfo.deviceName();
        if (thisName == deviceName)
            return deviceInfo;
    }
    return retVal; //this is empty, and not useful
}

int OptionsDialog::getEstimSamplingRate()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_SAMPLE_RATE, DEFAULT_PREF_ESTIM_SAMPLE_RATE).toInt();
}

int OptionsDialog::getEstimStartingFrequency()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_FREQUENCY_START, DEFAULT_PREF_ESTIM_FREQUENCY_START).toInt();
}

int OptionsDialog::getEstimEndingFrequency()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_FREQUENCY_END, DEFAULT_PREF_ESTIM_FREQUENCY_END).toInt();
}

int OptionsDialog::getEstimTotalSignalGrowth()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_VOL_INCREASE, DEFAULT_PREF_ESTIM_VOL_INCREASE).toInt();
}

int OptionsDialog::getEstimMaxStrokeLength()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_MAX_STROKE_DURATION, DEFAULT_PREF_ESTIM_MAX_STROKE_DURATION).toInt();
}

int OptionsDialog::getEstimBeatFadeInTime()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_BEAT_FADE_IN_TIME, DEFAULT_PREF_ESTIM_BEAT_FADE_IN_TIME).toInt();
}

int OptionsDialog::getEstimBeatFadeInAnticipationTime()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION, DEFAULT_PREF_ESTIM_BEAT_FADE_IN_ANTICIPATION).toInt();
}

int OptionsDialog::getEstimBeatFadeOutTime()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_BEAT_FADE_OUT_TIME, DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_TIME).toInt();
}

int OptionsDialog::getEstimBeatFadeOutDelay()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_BEAT_FADE_OUT_DELAY, DEFAULT_PREF_ESTIM_BEAT_FADE_OUT_DELAY).toInt();
}

int OptionsDialog::getEstimBoostShortStrokes()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_SHORT_STROKES_BOOST, DEFAULT_PREF_ESTIM_SHORT_STROKES_BOOST).toInt();
}

QString OptionsDialog::getEstimStrokeStyle()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_STROKE_STYLE, DEFAULT_PREF_ESTIM_STROKE_STYLE).toString();
}

bool OptionsDialog::getEstimInvertStrokes()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_INVERT_STROKES, DEFAULT_PREF_ESTIM_INVERT_STROKES).toBool();
}

int OptionsDialog::getEstimStartPlaybackFadeInTime()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_START_FADE_IN_TIME, DEFAULT_PREF_ESTIM_START_FADE_IN_TIME).toInt();
}

int OptionsDialog::getEstimSignalPan()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_PANNING, DEFAULT_PREF_ESTIM_PANNING).toInt();
}

double OptionsDialog::getEstimCompressorBiteTime()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_COMPRESSOR_BITE_TIME, DEFAULT_PREF_ESTIM_COMPRESSOR_BITE_TIME).toDouble();
}

int OptionsDialog::getEstimCompressorStrength()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_COMPRESSOR_STRENGTH, DEFAULT_PREF_ESTIM_COMPRESSOR_STRENGTH).toInt();
}

double OptionsDialog::getEstimCompressorRelease()
{
    QSettings settings;
    return settings.value(PREF_ESTIM_COMPRESSOR_RELEASE_TIME, DEFAULT_PREF_ESTIM_COMPRESSOR_RELEASE_TIME).toDouble();
}

void OptionsDialog::on_estimEnabledCheckBox_stateChanged(int checkState)
{
    bool enabled = (checkState == Qt::Checked);
    ui->estimDeviceComboBox->setEnabled(enabled);
    ui->estimStartingFrequencySpinbox->setEnabled(enabled);
    ui->estimEndingFrequencySpinbox->setEnabled(enabled);
    ui->estimTotalSignalGrowthSpinBox->setEnabled(enabled);
    ui->estimMaxStrokeLengthSpinBox->setEnabled(enabled);
    ui->estimBeatFadeInTimeSpinBox->setEnabled(enabled);
    ui->estimBeatFadeInAnticipationTimeSpinBox->setEnabled(enabled);
    ui->estimBeatFadeOutTimeSpinBox->setEnabled(enabled);
    ui->estimBeatFadeOutDelaySpinBox->setEnabled(enabled);
    ui->estimBoostShortStrokesSpinBox->setEnabled(enabled);
    ui->estimUpDownBeatRadioButton->setEnabled(enabled);
    ui->estimDownBeatUpRadioButton->setEnabled(enabled);
    ui->estimInvertStrokesCheckBox->setEnabled(enabled);
    ui->estimStartPlaybackFadeInTimeSpinBox->setEnabled(enabled);
    ui->estimSignalPanHorizontalSlider->setEnabled(enabled);
    ui->estimCompressorBiteTimeSpinBox->setEnabled(enabled);
    ui->estimCompressorStrengthSpinBox->setEnabled(enabled);
    ui->estimCompressorReleaseSpinBox->setEnabled(enabled);
}

void OptionsDialog::on_estimSignalPanHorizontalSlider_valueChanged(int value)
{
    if (value == 0)
        ui->estimSignalPanDescriptionLabel->setText(tr("Centred"));
    else if (value < 0)
        ui->estimSignalPanDescriptionLabel->setText(QString(tr("%1% Left")).arg(-value));
    else
        ui->estimSignalPanDescriptionLabel->setText(QString(tr("%1% Right")).arg(value));
}

void OptionsDialog::on_estimSignalPanHorizontalSlider_sliderMoved(int position)
{
    on_estimSignalPanHorizontalSlider_valueChanged(position);
}

void OptionsDialog::on_estimUpDownBeatRadioButton_toggled(bool checked)
{
    if (checked && !settingPrefs)
    {
        int maxBudge = std::min(ui->estimMaxStrokeLengthSpinBox->value() / 2, ui->estimBeatFadeOutDelaySpinBox->value());
        ui->estimBeatFadeOutDelaySpinBox->setValue(ui->estimBeatFadeOutDelaySpinBox->value() - maxBudge);
        ui->estimBeatFadeInAnticipationTimeSpinBox->setValue(ui->estimBeatFadeInAnticipationTimeSpinBox->value() + maxBudge);
    }
}

void OptionsDialog::on_estimDownBeatUpRadioButton_toggled(bool checked)
{
    if (checked && !settingPrefs)
    {
        int maxBudge = std::min(ui->estimMaxStrokeLengthSpinBox->value() / 2, ui->estimBeatFadeInAnticipationTimeSpinBox->value());
        ui->estimBeatFadeInAnticipationTimeSpinBox->setValue(ui->estimBeatFadeInAnticipationTimeSpinBox->value() - maxBudge);
        ui->estimBeatFadeOutDelaySpinBox->setValue(ui->estimBeatFadeOutDelaySpinBox->value() + maxBudge);
    }
}

void OptionsDialog::on_obscureBottomScreenPortionCheckBox_toggled(bool checked)
{
    ui->obscureBottomScreenPortionWidthSlider->setEnabled(checked);
    ui->obscureBottomScreenPortionWidthSpinBox->setEnabled(checked);
}

void OptionsDialog::on_useDefaultVideoAudioDeviceCheckBox_toggled(bool checked)
{
    bool enabled = !checked;
    ui->alternativeVideoAudioDeviceLabel->setEnabled(enabled);
    ui->alternativeVideoAudioDeviceComboBox->setEnabled(enabled);
}

void OptionsDialog::on_meterChooseNowMarkerColourButton_clicked()
{
    nowMarkerColor = QColorDialog::getColor();
    setNowMarkerColorFrame(nowMarkerColor);
}

void OptionsDialog::setNowMarkerColorFrame(QColor & color)
{
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString rgbRep = QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
    ui->nowMarkerColourFrame->setStyleSheet(rgbRep);
}
