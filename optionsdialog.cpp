#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    triggeringWidget(NULL)
{
    ui->setupUi(this);
    setControlsFromPreferences();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setControlsFromPreferences()
{
    QSettings settings;
    ui->startingIntensitySlider->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->startingIntensitySpinBox->setValue(settings.value("Starting Min Intensity", MainWindow::startingMinIntensity).toInt());
    ui->endingIntensitySlider->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());
    ui->endingIntensitySpinBox->setValue(settings.value("Ending Min Intensity", MainWindow::endingMinIntensity).toInt());

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

    ui->beatMeterWidthSlider->setValue(settings.value("Beat Meter Width").toDouble() * 100);
    ui->beatMeterWidthSpinBox->setValue(settings.value("Beat Meter Width").toDouble());
}

void OptionsDialog::setPreferencesFromControls()
{
    QSettings settings;
    settings.setValue("Starting Min Intensity", ui->startingIntensitySpinBox->value());
    settings.setValue("Ending Min Intensity", ui->endingIntensitySlider->value());
    settings.setValue("Beat Meter Width", ui->beatMeterWidthSpinBox->value());
    settings.setValue("Connect To Handy", ui->connectToHandyCheckBox->isChecked());
    settings.setValue("Handy Connection Key", ui->handyKeyEdit->text());
    settings.setValue("Handy Sync Offset", ui->syncOffsetSpinBox->value());
    settings.setValue("Handy Range Top", ui->topOfRangeSlider->value());
    settings.setValue("Handy Range Bottom", ui->bottomOfRangeSlider->value());
    settings.setValue("Handy Range Anchor", ui->rangeAnchorSlider->value());
    settings.setValue("Handy Full Stroke Duration", ui->handyStrokeDurationSpinBox->value());
    settings.setValue("Handy Scaling Proportion", ui->handyScalingProportionSlider->value());
    settings.setValue("Full Strokes", ui->fullStrokeRadioButton->isChecked());
    settings.setValue("Beat Meter Width", ui->beatMeterWidthSpinBox->value());
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

void OptionsDialog::on_beatMeterWidthSlider_valueChanged(int value)
{
    ui->beatMeterWidthSpinBox->setValue(value / 100.0);
}

void OptionsDialog::on_beatMeterWidthSpinBox_valueChanged(double value)
{
    ui->beatMeterWidthSlider->setValue(value * 100);
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
