#include "buttplugdeviceconfigdialog.h"
#include "ui_buttplugdeviceconfigdialog.h"
#include "vibratorpulsefeatureparams.h"
#include "globals.h"
#include "mainwindow.h"
#include <QSettings>

QVector<ButtplugFeatureParams *> ButtplugDeviceConfigDialog::entries;

ButtplugDeviceConfigDialog::ButtplugDeviceConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ButtplugDeviceConfigDialog)
{
    ui->setupUi(this);

    VibratorPulseFeatureParams * defaultVibratorParams = getDefaultVibratorParams();
    bool defaultVibratorParamsAlreadyPresent = defaultVibratorParams != nullptr;

    if (!defaultVibratorParamsAlreadyPresent)
    {
        defaultVibratorParams = new VibratorPulseFeatureParams(mainWindow->getButtplugInterface());
        entries.append(defaultVibratorParams);
    }

    defaultVibratorParams->adoptUi(ui->defaultVibeLayout,
                                   ui->defaultVibeEnableCheckBox,
                                   ui->defaultVibeEventTypeComboBox,
                                   ui->defaultVibeSyncAdjustSpinBox,
                                   ui->defaultVibeStartIntensityHighSlider,
                                   ui->defaultVibeStartIntensityHighSpinBox,
                                   ui->defaultVibeStartIntensityLowSpinBox,
                                   ui->defaultVibeStartIntensityLowSlider,
                                   ui->defaultVibeStartDurationShortSlider,
                                   ui->defaultVibeStartDurationShortSpinBox,
                                   ui->defaultVibeStartDurationLongSpinBox,
                                   ui->defaultVibeStartDurationLongSlider,
                                   ui->defaultVibeEndIntensityHighSlider,
                                   ui->defaultVibeEndIntensityHighSpinBox,
                                   ui->defaultVibeEndIntensityLowSpinBox,
                                   ui->defaultVibeEndIntensityLowSlider,
                                   ui->defaultVibeEndDurationShortSlider,
                                   ui->defaultVibeEndDurationShortSpinBox,
                                   ui->defaultVibeEndDurationLongSpinBox,
                                   ui->defaultVibeEndDurationLongSlider,
                                   ui->defaultVibeDeleteButton );
    if (!defaultVibratorParamsAlreadyPresent)
        defaultVibratorParams->adoptUiValues();
    defaultVibratorParams->connectWidgetSignals();
    if (defaultVibratorParamsAlreadyPresent)
        defaultVibratorParams->updateUiFromData();

    for (auto params : entries) {
        if (!params->isDefaultParams())
        {
            QFrame * panel = new QFrame(ui->scrollAreaWidgetContents);
            panel->setFrameShape(QFrame::WinPanel);
            panel->setFrameShadow(QFrame::Raised);
            params->createUi(panel, ui->scrollAreaLayout);
            params->connectWidgetSignals();
            params->updateUiFromData();
            nonDefaultPanels.insert(params, panel);
            ui->scrollAreaLayout->addWidget(panel);
            connect(params, SIGNAL(aboutToDelete(ButtplugFeatureParams *)), this, SLOT(deleteParams(ButtplugFeatureParams *)));
        }
    }

    refreshFeatureList();
    connect(mainWindow->getButtplugInterface(), SIGNAL(deviceAdded(int)), this, SLOT(refreshFeatureList()));
    connect(mainWindow->getButtplugInterface(), SIGNAL(deviceRemoved(int)), this, SLOT(refreshFeatureList()));
}

ButtplugDeviceConfigDialog::~ButtplugDeviceConfigDialog()
{
    delete ui;
}

#define BUTTPLUG_CONFIGS_SETTINGS_GROUP "Buttplug Device Settings"

void ButtplugDeviceConfigDialog::readInConfigs()
{
    QSettings settings;
    settings.beginGroup(BUTTPLUG_CONFIGS_SETTINGS_GROUP);
    entries.clear();
    for (QString childGroup : settings.childGroups())
    {
        settings.beginGroup(childGroup);
        entries.append(ButtplugFeatureParams::fromSettingsGroup(settings));
        settings.endGroup();
    }
    settings.endGroup();
}

void ButtplugDeviceConfigDialog::writeOutConfigs()
{
    QSettings settings;
    settings.beginGroup(BUTTPLUG_CONFIGS_SETTINGS_GROUP);
    //clear the group
    settings.remove("");
    for (QString childGroup : settings.childGroups())
    {
        settings.beginGroup(childGroup);
        settings.remove("");
        settings.endGroup();
    }
    for (ButtplugFeatureParams * params : entries)
    {
        params->writeSettingsGroup(settings);
    }
    settings.endGroup();
}

void ButtplugDeviceConfigDialog::refreshFeatureList()
{
    ui->deviceSelectComboBox->clear();
    for (auto feature : mainWindow->getButtplugInterface()->getAllFeatures())
    {
        ui->deviceSelectComboBox->addItem(feature->getName());
    }
    ui->addButton->setEnabled(ui->deviceSelectComboBox->count() != 0);
    ui->deviceSelectComboBox->setEnabled(ui->deviceSelectComboBox->count() != 0);
}

void ButtplugDeviceConfigDialog::saveAndClose()
{
    writeOutConfigs();
    accept();
}

void ButtplugDeviceConfigDialog::deleteParams(ButtplugFeatureParams * params)
{
    QWidget * panel = nonDefaultPanels.value(params);
    if (panel != nullptr)
    {
        ui->scrollAreaLayout->removeWidget(panel);
        delete panel;
    }
}

VibratorPulseFeatureParams *ButtplugDeviceConfigDialog::getDefaultVibratorParams()
{
    for (ButtplugFeatureParams * entry : entries)
        if (entry->isDefaultParams() && entry->featureType == VibratorMotor)
            return dynamic_cast<VibratorPulseFeatureParams *>(entry);
    return nullptr;
}

void ButtplugDeviceConfigDialog::on_addButton_clicked()
{
    ButtplugDeviceFeature * currentFeature = mainWindow->getButtplugInterface()->getFeatureByName(ui->deviceSelectComboBox->currentText());
    if (currentFeature != nullptr)
    {
        switch (currentFeature->type)
        {
        case VibratorMotor:
            createNewVibratorParams(currentFeature);
            break;
        default:
            //do nothing
            break;
        }
    }
}

void ButtplugDeviceConfigDialog::createNewVibratorParams(ButtplugDeviceFeature *feature)
{
    QFrame * panel = new QFrame(ui->scrollAreaWidgetContents);
    panel->setFrameShape(QFrame::WinPanel);
    panel->setFrameShadow(QFrame::Raised);
    VibratorPulseFeatureParams * newVibratorParams = new VibratorPulseFeatureParams(mainWindow->getButtplugInterface(), feature);
    newVibratorParams->copyParamsFrom(getDefaultVibratorParams());
    newVibratorParams->createUi(panel, ui->scrollAreaLayout);
    newVibratorParams->connectWidgetSignals();
    newVibratorParams->updateUiFromData();
    entries.append(newVibratorParams);
    nonDefaultPanels.insert(newVibratorParams, panel);
    ui->scrollAreaLayout->addWidget(panel);
    connect(newVibratorParams, SIGNAL(aboutToDelete(ButtplugFeatureParams *)), this, SLOT(deleteParams(ButtplugFeatureParams *)));
}
