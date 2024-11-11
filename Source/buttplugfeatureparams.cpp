#include "buttplugfeatureparams.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QDebug>
#include "vibratorpulsefeatureparams.h"
#include "buttplugdeviceconfigdialog.h"
#include "buttplug/buttplugdevice.h"

ButtplugFeatureParams::ButtplugFeatureParams(QObject *parent, ButtplugDeviceFeatureType featureType, ButtplugDeviceParameter::Type type, bool enabled, short syncAdjust)
    :
    QObject(parent),
    featureType(featureType),
    paramsType(type),
    enabled(enabled),
    syncAdjust(syncAdjust)
{

}

ButtplugFeatureParams::ButtplugFeatureParams(QObject *parent, ButtplugDeviceFeature *feature, ButtplugDeviceParameter::Type type, bool enabled, short syncAdjust)
    :
    QObject(parent),
    featureType(feature->type),
    paramsType(type),
    enabled(enabled),
    deviceName(feature->device->name),
    featureName(feature->getName()),
    syncAdjust(syncAdjust)
{

}

bool ButtplugFeatureParams::isDefaultParams()
{
    return deviceName.isEmpty();
}

void ButtplugFeatureParams::writeSettingsGroup(QSettings &settings)
{
    //this parent method, intended to be called by its children, should not
    //create its own settings group, but just add them to whatever group the child created
    settings.setValue(PARAMS_TYPE_SETTING, paramsType);
    settings.setValue(PARAMS_ENABLED_SETTING, enabled);
    settings.setValue(DEVICE_NAME_SETTING, deviceName);
    settings.setValue(FEATURE_NAME_SETTING, featureName);
    settings.setValue(SYNC_ADJUST_SETTING, syncAdjust);
}

ButtplugFeatureParams *ButtplugFeatureParams::fromSettingsGroup(QSettings &settings)
{
    if (!settings.contains(PARAMS_TYPE_SETTING))
        return nullptr;

    ButtplugDeviceParameter::Type type = (ButtplugDeviceParameter::Type) settings.value(PARAMS_TYPE_SETTING).toInt();
    qDebug() << "reading in params of type " << type;

    switch (type)
    {
    case ButtplugDeviceParameter::VibratorPulse:
        return VibratorPulseFeatureParams::fromSettingsGroup(settings);
    default:
        return nullptr;
    }
}

void ButtplugFeatureParams::readSettingsGroup(QSettings &settings)
{
    paramsType = (ButtplugDeviceParameter::Type) settings.value(PARAMS_TYPE_SETTING).toInt();
    enabled = settings.value(PARAMS_ENABLED_SETTING).toBool();
    deviceName = settings.value(DEVICE_NAME_SETTING).toString();
    featureName = settings.value(FEATURE_NAME_SETTING).toString();
    syncAdjust = settings.value(SYNC_ADJUST_SETTING).toInt();
}

void ButtplugFeatureParams::createUi(QWidget * parentWidget, QBoxLayout *)
{
    //used but not stored:
    QVBoxLayout *eventTypeAndSyncLayout;
    QSpacerItem *verticalSpacerTop;
    QSpacerItem *verticalSpacerMiddle;
    QSpacerItem *verticalSpacerBottom;
    QLabel *eventTypeLabel;
    QLabel *syncAdjustLabel;

    layout = new QHBoxLayout(parentWidget);
    layout->setObjectName(QString::fromUtf8("layout"));
    enableCheckBox = new QCheckBox(parentWidget);
    enableCheckBox->setObjectName(QString::fromUtf8("enableCheckBox"));
    enableCheckBox->setChecked(true);

    layout->addWidget(enableCheckBox);

    eventTypeAndSyncLayout = new QVBoxLayout();
    eventTypeAndSyncLayout->setObjectName(QString::fromUtf8("eventTypeAndSyncLayout"));
    verticalSpacerTop = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    eventTypeAndSyncLayout->addItem(verticalSpacerTop);

    eventTypeLabel = new QLabel(parentWidget);
    eventTypeLabel->setObjectName(QString::fromUtf8("eventTypeLabel"));

    eventTypeAndSyncLayout->addWidget(eventTypeLabel);

    eventTypeComboBox = new QComboBox(parentWidget);
    eventTypeComboBox->setObjectName(QString::fromUtf8("eventTypeComboBox"));

    eventTypeAndSyncLayout->addWidget(eventTypeComboBox);

    verticalSpacerMiddle = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    eventTypeAndSyncLayout->addItem(verticalSpacerMiddle);

    syncAdjustLabel = new QLabel(parentWidget);
    syncAdjustLabel->setObjectName(QString::fromUtf8("syncAdjustLabel"));

    eventTypeAndSyncLayout->addWidget(syncAdjustLabel);

    syncAdjustSpinBox = new QSpinBox(parentWidget);
    syncAdjustSpinBox->setObjectName(QString::fromUtf8("syncAdjustSpinBox"));
    syncAdjustSpinBox->setMinimum(-1500);
    syncAdjustSpinBox->setMaximum(1500);

    eventTypeAndSyncLayout->addWidget(syncAdjustSpinBox);

    verticalSpacerBottom = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    eventTypeAndSyncLayout->addItem(verticalSpacerBottom);


    layout->addLayout(eventTypeAndSyncLayout);

    enableCheckBox->setText(featureName);
    eventTypeLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Event Type", nullptr));
    syncAdjustLabel->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Sync adjust", nullptr));
    syncAdjustSpinBox->setSuffix(QApplication::translate("ButtplugDeviceConfigDialog", "ms", nullptr));
}

void ButtplugFeatureParams::addDeleteButton(QWidget * parent, QHBoxLayout *layout)
{
    deleteButton = new QToolButton(parent);
    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
    deleteButton->setText(QApplication::translate("ButtplugDeviceConfigDialog", "Delete", nullptr));

    layout->addWidget(deleteButton);
}

void ButtplugFeatureParams::adoptUi(QHBoxLayout *layout, QCheckBox *enableCheckBox, QComboBox *eventTypeComboBox, QSpinBox *syncAdjustSpinBox)
{
    this->layout = layout;
    this->enableCheckBox = enableCheckBox;
    this->eventTypeComboBox = eventTypeComboBox;
    this->syncAdjustSpinBox = syncAdjustSpinBox;
}

void ButtplugFeatureParams::adoptUiValues()
{
    enabled = enableCheckBox->isChecked();
    syncAdjust = syncAdjustSpinBox->value();
    //this method only used for defaults, so this possibly not needed...
    //    eventType = eventTypeComboBox->currentText().toInt();
}

void ButtplugFeatureParams::connectWidgetSignals()
{
    connect(enableCheckBox, SIGNAL(toggled(bool)), this, SLOT(newEnabledState(bool)));
    connect(syncAdjustSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_SyncAdjustChanged(int)));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteRequested()));
}

void ButtplugFeatureParams::updateUiFromData()
{
    enableCheckBox->setChecked(enabled);
    syncAdjustSpinBox->setValue(syncAdjust);
    eventTypeComboBox->setCurrentText(QString::number(eventType));
}

void ButtplugFeatureParams::copyParamsFrom(ButtplugFeatureParams *other)
{
    enabled = other->enabled;
    eventType = other->eventType;
    syncAdjust = other->syncAdjust;
}

void ButtplugFeatureParams::newEnabledState(bool enabled)
{
    setUiEnabled(enabled);
}

void ButtplugFeatureParams::newEventType(int )
{

}

void ButtplugFeatureParams::deleteRequested()
{
    emit aboutToDelete(this);
    for (int i = ButtplugDeviceConfigDialog::entries.size() - 1; i >= 0; --i)
    {
        if (ButtplugDeviceConfigDialog::entries[i] == this)
            ButtplugDeviceConfigDialog::entries.remove(i);
    }
}

void ButtplugFeatureParams::on_SyncAdjustChanged(int newValue)
{
    syncAdjust = newValue;
}
