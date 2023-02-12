#include "enableidentifyintervalsdialog.h"
#include "ui_enableidentifyintervalsdialog.h"
#include "beatanalysis.h"
#include "newbeatvaluewidget.h"
#include "newcustombeatvaluewidget.h"
#include <QPushButton>
#include "globals.h"
#include "uniquebeatinterval.h"
#include "helperfunctions.h"
#include "adjustdialog.h"
#include "editorwindow.h"
#include <QMessageBox>

EnableIdentifyIntervalsDialog::EnableIdentifyIntervalsDialog(int valueInMs, double currentTempo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnableIdentifyIntervalsDialog),
    valueInMs(valueInMs),
    tempo(currentTempo),
    originalTempo(currentTempo)
{
    ui->setupUi(this);

    calculateValueInBeats();

    ui->intervalLengthLabel->setText(QString("%1ms (%2.%3 seconds)").arg(valueInMs).arg(valueInMs / 1000).arg(valueInMs % 1000, 3, 10, QChar('0')));
    ui->beatsSpinBox->setValue(valueInBeats);

    QVector<int> divisors;
    divisors.append(1);
    divisors.append(2);
    if (BeatAnalysis::Configuration::allowTripletValues)
        divisors.append(3);
    divisors.append(4);
    if (BeatAnalysis::Configuration::allowTripletValues)
        divisors.append(6);
    divisors.append(8);
    if (BeatAnalysis::Configuration::allowTripletValues)
        divisors.append(9);
    if (BeatAnalysis::Configuration::allowTripletValues)
        divisors.append(12);
    divisors.append(16);
    int numColumns = (divisors.length() + 2 /* 1 for custom, and 1 for luck (a.k.a. integer division) */) / 2;
    int widgetsAdded = 0;
    for (int divisor : divisors) {
        //thing
        int row = widgetsAdded / numColumns;
        int column = widgetsAdded % numColumns;
//	    QString shortcutString = settings.value(QString("SPLIT_DIALOG_SHORTCUT_ROW_%1_COLUMN_%2").arg(row).arg(column), defaultValueShortcuts[row][column]).toString();
//	    QKeySequence sequence(shortcutString);
//	    QShortcut * shortcut = new QShortcut(sequence,this);
        NewBeatValueWidget * valueWidget = new NewBeatValueWidget(ui->valuesFrame, valueInBeats, divisor);
//        connect(shortcut, SIGNAL(activated()), valueWidget, SLOT(select()));
        connect(valueWidget, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
        connect(valueWidget, SIGNAL(selected(AbstractNewBeatValueWidget *)), this, SLOT(newValueSelected(AbstractNewBeatValueWidget *)));
        valueWidgets.append(valueWidget);
        ui->gridLayout->addWidget(valueWidget, row, column);
        ++widgetsAdded;
    }
    int row = widgetsAdded / numColumns;
    int column = widgetsAdded % numColumns;
    customValueWidget = new NewCustomBeatValueWidget(valueInBeats, ui->valuesFrame);
    connect(customValueWidget, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
    connect(customValueWidget, SIGNAL(selected(AbstractNewBeatValueWidget *)), this, SLOT(newValueSelected(AbstractNewBeatValueWidget *)));
    ui->gridLayout->addWidget(customValueWidget, row, column);
    valueWidgets.append(customValueWidget);

    selectBestMatch();
//    connect(ui->intervalNumeratorSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_numeratorValueChanged(int)));
//    setShortcuts();
//    setLabels();

    //this line at the end of the constructor because slots have already been connected by name at beginning of constructor, but
    //some widgets aren't ready yet at that point.
    ui->tempoSpinBox->setValue(currentTempo);
}

EnableIdentifyIntervalsDialog::~EnableIdentifyIntervalsDialog()
{
    delete ui;
}

void EnableIdentifyIntervalsDialog::accept()
{
    //Do we have a value to create or enable?
    if (ui->createOrEnableCheckBox->isChecked())
    {
        //Does the value have a name?
        if (ui->intervalNameEdit->text().isEmpty())
        {
            //we don't want to create a value with no name
            return;
        }
        //Do we have a bad match?
        if (currentlySelectedWidget->matchness() < BeatAnalysis::Configuration::badMatchThreshhold && !ui->openAdjustDialogCheckBox->isChecked())
        {
            QString choice = askAboutBadMatch();
            if (choice == "cancel")
                return;
            if (choice == "adjust")
                ui->openAdjustDialogCheckBox->setChecked(true);
        }
    }
    //Are we changing the tempo?
    if (ui->updateTempoCheckBox->isChecked())
    {
        //Do we have a big tempo change?
        if (tempoChangedLots())
        {
            if (!askAboutBigTempoChange())
            {
                return;
            }
        }
    }
    //Do we have any masking values?
    if (selectedValueIsMasked())
    {
        if (ui->disableMaskingValuesCheckBox->isChecked())
        {
            if (!confirmChoiceToDisableMaskingValues())
            {
                return;
            }
        }
        else if (!ui->openAdjustDialogCheckBox->isChecked()) //disable masking values checkbox is not checked
        {
            QString choice = askAboutMaskingValues();
            if (choice == "cancel")
                return;
            else if (choice == "adjust")
                ui->openAdjustDialogCheckBox->setChecked(true);
            else if (choice == "disable")
                ui->disableMaskingValuesCheckBox->setChecked(true);
        }
    }
    //That's all the opportunities for the user to change their mind, so...
    //Do we have a value to create or enable?
    BeatValue * existingValue = getExistingMatchingValue(getSelectedValueInBeats());
    BeatValue valueToAdjustTo;
    if (ui->createOrEnableCheckBox->isChecked())
    {
		if (existingValue)
        {
            existingValue->active = true;
            valueToAdjustTo = *existingValue;
        }
        else
        {
            BeatValue newValue = createSelectedValue();
            beatValues.append(newValue);
            valueToAdjustTo = beatValues.last();
        }
    }
    //Are we changing the tempo?
    if (ui->updateTempoCheckBox->isChecked())
    {
        BeatAnalysis::Configuration::currentBPM = tempo;
        BeatAnalysis::Configuration::inputTempoProvided = true;
    }
    //Are we disabling masking values?
    if (ui->disableMaskingValuesCheckBox->isChecked())
    {
        for (BeatValue maskingValue : getMaskingValues())
        {
            BeatValue * existingValue = getExistingMatchingValue(maskingValue.value());
            existingValue->active = false;
        }
    }
    //Do we want to adjust the interval?
    if (ui->openAdjustDialogCheckBox->isChecked())
    {
        AdjustDialog dialog(valueToAdjustTo, editor);
        dialog.exec();
    }
    QDialog::accept();
    editor->autoReanalyse();
}

void EnableIdentifyIntervalsDialog::on_tempoSpinBox_valueChanged(double newTempo)
{
    bool currentIsOriginal = (abs(tempo - originalTempo) < 0.01);
    bool newIsOriginal = (abs(newTempo - originalTempo) < 0.01);
    if (!newIsOriginal)
        lastNonOriginalTempo = newTempo;
    //if the tempo has changed from the original for the first time, tick the change tempo box
    if (currentIsOriginal && !newIsOriginal)
        ui->updateTempoCheckBox->setChecked(true);
    else if (newIsOriginal)
        ui->updateTempoCheckBox->setChecked(false);
    tempo = newTempo;
    calculateValueInBeats();
    ui->beatsSpinBox->setValue(valueInBeats);
    for (auto widget : valueWidgets)
    {
        widget->updateForValue(valueInBeats);
    }
    AbstractNewBeatValueWidget * widget = customValueWidget;
    if (!customValueWidget->isSelected())
    {
        widget = selectBestMatch();
    }
    if (widget)
    {
        updateDescriptiveText(widget);
    }
    updateCheckboxes();
}

void EnableIdentifyIntervalsDialog::on_intervalNumeratorSpinBox_valueChanged(int)
{
    customValueWidget->setValue(ui->intervalNumeratorSpinBox->value(), ui->intervalDivisorSpinBox->value());
    customValueWidget->select();
}

AbstractNewBeatValueWidget * EnableIdentifyIntervalsDialog::selectBestMatch()
{
    AbstractNewBeatValueWidget * bestMatch = nullptr;
    float bestValue = __FLT_MAX__;
    for (auto widget : valueWidgets)
    {
        float resistance = widget->getMatchResistance();
        if (resistance < bestValue)
        {
            bestMatch = widget;
            bestValue = resistance;
        }
    }
    if (bestMatch)
    {
        bestMatch->select();
    }
    return bestMatch;
}

void EnableIdentifyIntervalsDialog::ensureSingleSelection()
{
    for (auto widget : valueWidgets)
    {
        if (sender() != widget)
        {
            widget->deselect();
        }
    }
}

void EnableIdentifyIntervalsDialog::newValueSelected(AbstractNewBeatValueWidget * widget)
{
    currentlySelectedWidget = widget;
    const QSignalBlocker blocker(ui->intervalNumeratorSpinBox);
    customValueWidget->setValue(widget->getNumerator(), widget->getDivisor());
    updateDescriptiveText(widget);
    updateCheckboxes();
}

void EnableIdentifyIntervalsDialog::calculateValueInBeats()
{
    double msInBeat = (double) 60000 / tempo;
    valueInBeats = valueInMs / msInBeat;
}

double EnableIdentifyIntervalsDialog::getSelectedValueInBeats()
{
    return (double) ui->intervalNumeratorSpinBox->value() / ui->intervalDivisorSpinBox->value();
}

void EnableIdentifyIntervalsDialog::updateDescriptiveText(AbstractNewBeatValueWidget * widget)
{
    bool keepOldText = ui->intervalNumeratorSpinBox->value() == widget->getNumerator() &&
                       ui->intervalDivisorSpinBox->value() == widget->getDivisor() &&
                       widget->getName().isEmpty();
    if (!keepOldText)
    {
        ui->intervalNameEdit->setText(widget->getName());
    }

    bool valueExists = hasExistingMatchingValue(widget->getCurrentValue());
    ui->intervalNameEdit->setEnabled(!valueExists);
    if (valueExists && ui->intervalNameEdit->text().isEmpty())
    {
        ui->intervalNameEdit->setText(getExistingMatchingValue(widget->getCurrentValue())->name);
    }
    ui->intervalNumeratorSpinBox->setValue(widget->getNumerator());
    ui->intervalDivisorSpinBox->setValue(widget->getDivisor());
    if (!widget->isAMatch() || (widget->matchness() < BeatAnalysis::Configuration::badMatchThreshhold && !ui->openAdjustDialogCheckBox->isChecked()))
    {
        ui->intervalInfoLabel->setStyleSheet("color: red");
    }
    else
    {
        ui->intervalInfoLabel->setStyleSheet("");
    }
    ui->intervalInfoLabel->setText(QString("(%1 beats, %2% match)").arg(widget->getCurrentValue(),1,'f',4).arg(widget->matchness(),1,'f',0));
}

void EnableIdentifyIntervalsDialog::updateCheckboxes()
{
    BeatValue * existingValue = getExistingMatchingValue(currentlySelectedWidget->getCurrentValue());
    QString createOrEnableCheckboxString;
    if (existingValue && existingValue->active)
    {
        //the selected value already exists and is enabled, so can be neither created nor enabled
        ui->createOrEnableCheckBox->setChecked(false);
        ui->createOrEnableCheckBox->setEnabled(false);
        createOrEnableCheckboxString = (QApplication::translate("EnableIdentifyIntervalsDialog", "Enable '%1' value (%2 beats, already enabled)", nullptr));
    }
    else
    {
        bool wasEnabled = ui->createOrEnableCheckBox->isEnabled();
        if (!wasEnabled)
        {
            ui->createOrEnableCheckBox->setEnabled(true);
            ui->createOrEnableCheckBox->setChecked(true);
        }

        if (existingValue)
            createOrEnableCheckboxString = (QApplication::translate("EnableIdentifyIntervalsDialog", "Enable '%1' value (%2 beats)", nullptr));
        else
        {
            if (ui->intervalNameEdit->text().isEmpty())
                createOrEnableCheckboxString = (QApplication::translate("EnableIdentifyIntervalsDialog", "Create new%1 value (%2 beats, needs a name)", nullptr));
            else
                createOrEnableCheckboxString = (QApplication::translate("EnableIdentifyIntervalsDialog", "Create '%1' value (%2 beats)", nullptr));
        }
    }

    ui->createOrEnableCheckBox->setText(createOrEnableCheckboxString.arg(ui->intervalNameEdit->text()).arg(currentlySelectedWidget->getCurrentValue(),1,'f',4));

    QString tempoCheckboxString = QApplication::translate("EnableIdentifyIntervalsDialog", "Update tempo to %1 BPM", nullptr);
    ui->updateTempoCheckBox->setText(tempoCheckboxString.arg(ui->tempoSpinBox->value(),1,'f',2));
    QString tempoTextColor;
    if (tempoChangedLots())
    {
        ui->updateTempoCheckBox->setStyleSheet("color: red");
    }
    else
    {
        ui->updateTempoCheckBox->setStyleSheet("");
    }

    if (getSelectedValue().isPossibleMatchFor(valueInMs) && selectedValueIsMasked())
    {
        ui->disableMaskingValuesCheckBox->setEnabled(true);
        QString maskingCheckboxText = QApplication::translate("EnableIdentifyIntervalsDialog",
                                                       "Disable masking values (%1)",
                                                       "Value for 'disable masking values' checkbox when there are masking values");
        ui->disableMaskingValuesCheckBox->setText(maskingCheckboxText.arg(getMaskingValuesString()));
        if (!ui->openAdjustDialogCheckBox->isChecked())
            ui->disableMaskingValuesCheckBox->setStyleSheet("color: red");
    }
    else
    {
        ui->disableMaskingValuesCheckBox->setEnabled(false);
        ui->disableMaskingValuesCheckBox->setText(QApplication::translate("EnableIdentifyIntervalsDialog",
                                                                          "Disable masking values (none detected)",
                                                                          "Default value for 'disable masking values' checkbox"));
        ui->disableMaskingValuesCheckBox->setStyleSheet("");
    }

    QString adjustCheckboxString = QApplication::translate("EnableIdentifyIntervalsDialog", "Open dialog to ajust interval (currently %1ms) to match '%2' (%3ms)", nullptr);
    ui->openAdjustDialogCheckBox->setText(adjustCheckboxString.arg(valueInMs).arg(ui->intervalNameEdit->text()).arg(roundToInt(getSelectedValue().getLength(ui->updateTempoCheckBox->isChecked() ? tempo : originalTempo))));
    bool currentSelectionIsMatch = getSelectedValue().isPossibleMatchFor(valueInMs, ui->updateTempoCheckBox->isChecked() ? tempo : originalTempo);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled((currentSelectionIsMatch && !ui->intervalNameEdit->text().isEmpty()) || ui->openAdjustDialogCheckBox->isChecked());

}

BeatValue * EnableIdentifyIntervalsDialog::getExistingMatchingValue(float myValue)
{
    for (int i = 0; i < beatValues.size(); ++i)
    {
        float thisValue = beatValues[i].value();
        if (thisValue == myValue)
        {
            return &beatValues[i];
        }
    }
    return nullptr;
}

bool EnableIdentifyIntervalsDialog::hasExistingMatchingValue(float value)
{
    return getExistingMatchingValue(value) != nullptr;
}

BeatValue EnableIdentifyIntervalsDialog::getSelectedValue()
{
    if (hasExistingMatchingValue(currentlySelectedWidget->getCurrentValue()))
    {
        BeatValue * valuePtr = getExistingMatchingValue(currentlySelectedWidget->getCurrentValue());
        return *valuePtr;
    }
    return createSelectedValue();
}

BeatValue EnableIdentifyIntervalsDialog::createSelectedValue()
{
    BeatValue retVal(customValueWidget->getNumerator(),customValueWidget->getDivisor(), ui->intervalNameEdit->text());
    return retVal;
}

double EnableIdentifyIntervalsDialog::getPerfectMatchTempo()
{
    double valueToMatch = currentlySelectedWidget->getCurrentValue();
    double msPerBeat = valueInMs / valueToMatch;
    return 60000 / msPerBeat;
}

QVector<BeatValue> EnableIdentifyIntervalsDialog::getMaskingValues()
{
    QVector<BeatValue> retVal;
    UniqueBeatInterval actualInterval(valueInMs);
    double actualDeviation = percentageDifferenceBetween(actualInterval.getLength(), getSelectedValue().getLength(tempo));
    for (auto existingValue : beatValues)
    {
        double thisVariation = percentageDifferenceBetween(actualInterval.getLength(), existingValue.getLength(tempo));
        if (thisVariation < actualDeviation)
            retVal.append(existingValue);
    }
    return retVal;
}

QString EnableIdentifyIntervalsDialog::getMaskingValuesString()
{
    QString list("");
    auto maskingValues = getMaskingValues();
    for (int i = 0; i < maskingValues.size(); ++i) {
        if (i)
            list.append(", ");
        BeatValue value = maskingValues[i];
        list.append(value.name);
    }
    return list;
}

bool EnableIdentifyIntervalsDialog::selectedValueIsMasked()
{
    return ! getMaskingValues().isEmpty();
}

bool EnableIdentifyIntervalsDialog::tempoChangedLots()
{
    return percentageDifferenceBetween(originalTempo, tempo) > 5 || absoluteDifferenceBetween(originalTempo, tempo) > 5;
}

/*
 * Considerations for 'OK':
 *
 * Value: New (will be created), disabled (but will be enabled), enabled (so... nothing to do?)
 * Accuracy: Well-matched and best match (great!), poorly-matched or masked by another interval (consider nudging tempo, or interval length?), not matched (Wat r u doin'!?!)
 * Tempo: not changed (fine), changed a little (should consider updating tempo - especially if needed to match), changed a lot (BIG warning if tempo was set beforehand)
 *
 * Possible situations:
 * 1. assuming no tempo change and no masking
 * Nothing to do: The selected value exists, is enabled and already the best match for the given interval at both the previous and new tempo.
 * 		Maybe give them a little message to say they didn't need to do anything (so they don't expect something to happen)
 * Best: The selected value is new or disabled (to be enabled) and will be the best match for the given interval at both the previous and new tempo.
 * 		Hopefully they know what they're doing - we'll just do what's requested without asking.
 * Existing poor match: The selected value exists, is enabled and already the best for the given interval, but it's a poor match and they haven't changed the tempo.
 * 		"The selected value was already detected for this interval (Xms), but it's not a great match.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * 		Press Adjust to keep the selected value and move the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * Poor match: The selected value is new or disabled (to be enabled) but will be a poor match for the given interval at both the previous and new tempo
 * 		"The selected value (X/Y) is not a great match for the selected interval (Xms).
 * 		Press Adjust to adjust the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * No match: The selected value does not match the interval at all
 * 		Disable OK button, message explains why:
 * 		"To successfully identify the interval of Xms, you must select a value that matches it. Try selecting a different value, or adjusting the tempo until the selected value matches this interval."
 *
 * 2. considering masking...
 * Existing masked match: The selected value is already enabled, and would match the interval if it wasn't better-matched by other enabled values.
 * 		Consider:
 * 		Disabling masking values
 * 		Adjusting interval to better match selected value
 * 		Adjusting tempo to change best match
 *
 * New masked match: The selected value is new or disabled (to be enabled), and would match the interval if it wasn't better-matched by other enabled values.
 * 		Consider:
 * 		Disabling masking values
 * 		Adjusting interval to better match selected value
 * 		Adjusting tempo to change best match
 * 3. considering tempo changes
 * Nothing to do: The selected value exists, is enabled and already the best match for the given interval at both the previous and new tempo (small tempo change).
 * 		Maybe give them a little message to say they didn't need to do anything (so they don't expect something to happen)
 * Nothing to do: The selected value exists, is enabled and already the best match for the given interval at both the previous and new tempo (big tempo change).
 * 		This seems unlikely to happen, but sure... given that they don't need to change the tempo, ask if they're really sure
 * Best: The selected value is new or disabled (to be enabled) and will be the best match for the given interval at the new tempo (small tempo change) and matches at the existing tempo.
 * 		Give them the option to change the tempo, but not recommend it (enable checkbox but don't tick?). Otherwise, just go ahead.
 * Match requires large tempo change: The selected value is new or disabled (to be enabled) and will be the best match for the given interval at the new tempo, but (large tempo change) not at the previous tempo.
 * 		"You have made a large change to the tempo. Large tempo changes may cause any previously selected values to no longer match."
 * 		Press Cancel to go back and change the tempo or select a different value.
 * 		[Cancel] [Continue]
 * Existing poor match: The selected value exists, is enabled and already the best for the given interval, but it's a poor match and they changed the tempo a little.
 * 		Enable tempo change if the new tempo is a better match than the old.
 * 		If they haven't already selected 'open adjust dialog', then warn them:
 * 		"The selected value was already detected for this interval (Xms), but it's not a great match.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * 		Press Adjust to keep the selected value and move the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * Existing poor match: The selected value exists, is enabled and already the best for the given interval, but it's a poor match and they changed the tempo quite a lot.
 * 		Enable tempo change if the new tempo is a better match than the old.
 * 		If they have enabled the 'update tempo' box, warn them:
 * 		"The selected value (X) is a poor match for this interval (Xms), and you have made a large change to the tempo. Large tempo changes may cause any previously selected values to no longer match."
 * 		Press Cancel to go back and select a different value or adjust the tempo to improve the match.
 * 		[Cancel] [Continue]
 * 		If they HAVEN'T enabled the 'update tempo' box, and they HAVEN'T selected to adjust the interval, warn them (this is the same as they don't change the tempo):
 * 		"The selected value was already detected for this interval (Xms), but it's not a great match.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * 		Press Adjust to keep the selected value and move the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * 		Else if they didn't choose to change tempo, but they are choosing to adjust the inteval, then they're probably doing the right thing. Continue without asking.
 * Poor match: The selected value is new or disabled (to be enabled) but will be a poor match for the given interval at the new tempo, which has been changed a little.
 * 		"The selected value (X/Y) is not a great match for the selected interval (Xms).
 * 		Press Adjust to adjust the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * Poor match: The selected value is new or disabled (to be enabled) but will be a poor match for the given interval at the new tempo, which has been changed a lot.
 * 		If they have enabled the 'update tempo' box (or if it's been enabled for them), warn them:
 * 		"The selected value (X) is a poor match for this interval (Xms), and you have made a large change to the tempo. Large tempo changes may cause any previously selected values to no longer match."
 * 		Press Cancel to go back and select a different value or adjust the tempo to improve the match.
 * 		[Cancel] [Continue]
 * 		If they HAVEN'T enabled the 'update tempo' box, and they HAVEN'T selected to adjust the interval, warn them (this is the same as they don't change the tempo):
 * 		"The selected value (X/Y) is not a great match for the selected interval (Xms).
 * 		Press Adjust to adjust the beats to better match the selected value.
 * 		Press Accept to keep the selected value and close the dialog without making any changes.
 * 		Press Cancel to go back and select a different value or adjust the tempo to improvee the match.
 * 		Otherwise in the unlikely event that the interval matches at both tempos, if they didn't choose to change tempo, but they are choosing to adjust the inteval, then let them do their thing. Continue without asking.
 * Only matches because of tempo change:
 * 		I don't think this is a thing, but: if selected value only matches at new tempo, but change tempo is not selected, then the input is invalid - disable OK and explain.
 * No match: The selected value does not match the interval at all
 * 		Disable OK button, message explains why:
 * 		"To successfully identify the interval of Xms, you must select a value that matches it. Try selecting a different value, or adjusting the tempo until the selected value matches this interval."
 *
 * 4. considering masking AND tempo changes??
 * All cases: Do a check to see if selection will not match because of masking, as above.
*/
QString EnableIdentifyIntervalsDialog::askAboutMaskingValues()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Masking Values Detected"));
    QString description = QString(tr("The selected value (%1/%2) will not match the selected interval (%3ms) because the following value(s) are a closer match:\n"
                "%4\n\n"
                "Press Adjust to adjust the length of the interval to perfectly match the selected value at the selected tempo.\n"
                "Press Disable to disable the closer-matching values mentioned above. You should only do this if you are sure those values should never occur in the beats currently loaded into the editor.\n"
                "Press Cancel to go back and adjust the tempo to change the best match."))
            .arg(ui->intervalNumeratorSpinBox->value())
            .arg(ui->intervalDivisorSpinBox->value())
            .arg(valueInMs)
            .arg(valueInMs);
    msgBox.setText(description);
    QPushButton * adjustButton = msgBox.addButton(tr("Adjust"), QMessageBox::ActionRole);
    QPushButton * disableButton = msgBox.addButton(tr("Accept"), QMessageBox::DestructiveRole);
    QPushButton * cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);

    msgBox.exec();

    if (msgBox.clickedButton() == disableButton)
        return "disable";
    if (msgBox.clickedButton() == adjustButton)
        return "adjust";
    return "cancel";
}

bool EnableIdentifyIntervalsDialog::askAboutBigTempoChange()
{
    QString description = QString(tr("The tempo has been changed from %1 BPM to %2 BPM.\n"
                "Large changes in tempo can cause intervals to no longer be recognised, or to be recognised as a different value\n"
                "Are you sure you want to change the tempo?"))
            .arg(originalTempo,1,'f',2)
            .arg(tempo,1,'f',2);
    return QMessageBox::question(this, tr("Large Tempo Change"), description) == QMessageBox::Yes;
}

bool EnableIdentifyIntervalsDialog::confirmChoiceToDisableMaskingValues()
{
    QString description = QString(tr("You have chosen to disable masking values\n"
                "You should only do this if you are certain that the following values will never occur in the current editor content:\n\n"
                "%1\n\n"
                "Are you sure you want to disable these values?"))
            .arg(getMaskingValuesString());
    return QMessageBox::question(this, tr("Disable Masking Values?"), description) == QMessageBox::Yes;
}

QString EnableIdentifyIntervalsDialog::askAboutBadMatch()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Bad Match"));
    QString description = QString(tr("The selected value (%1/%2) is not a great match for the selected interval (%3ms).\n\n"
                "Press Adjust to adjust the beats to better match the selected value.\n"
                "Press Accept to keep the selected value and close the dialog without making any changes.\n"
                "Press Cancel to go back and select a different value or adjust the tempo to improve the match.\n"))
            .arg(ui->intervalNumeratorSpinBox->value())
            .arg(ui->intervalDivisorSpinBox->value())
            .arg(valueInMs);
    msgBox.setText(description);
    QPushButton * continueButton = msgBox.addButton(tr("Accept"), QMessageBox::AcceptRole);
    QPushButton * adjustButton = msgBox.addButton(tr("Adjust"), QMessageBox::ActionRole);
    QPushButton * cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(cancelButton);

    msgBox.exec();

    if (msgBox.clickedButton() == continueButton)
        return "continue";
    if (msgBox.clickedButton() == adjustButton)
        return "adjust";
    return "cancel";
}

void EnableIdentifyIntervalsDialog::on_intervalNameEdit_textChanged(const QString &name)
{
    if (name.isEmpty())
    {
        ui->intervalNameEdit->setStyleSheet("background-color: red");
    }
    else
    {
        ui->intervalNameEdit->setStyleSheet("");
    }
    updateCheckboxes();
    /*
     * I think the below code was written with the intention of updating the 'OK' button to say something else like 'Create' or 'Enable', but for now, ignore this...
    if (hasExistingMatchingValue(getSelectedValueInBeats()))
    {
        if (currentValue.matchesInterval(valueInMs))
        {
            //this is already detected correctly
        }
    }
    */
}




void EnableIdentifyIntervalsDialog::on_originalTempoButton_clicked()
{
    ui->tempoSpinBox->setValue(originalTempo);
}

void EnableIdentifyIntervalsDialog::on_perfectMatchTempoButton_clicked()
{
    double newTempo = getPerfectMatchTempo();
    ui->tempoSpinBox->setValue(newTempo);
}

void EnableIdentifyIntervalsDialog::on_minimumNecessaryChangeTempoButton_clicked()
{
    //first of all, do we need a change?
    BeatValue * valueToMatch = getBeatValueFromLengthInBeats(currentlySelectedWidget->getCurrentValue());
    bool valueAlreadyExists = valueToMatch != nullptr;

    BeatValue newValue;
    if (!valueAlreadyExists)
    {
        newValue = createSelectedValue();
        valueToMatch = &newValue;
        beatValues.append(newValue);
    }
    if (valueToMatch->isBestEnabledMatchFor(valueInMs, originalTempo))
    {
        //we don't need to budge the tempo at all.
        if (!valueAlreadyExists)
        {
            beatValues.removeLast();
        }
        on_originalTempoButton_clicked();
        return;
    }
    else
    {
        //we're going to have to budge the tempo at least a bit.
        //let's see how far we have to move for a perfect match
        //positive result means we need to increase the tempo, negative means decrease
        double fullDifference = getPerfectMatchTempo() - originalTempo;
        //starts at 1 (for the whole difference) and reduces towards 0 as far as possible
        float diffMultiplier = 1;
        for (float diffStep = 0.5; abs(fullDifference * diffStep) > 0.01; diffStep /= 2)
        {
            //try reducing the change by the step
            double targetTempo = originalTempo + (fullDifference * (diffMultiplier - diffStep));
            if (valueToMatch->isBestEnabledMatchFor(valueInMs,targetTempo))
            {
                //we can reduce the difference by the step amount and still match, so do that
                diffMultiplier -= diffStep;
            }
            //else reducing by this much causes it to no longer match, so just try again with a smaller step size
        }
        //we should now have the smallest tempo change that will cause the selected value to match:
        double newTempo = originalTempo + (fullDifference * diffMultiplier);
        if (!valueAlreadyExists)
        {
            beatValues.removeLast();
        }
        ui->tempoSpinBox->setValue(newTempo);
    }
}

void EnableIdentifyIntervalsDialog::on_updateTempoCheckBox_stateChanged(int newState)
{
    if(newState == Qt::CheckState::Unchecked)
        ui->tempoSpinBox->setValue(originalTempo);
    else
        ui->tempoSpinBox->setValue(lastNonOriginalTempo);
    updateCheckboxes();
}

void EnableIdentifyIntervalsDialog::on_openAdjustDialogCheckBox_stateChanged(int)
{
    updateDescriptiveText(currentlySelectedWidget);
}

void EnableIdentifyIntervalsDialog::on_openAdjustDialogCheckBox_clicked()
{
    updateCheckboxes();
}
