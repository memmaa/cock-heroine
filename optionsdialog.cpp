#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"
#include <QAudioDeviceInfo>

#define DEFAULT_BEAT_METER_HEIGHT 100
#define DEFAULT_BEAT_METER_WIDTH 1920
#define DEFAULT_BEAT_METER_SPEED 167
#define DEFAULT_BEAT_METER_FRAME_RATE 29.97
#define DEFAULT_BEAT_MARKER_DIAMETER 20

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

void OptionsDialog::populateUi()
{
    const auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &deviceInfo : deviceInfos)
        ui->estimDeviceComboBox->addItem(deviceInfo.deviceName());
}

void OptionsDialog::setControlsFromPreferences()
{
    QSettings settings;
    ui->startingIntensitySlider->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->startingIntensitySpinBox->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->endingIntensitySlider->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());
    ui->endingIntensitySpinBox->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());

    ui->obscureBottomScreenPortionWidthSlider->setValue(settings.value("Obscure Bottom Screen Portion").toDouble() * 100);
    ui->obscureBottomScreenPortionWidthSpinBox->setValue(settings.value("Obscure Bottom Screen Portion").toDouble());

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

    ui->meterHeightSpinBox->setValue(settings.value("Beat Meter Height", DEFAULT_BEAT_METER_HEIGHT).toInt());
    ui->meterWidthSpinBox->setValue(settings.value("Beat Meter Width", DEFAULT_BEAT_METER_WIDTH).toInt());
    ui->meterSpeedSpinBox->setValue(settings.value("Beat Meter Speed", DEFAULT_BEAT_METER_SPEED).toInt());
    ui->meterFrameRateSpinBox->setValue(settings.value("Beat Meter Frame Rate", DEFAULT_BEAT_METER_FRAME_RATE).toDouble());
    ui->meterMarkerDiameterSpinBox->setValue(settings.value("Beat Marker Diameter", DEFAULT_BEAT_MARKER_DIAMETER).toInt());

    ui->estimEnabledCheckBox->setChecked(settings.value("Emit E-Stim Signal",false).toBool());
    ui->estimDeviceComboBox->setCurrentText(settings.value("E-Stim Output Device", "").toString());
}

void OptionsDialog::setPreferencesFromControls()
{
    QSettings settings;
    settings.setValue("Starting Min Intensity", ui->startingIntensitySpinBox->value());
    settings.setValue("Ending Min Intensity", ui->endingIntensitySlider->value());
    settings.setValue("Obscure Bottom Screen Portion", ui->obscureBottomScreenPortionWidthSlider->value());
    settings.setValue("Connect To Handy", ui->connectToHandyCheckBox->isChecked());
    settings.setValue("Handy Connection Key", ui->handyKeyEdit->text());
    settings.setValue("Handy Sync Offset", ui->syncOffsetSpinBox->value());
    settings.setValue("Handy Range Top", ui->topOfRangeSlider->value());
    settings.setValue("Handy Range Bottom", ui->bottomOfRangeSlider->value());
    settings.setValue("Handy Range Anchor", ui->rangeAnchorSlider->value());
    settings.setValue("Handy Full Stroke Duration", ui->handyStrokeDurationSpinBox->value());
    settings.setValue("Handy Scaling Proportion", ui->handyScalingProportionSlider->value());
    settings.setValue("Full Strokes", ui->fullStrokeRadioButton->isChecked());

    settings.setValue("Beat Meter Height", ui->meterHeightSpinBox->value());
    settings.setValue("Beat Meter Width", ui->meterWidthSpinBox->value());
    settings.setValue("Beat Meter Speed", ui->meterSpeedSpinBox->value());
    settings.setValue("Beat Meter Frame Rate", ui->meterFrameRateSpinBox->value());
    settings.setValue("Beat Marker Diameter", ui->meterMarkerDiameterSpinBox->value());

    settings.setValue("Emit E-Stim Signal", ui->estimEnabledCheckBox->isChecked());
    settings.setValue("E-Stim Output Device", ui->estimDeviceComboBox->currentText());
}

bool OptionsDialog::emitEstimSignal()
{
    QSettings settings;
    return settings.value("Emit E-Stim Signal", false).toBool();
}

QString OptionsDialog::getEstimOutputDevice()
{
    QSettings settings;
    return settings.value("E-Stim Output Device", "").toString();
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
    return settings.value("Beat Meter Height", DEFAULT_BEAT_METER_HEIGHT).toInt();
}

int OptionsDialog::getBeatMeterWidth()
{
    QSettings settings;
    return settings.value("Beat Meter Width", DEFAULT_BEAT_METER_WIDTH).toInt();
}

int OptionsDialog::getBeatMeterSpeed()
{
    QSettings settings;
    return settings.value("Beat Meter Speed", DEFAULT_BEAT_METER_SPEED).toInt();
}

double OptionsDialog::getBeatMeterFrameRate()
{
    QSettings settings;
    return settings.value("Beat Meter Frame Rate", DEFAULT_BEAT_METER_FRAME_RATE).toDouble();
}

int OptionsDialog::getBeatMarkerDiameter()
{
    QSettings settings;
    return settings.value("Beat Marker Diameter", DEFAULT_BEAT_MARKER_DIAMETER).toInt();
}
