#include "analysisoptionsdialog.h"
#include "ui_analysisoptionsdialog.h"
#include "beatanalysis.h"
#include <QPushButton>
#include <QTime>
#include <QTimer>

AnalysisOptionsDialog::AnalysisOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalysisOptionsDialog)
{
    ui->setupUi(this);
    QList<QAbstractButton *> buttons = ui->buttons->buttons();
    for (int i = 0; i < buttons.size(); ++i)
    {
        QPushButton * thisButton = qobject_cast<QPushButton *>(buttons[i]);
        if (ui->buttons->buttonRole(thisButton) == QDialogButtonBox::AcceptRole)
        {
            ui->buttons->removeButton(thisButton);
            thisButton->setDefault(true);
            ui->buttons->addButton(thisButton,QDialogButtonBox::AcceptRole);
        }
    }

    settings.beginGroup(ui->tempoGroupBox->title());

    ui->useProvidedTempoRadioButton->setChecked(
                settings.value("inputTempoProvided",BeatAnalysis::Configuration::inputTempoProvided).toBool());
    ui->automaticTempoRadioButton->setChecked( ! ui->useProvidedTempoRadioButton->isChecked());
    ui->providedInputTempoSpinBox->setValue(
                settings.value("providedInputTempo",BeatAnalysis::Configuration::providedInputTempo).toDouble());
    QString automaticTempoDetectionSpeed(
                settings.value("automaticTempoDetectionSpeed",ui->normalRadioButton->text()).toString());
    if (automaticTempoDetectionSpeed == ui->fastRadioButton->text())
             ui->fastRadioButton->setChecked(true);
    else if (automaticTempoDetectionSpeed == ui->slowRadioButton->text())
        ui->slowRadioButton->setChecked(true);
    else /*(automaticTempoDetectionSpeed == ui->normalRadioButton->text())*/
        ui->normalRadioButton->setChecked(true);

    settings.endGroup();

    if (ui->automaticTempoRadioButton->isChecked())
        ui->providedInputTempoSpinBox->setEnabled(false);
    else
    {
        ui->fastRadioButton->setEnabled(false);
        ui->normalRadioButton->setEnabled(false);
        ui->slowRadioButton->setEnabled(false);
    }

    settings.beginGroup(ui->beatDetectionGroupBox->title());

    ui->maxPercentAcceptableBeatErrorSpinBox->setValue(
                settings.value("maxPercentAcceptableBeatError",BeatAnalysis::Configuration::maxPercentAcceptableBeatError).toFloat());
    ui->maxUniqueIntervalMergeCyclesSpinBox->setValue(
                settings.value("maxUniqueIntervalMergeCycles",BeatAnalysis::Configuration::maxUniqueIntervalMergeCycles).toInt());
    ui->detectTripletValuesCheckBox->setChecked(
                settings.value("allowTripletValues",BeatAnalysis::Configuration::allowTripletValues).toBool());
    ui->minNumberOfBeatsToQualifyAsBreakSpinBox->setValue(
                settings.value("minNumberOfBeatsToQualifyAsBreak",BeatAnalysis::Configuration::minNumberOfBeatsToQualifyAsBreak).toFloat());
    ui->allowHalfBeatsInBreaksCheckBox->setChecked(
                settings.value("allowHalfBeatsInBreaks",BeatAnalysis::Configuration::allowHalfBeatsInBreaks).toBool());
    ui->maxPermissibleProportionUnmatchedBeatsSpinBox->setValue(
                settings.value("maxPermissibleProportionUnmatchedBeats",BeatAnalysis::Configuration::maxPermissibleProportionUnmatchedBeats).toFloat() * 100);

    settings.endGroup();
    settings.beginGroup(ui->patternsGroupBox->title());

    ui->matchPatternMembersByNearestKnownValueCheckBox->setChecked(
                settings.value("matchPatternMembersByNearestKnownValue",BeatAnalysis::Configuration::matchPatternMembersByNearestKnownValue).toBool());
    ui->matchPatternMembersByActualValueCheckBox->setChecked(
                settings.value("matchPatternMembersByActualValue",BeatAnalysis::Configuration::matchPatternMembersByActualValue).toBool());
    ui->maxPatternLengthInStrokesSpinBox->setValue(
                settings.value("maxPatternLengthInStrokes",BeatAnalysis::Configuration::maxPatternLengthInStrokes).toInt());
    ui->allowOddPatternLengthsCheckBox->setChecked(
                settings.value("allowOddPatternLengths",BeatAnalysis::Configuration::allowOddPatternLengths).toBool());
    ui->preferLongPatternsCheckBox->setChecked(
                settings.value("preferLongPatterns",BeatAnalysis::Configuration::preferLongPatterns).toBool());
    ui->minLongPatternLengthInStrokesSpinBox->setValue(
                settings.value("minLongPatternLengthInStrokes",BeatAnalysis::Configuration::minLongPatternLengthInStrokes).toInt());
    ui->minLongPatternRepetitionsToPreferSpinBox->setValue(
                settings.value("minLongPatternRepetitionsToPrefer",BeatAnalysis::Configuration::minLongPatternRepetitionsToPrefer).toInt());

    settings.endGroup();

    if (ui->matchPatternMembersByActualValueCheckBox->isChecked() == false &&
            ui->matchPatternMembersByNearestKnownValueCheckBox->isChecked() == false)
    {
        ui->matchPatternMembersByActualValueCheckBox->setChecked(true);
        ui->matchPatternMembersByNearestKnownValueCheckBox->setChecked(true);
    }

    ui->minLongPatternLengthInStrokesSpinBox->setEnabled(ui->preferLongPatternsCheckBox->isChecked());
    ui->minLongPatternRepetitionsToPreferSpinBox->setEnabled(ui->preferLongPatternsCheckBox->isChecked());

    tapTempoTimer = new QTime();

    tempoTimeoutTimer = new QTimer(this);
    tempoTimeoutTimer->setTimerType(Qt::PreciseTimer);
    tempoTimeoutTimer->setSingleShot(true);
    tempoTimeoutTimer->setInterval(4000);
    connect(tempoTimeoutTimer, SIGNAL(timeout()), this, SLOT(onTempoTimeout()));
}

AnalysisOptionsDialog::~AnalysisOptionsDialog()
{
    delete ui;
}

void AnalysisOptionsDialog::on_useProvidedTempoRadioButton_clicked()
{
    ui->providedInputTempoSpinBox->setEnabled(true);
    ui->fastRadioButton->setEnabled(false);
    ui->normalRadioButton->setEnabled(false);
    ui->slowRadioButton->setEnabled(false);
}

void AnalysisOptionsDialog::on_automaticTempoRadioButton_clicked()
{
    ui->useProvidedTempoRadioButton->setChecked(false); //shouldn't have to do this, but something weird's afoot
    ui->fastRadioButton->setEnabled(true);
    ui->normalRadioButton->setEnabled(true);
    ui->slowRadioButton->setEnabled(true);
    ui->providedInputTempoSpinBox->setEnabled(false);
}

void AnalysisOptionsDialog::on_preferLongPatternsCheckBox_clicked()
{
    ui->minLongPatternLengthInStrokesSpinBox->setEnabled(ui->preferLongPatternsCheckBox->isChecked());
    ui->minLongPatternRepetitionsToPreferSpinBox->setEnabled(ui->preferLongPatternsCheckBox->isChecked());
}


void AnalysisOptionsDialog::on_matchPatternMembersByNearestKnownValueCheckBox_clicked()
{
    if (ui->matchPatternMembersByNearestKnownValueCheckBox->isChecked() == false)
        ui->matchPatternMembersByActualValueCheckBox->setChecked(true);
}

void AnalysisOptionsDialog::on_matchPatternMembersByActualValueCheckBox_clicked()
{
    if (ui->matchPatternMembersByActualValueCheckBox->isChecked() == false)
        ui->matchPatternMembersByNearestKnownValueCheckBox->setChecked(true);
}

void AnalysisOptionsDialog::on_buttons_accepted()
{
    settings.beginGroup(ui->tempoGroupBox->title());

    settings.setValue("inputTempoProvided",BeatAnalysis::Configuration::inputTempoProvided = ui->useProvidedTempoRadioButton->isChecked());
    if (ui->useProvidedTempoRadioButton->isChecked())
        settings.setValue("providedInputTempo",BeatAnalysis::Configuration::providedInputTempo = ui->providedInputTempoSpinBox->value());
    else
    {
        if (ui->fastRadioButton->isChecked())
        {
            settings.setValue("automaticTempoDetectionSpeed",ui->fastRadioButton->text());
            BeatAnalysis::Configuration::expectFastTempo();
        }
        else if (ui->slowRadioButton->isChecked())
        {
            settings.setValue("automaticTempoDetectionSpeed",ui->slowRadioButton->text());
            BeatAnalysis::Configuration::expectSlowTempo();
        }
        else //(ui->normalRadioButton->isChecked())
        {
            settings.setValue("automaticTempoDetectionSpeed",ui->normalRadioButton->text());
            BeatAnalysis::Configuration::expectNormalTempo();
        }
    }

    settings.endGroup();
    settings.beginGroup(ui->beatDetectionGroupBox->title());

    settings.setValue("maxPercentAcceptableBeatError",BeatAnalysis::Configuration::maxPercentAcceptableBeatError = ui->maxPercentAcceptableBeatErrorSpinBox->value());
    settings.setValue("maxUniqueIntervalMergeCycles",BeatAnalysis::Configuration::maxUniqueIntervalMergeCycles = ui->maxUniqueIntervalMergeCyclesSpinBox->value());
    settings.setValue("allowTripletValues",BeatAnalysis::Configuration::allowTripletValues = ui->detectTripletValuesCheckBox->isChecked());
    settings.setValue("minNumberOfBeatsToQualifyAsBreak",BeatAnalysis::Configuration::minNumberOfBeatsToQualifyAsBreak = ui->minNumberOfBeatsToQualifyAsBreakSpinBox->value());
    settings.setValue("allowHalfBeatsInBreaks",BeatAnalysis::Configuration::allowHalfBeatsInBreaks = ui->allowHalfBeatsInBreaksCheckBox->isChecked());
    settings.setValue("maxPermissibleProportionUnmatchedBeats",BeatAnalysis::Configuration::maxPermissibleProportionUnmatchedBeats = (ui->maxPermissibleProportionUnmatchedBeatsSpinBox->value() / 100));

    settings.endGroup();
    settings.beginGroup(ui->patternsGroupBox->title());

    settings.setValue("matchPatternMembersByNearestKnownValue",BeatAnalysis::Configuration::matchPatternMembersByNearestKnownValue = ui->matchPatternMembersByNearestKnownValueCheckBox->isChecked());
    settings.setValue("matchPatternMembersByActualValue",BeatAnalysis::Configuration::matchPatternMembersByActualValue = ui->matchPatternMembersByActualValueCheckBox->isChecked());
    settings.setValue("maxPatternLengthInStrokes",BeatAnalysis::Configuration::maxPatternLengthInStrokes = ui->maxPatternLengthInStrokesSpinBox->value());
    settings.setValue("allowOddPatternLengths",BeatAnalysis::Configuration::allowOddPatternLengths = ui->allowOddPatternLengthsCheckBox->isChecked());
    settings.setValue("preferLongPatterns",BeatAnalysis::Configuration::preferLongPatterns = ui->preferLongPatternsCheckBox->isChecked());
    if (ui->preferLongPatternsCheckBox->isChecked())
    {
        settings.setValue("minLongPatternLengthInStrokes",BeatAnalysis::Configuration::minLongPatternLengthInStrokes = ui->minLongPatternLengthInStrokesSpinBox->value());
        settings.setValue("minLongPatternRepetitionsToPrefer",BeatAnalysis::Configuration::minLongPatternRepetitionsToPrefer = ui->minLongPatternRepetitionsToPreferSpinBox->value());
    }

    settings.endGroup();
}

void AnalysisOptionsDialog::on_tapTempoInputButton_clicked()
{
    if ( ! ui->useProvidedTempoRadioButton->isChecked())
    {
        ui->useProvidedTempoRadioButton->setChecked(true);
    }
    if ( ! tempoTimeoutTimer->isActive())
    {
        tapTempoTimer->start();
        tapTempoCounter = 0;
        ui->tapTempoResetButton->setEnabled(true);
        previousTempo = ui->providedInputTempoSpinBox->value();
    }

    ++tapTempoCounter;
    tempoTimeoutTimer->start();

    if (tapTempoCounter == 1)
    {
        ui->tapTempoInputButton->setText("Keep tapping...");
    }
    else
    {
        QString buttonLabel = QString("Tap Tempo (%1)").arg(tapTempoCounter);
        ui->tapTempoInputButton->setText(buttonLabel);
        double beatInterval = ((double)tapTempoTimer->elapsed() / (double)(tapTempoCounter - 1));
        double tempo = (60 * 1000) / beatInterval;
        ui->providedInputTempoSpinBox->setValue(tempo);
    }
}

void AnalysisOptionsDialog::onTempoTimeout()
{
    ui->tapTempoInputButton->setText(QString("Tap Tempo..."));
    ui->tapTempoResetButton->setEnabled(false);
    tapTempoCounter = 0;
}

void AnalysisOptionsDialog::on_tapTempoResetButton_clicked()
{
    ui->tapTempoInputButton->setText(QString("Tap Tempo..."));
    ui->tapTempoResetButton->setEnabled(false);
    tapTempoCounter = 0;
    tempoTimeoutTimer->stop();
    ui->providedInputTempoSpinBox->setValue(previousTempo);
}
