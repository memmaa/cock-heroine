#include "optimisationoptionsdialog.h"
#include "ui_optimisationoptionsdialog.h"
#include "beatoptimisation.h"
#include "globals.h"
#include "beattimestamp.h"
#include "beatanalysis.h"

OptimisationOptionsDialog::OptimisationOptionsDialog(EditorWindow *parent) :
    QDialog(parent),
    ui(new Ui::OptimisationOptionsDialog)
{
    ui->setupUi(this);

    settings.beginGroup("optimisationOptions");

    /*ui->useProvidedOutputTempoRadioButton->setChecked(
                settings.value("outputTempoProvided",BeatOptimisation::Configuration::outputTempoProvided).toBool());*/
    ui->roundBPMCheckBox->setChecked(
                settings.value("roundToNearestBPM",BeatOptimisation::Configuration::roundToNearestBPM).toBool());
    ui->automaticOptimisationRadioButton->setChecked( ! ui->useProvidedOutputTempoRadioButton->isChecked());
    ui->manualBPMSpinBox->setValue(
                //settings.value("providedOutputTempo",BeatOptimisation::Configuration::providedOutputTempo).toDouble());
                BeatAnalysis::Configuration::currentBPM);
    ui->manualStartTimestampCheckBox->setChecked(
                settings.value("startingTimestampProvided",BeatOptimisation::Configuration::startingTimestampProvided).toBool());
    ui->manualStartTimestampSpinBox->setValue(
                //settings.value("providedStartingTimestamp",(qlonglong)BeatOptimisation::Configuration::providedStartingTimestamp).toLongLong());
                beatTimestamps[0].eventData.timestamp);

    settings.endGroup();

    if (ui->useProvidedOutputTempoRadioButton->isChecked())
        on_useProvidedOutputTempoRadioButton_clicked();
    else
        on_automaticOptimisationRadioButton_clicked();
}

OptimisationOptionsDialog::~OptimisationOptionsDialog()
{
    delete ui;
}

void OptimisationOptionsDialog::on_automaticOptimisationRadioButton_clicked()
{
    ui->roundBPMCheckBox->setEnabled(true);
    ui->roundBPMEvenCheckBox->setEnabled(ui->roundBPMCheckBox->isChecked());
    ui->manualBPMSpinBox->setEnabled(false);
    ui->manualStartTimestampCheckBox->setChecked(false);
    ui->manualStartTimestampCheckBox->setEnabled(false);
    ui->manualStartTimestampSpinBox->setEnabled(false);
}

void OptimisationOptionsDialog::on_useProvidedOutputTempoRadioButton_clicked()
{
    ui->manualBPMSpinBox->setEnabled(true);
    ui->manualStartTimestampCheckBox->setEnabled(true);
    ui->roundBPMCheckBox->setEnabled(false);
    ui->roundBPMEvenCheckBox->setEnabled(false);
}

void OptimisationOptionsDialog::on_manualStartTimestampCheckBox_toggled(bool checked)
{
    ui->manualStartTimestampSpinBox->setEnabled(checked);
}

void OptimisationOptionsDialog::on_buttons_accepted()
{
    settings.beginGroup("optimisationOptions");

    settings.setValue("outputTempoProvided",BeatOptimisation::Configuration::outputTempoProvided = ui->useProvidedOutputTempoRadioButton->isChecked());
    settings.setValue("startingTimestampProvided",BeatOptimisation::Configuration::startingTimestampProvided = ui->manualStartTimestampCheckBox->isChecked());

    if (ui->automaticOptimisationRadioButton->isChecked())
    {
        settings.setValue("roundToNearestBPM",BeatOptimisation::Configuration::roundToNearestBPM = ui->roundBPMCheckBox->isChecked());
        BeatOptimisation::Configuration::roundToEvenBPM = ui->roundBPMEvenCheckBox->isChecked();
    }
    else
    {
        BeatOptimisation::Configuration::roundToNearestBPM = false;
        BeatOptimisation::Configuration::roundToEvenBPM = false;
        settings.setValue("providedOutputTempo",BeatOptimisation::Configuration::providedOutputTempo = ui->manualBPMSpinBox->value());
        if (ui->manualStartTimestampCheckBox->isChecked())
            settings.setValue("providedStartingTimestamp",(qlonglong)(BeatOptimisation::Configuration::providedStartingTimestamp = ui->manualStartTimestampSpinBox->value()));
    }

    settings.endGroup();
}

void OptimisationOptionsDialog::on_roundBPMCheckBox_clicked(bool checked)
{
    ui->roundBPMEvenCheckBox->setEnabled(checked);
    if (!checked)
        ui->roundBPMEvenCheckBox->setChecked(checked);
}
