#ifndef BUTTPLUGDEVICEPARAMS_H
#define BUTTPLUGDEVICEPARAMS_H

#include <QObject>
#include <QSettings>
#include "buttplug/buttplugdevicefeature.h"

#define PARAMS_TYPE_SETTING "Params type"
#define PARAMS_ENABLED_SETTING "Parameters enabled"
#define DEVICE_NAME_SETTING "Device name"
#define FEATURE_NAME_SETTING "Feature name"
#define SYNC_ADJUST_SETTING "Synchronisation adjustment"

class QHBoxLayout;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QBoxLayout;
class QToolButton;

namespace ButtplugDeviceParameter {
    enum Type {
        VibratorPulse,
        StrokerRange,
        RotatorBurst
    };
}

class ButtplugFeatureParams : public QObject
{
    Q_OBJECT
public:
    explicit ButtplugFeatureParams(QObject *parent, ButtplugDeviceFeatureType featureType, ButtplugDeviceParameter::Type type, bool enabled = true, short syncAdjust = 0);
    explicit ButtplugFeatureParams(QObject *parent, ButtplugDeviceFeature * feature, ButtplugDeviceParameter::Type type, bool enabled = true, short syncAdjust = 0);
    ButtplugDeviceFeatureType featureType;
    ButtplugDeviceParameter::Type paramsType;
    bool enabled;
    QString deviceName;
    QString featureName;
    int eventType;
    short syncAdjust = 0;
    //!
    //! \brief isDefaultParams are these parameters intended for new or uncustomised devices/features?
    //! \return true for yes
    //!
    bool isDefaultParams();

    virtual void writeSettingsGroup(QSettings & settings);
    //!
    //! \brief fromSettingsGroup creates an instance of a ButtplugFeatureParams from a QSettings group
    //! \param settings the QSettings. It is expected that 'beginGroup' has been called on this QSettings so that
    //!                 the values of the parameters can be accessed directly using 'value()'.
    //! \return
    //!
    static ButtplugFeatureParams * fromSettingsGroup(QSettings & settings);
    virtual void readSettingsGroup(QSettings & settings);

    //this should really belong to a separate class like ButtplugFeatureBehaviour
    virtual void handleEvent(Event event, ButtplugDeviceFeature * feature) = 0;

signals:
    void aboutToDelete(ButtplugFeatureParams *);

protected:
    virtual void setUiEnabled(bool enabled) = 0;
    QHBoxLayout * layout;
    QCheckBox *enableCheckBox;
    QComboBox *eventTypeComboBox;
    QSpinBox *syncAdjustSpinBox;

    QToolButton *deleteButton;
public:
    virtual void createUi(QWidget *parentWidget, QBoxLayout *parentLayout);
    void addDeleteButton(QWidget *parent, QHBoxLayout * layout);
    void adoptUi(QHBoxLayout *layout, QCheckBox *enableCheckBox, QComboBox *eventTypeComboBox, QSpinBox *syncAdjustSpinBox);
    virtual void adoptUiValues();
    virtual void connectWidgetSignals();
    virtual void updateUiFromData();
    virtual void copyParamsFrom(ButtplugFeatureParams * other);

public slots:
    void newEnabledState(bool enabled);
    void newEventType(int eventType);
    void deleteRequested();

private slots:
    void on_SyncAdjustChanged(int newValue);
};

#endif // BUTTPLUGDEVICEPARAMS_H
