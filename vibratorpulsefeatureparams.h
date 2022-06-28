#ifndef VIBRATORPULSEDEVICEPARAMS_H
#define VIBRATORPULSEDEVICEPARAMS_H

#include "buttplugfeatureparams.h"
#include "event.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QSlider;
class QSpinBox;
class QVBoxLayout;
class QSpacerItem;

#define PULSE_INTENSITIES_SETTING "Pulse intensities"
#define PULSE_LENGTHS_SETTING "Pulse lengths"

class VibratorPulseFeatureParams : public ButtplugFeatureParams
{
    Q_OBJECT
public:
    VibratorPulseFeatureParams(QObject *parent = nullptr);
    VibratorPulseFeatureParams(QObject *parent, ButtplugDeviceFeature * feature);

public:
    virtual void createUi(QWidget *parentWidget, QBoxLayout *parentLayout) override;
    void adoptUi(QHBoxLayout *layout,
                    QCheckBox *enableCheckBox,
                    QComboBox *eventTypeComboBox, QSpinBox *syncAdjustSpinBox,
                    QSlider *startIntensityHighSlider,
                    QDoubleSpinBox *startIntensityHighSpinBox,
                    QDoubleSpinBox *startIntensityLowSpinBox,
                    QSlider *startIntensityLowSlider,
                    QSlider *startDurationShortSlider,
                    QSpinBox *startDurationShortSpinBox,
                    QSpinBox *startDurationLongSpinBox,
                    QSlider *startDurationLongSlider,
                    QSlider *endIntensityHighSlider,
                    QDoubleSpinBox *endIntensityHighSpinBox,
                    QDoubleSpinBox *endIntensityLowSpinBox,
                    QSlider *endIntensityLowSlider,
                    QSlider *endDurationShortSlider,
                    QSpinBox *endDurationShortSpinBox,
                    QSpinBox *endDurationLongSpinBox,
                    QSlider *endDurationLongSlider,
                    QToolButton *deleteButton );
    void adoptUiValues() override;
    void connectWidgetSignals() override;
    void updateUiFromData() override;
    void copyParamsFrom(ButtplugFeatureParams *other) override;

    void setStartingIntensityLow(double value);
    double getStartingIntensityLow();
    void setStartingIntensityHigh(double value);
    double getStartingIntensityHigh();
    void setStartingDurationShort(int value);
    int getStartingDurationShort();
    void setStartingDurationLong(int value);
    int getStartingDurationLong();
    void setEndingIntensityLow(double value);
    double getEndingIntensityLow();
    void setEndingIntensityHigh(double value);
    double getEndingIntensityHigh();
    void setEndingDurationShort(int value);
    int getEndingDurationShort();
    void setEndingDurationLong(int value);
    int getEndingDurationLong();
    double getRequestIntensity(Event event);
    int getPulseLength(Event event);

    //data 'layer'
    double startingIntensityLow;
    double startingIntensityHigh;
    int startingDurationShort;
    int startingDurationLong;
    double endingIntensityLow;
    double endingIntensityHigh;
    int endingDurationShort;
    int endingDurationLong;

    //this should really belong to a separate class like ButtplugFeatureBehaviour
    virtual void handleEvent(Event event, ButtplugDeviceFeature * feature) override;
    static QHash<ButtplugDeviceFeature *, QTimer *> endTimers;
    QTimer * getEndTimerFor(ButtplugDeviceFeature * feature);

    static VibratorPulseFeatureParams *fromSettingsGroup(QSettings & settings);
    virtual void writeSettingsGroup(QSettings & settings) override;
    QVariantList getPulseIntensitySettings();
    void setPulseIntensitySettings(QVariantList intensities);
    QVariantList getPulseLengthSettings();
    void setPulseLengthSettings(QVariantList lengths);
    void readSettingsGroup(QSettings &settings) override;

protected:
    void setUiEnabled(bool enabled) override;

private slots:
    void on_StartingIntensityLowChanged(double newValue);
    void on_StartingIntensityLowChanged(int newValue);

    void on_StartingIntensityHighChanged(double newValue);
    void on_StartingIntensityHighChanged(int newValue);

    void on_StartingDurationShortChanged(int newValue);
    void on_StartingDurationLongChanged(int newValue);

    void on_EndingIntensityLowChanged(double newValue);
    void on_EndingIntensityLowChanged(int newValue);

    void on_EndingIntensityHighChanged(double newValue);
    void on_EndingIntensityHighChanged(int newValue);

    void on_EndingDurationShortChanged(int newValue);

    void on_EndingDurationLongChanged(int newValue);

private:
    QComboBox *eventTypeComboBox;
    QSlider *startIntensityHighSlider;
    QDoubleSpinBox *startIntensityHighSpinBox;
    QDoubleSpinBox *startIntensityLowSpinBox;
    QSlider *startIntensityLowSlider;
    QGridLayout *startDurationGridLayout;
    QSlider *startDurationShortSlider;
    QSpinBox *startDurationShortSpinBox;
    QSpinBox *startDurationLongSpinBox;
    QSlider *startDurationLongSlider;
    QSlider *endIntensityHighSlider;
    QDoubleSpinBox *endIntensityHighSpinBox;
    QDoubleSpinBox *endIntensityLowSpinBox;
    QSlider *endIntensityLowSlider;
    QSlider *endDurationShortSlider;
    QSpinBox *endDurationShortSpinBox;
    QSpinBox *endDurationLongSpinBox;
    QSlider *endDurationLongSlider;

    //this should really belong to a separate class like ButtplugFeatureBehaviour
    void startPulseNow(Event event, ButtplugDeviceFeature * feature);
    void stopPulseNow(ButtplugDeviceFeature * feature);
};

#endif // VIBRATORPULSEDEVICEPARAMS_H
