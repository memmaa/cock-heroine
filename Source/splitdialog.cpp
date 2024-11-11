#include "globals.h"
#include "helperfunctions.h"
#include "splitdialog.h"
#include "ui_splitdialog.h"
#include "beatvaluewidget.h"
#include "beatpattern.h"
#include "editorwindow.h"
#include "ui_editorwindow.h"
#include <QSettings>
#include <QShortcut>
#include <QDebug>
#include <QLabel>
#include "uniquebeatinterval.h"
#include "beatanalysis.h"

SplitDialog::SplitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitDialog),
    pattern(new BeatPattern())
{
    ui->setupUi(this);
    int activeValuesCount = 0;
    foreach(BeatValue value, beatValues) {
        if (value.active)
            activeValuesCount++;
    }
    int numColumns = (activeValuesCount + 1) / 2;
    int widgetsAdded = 0;
    QSettings settings;
    for (int i = 0; i < beatValues.length(); ++i) {
        BeatValue value = beatValues[i];
        if (value.active)
        {
            int row = widgetsAdded / numColumns;
            int column = widgetsAdded % numColumns;
            QString shortcutString = settings.value(QString("SPLIT_DIALOG_SHORTCUT_ROW_%1_COLUMN_%2").arg(row).arg(column), defaultValueShortcuts[row][column]).toString();
            QKeySequence sequence(shortcutString);
            QShortcut * shortcut = new QShortcut(sequence,this);
            BeatValueWidget * valueWidget = new BeatValueWidget(ui->availableValuesFrame, value, BeatValueWidget::RepeatableSelection);
            connect(shortcut, SIGNAL(activated()), valueWidget, SLOT(select()));
            connect(valueWidget, SIGNAL(selected(BeatValue *)), this, SLOT(newValueSelected(BeatValue *)));
            ui->availableValuesLayout->addWidget(valueWidget, row, column);
            ++widgetsAdded;
        }
    }
    setShortcuts();
    setLabels();
    showEditorSplitPage();
    setButtonState();
}

void SplitDialog::setShortcuts()
{
    QSettings settings;

    //TODO: Below code copied from 'add' dialog. Update it for this dialog.
//    QString strKeyAddBefore = settings.value(SHORTCUT_ADD_INSERT_BEFORE_SELECTION, "K").toString();
//    QKeySequence keyAddBefore(strKeyAddBefore);
//    addBeforeShortcut = new QShortcut(keyAddBefore,this);
//    connect(addBeforeShortcut, SIGNAL(activated()), ui->beforeRadioButton, SLOT(click()));

//    QString strKeyAddAfter = settings.value(SHORTCUT_ADD_INSERT_AFTER_SELECTION, "J").toString();
//    QKeySequence keyAddAfter(strKeyAddAfter);
//    addAfterShortcut = new QShortcut(keyAddAfter,this);
//    connect(addAfterShortcut, SIGNAL(activated()), ui->afterRadioButton, SLOT(click()));

//    QString strKeyMoveBack = settings.value(SHORTCUT_ADD_MOVE_OTHERS_BACK, "L").toString();
//    QKeySequence keyMoveBack(strKeyMoveBack);
//    moveBackShortcut = new QShortcut(keyMoveBack,this);
//    connect(moveBackShortcut, SIGNAL(activated()), ui->shiftBackRadioButton, SLOT(click()));

//    QString strKeyMoveForward = settings.value(SHORTCUT_ADD_MOVE_OTHERS_FORWARD, "H").toString();
//    QKeySequence keyMoveForward(strKeyMoveForward);
//    moveForwardShortcut = new QShortcut(keyMoveForward,this);
//    connect(moveForwardShortcut, SIGNAL(activated()), ui->shiftForwardRadioButton, SLOT(click()));

    doneShortcut = new QShortcut(editor->ui->actionSplit->shortcut(),this);
    connect(doneShortcut, SIGNAL(activated()), this, SLOT(accept()));
}

void SplitDialog::setLabels()
{
    //TODO: See DeleteDialog and do the same here
}

SplitDialog::~SplitDialog()
{
    delete pattern;
    delete ui;
}

void SplitDialog::showEditorSplitPage()
{
    editor->ui->intervalEditorTabs->setCurrentWidget(editor->ui->splitIntervalsTab);
}

void SplitDialog::setButtonState()
{
    int totalMsSelected = editor->getContigousSelectionMilliseconds();
    float totalBeatsSelected = editor->getContigousSelectionBeats();

    BeatValue * shortestActiveValue = calculateShortestActiveBeatValue();

    if (totalMsSelected == -1 /*non-contigous selection*/ ||
            shortestActiveValue == NULL ||
            beatValues.isEmpty())
    {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!pattern->isEmpty());

    float remainingMsToAllocate = totalMsSelected - pattern->lengthInMsAtTempo();
    float remainingBeatsToAllocate = totalBeatsSelected - pattern->lengthInBeats();

    UniqueBeatInterval remainderInterval(remainingMsToAllocate);
    foreach (BeatValueWidget * widget, ui->availableValuesFrame->findChildren<BeatValueWidget *>())
    {
        if (remainingMsToAllocate > 0 &&
                (widget->getValue().getLength() <= remainingMsToAllocate ||
                remainderInterval.matchesThisInterval(widget->getValue().getLength()) ||
                widget->getValue().value() <= remainingBeatsToAllocate + 0.02 /* margin for rounding errors */
                 ))
        {
            widget->setEnabled(true);
        }
        else
        {
            widget->setEnabled(false);
        }
    }

    float proportionOfSpaceAllocated = pattern->lengthInMsAtTempo() / (float) totalMsSelected;
    if (proportionOfSpaceAllocated > 0.75 || pattern->lengthInMsAtTempo() * 2 > totalMsSelected + BeatAnalysis::Configuration::tempoInterval())
    {
        ui->loopToFillRadioButton->setEnabled(false);
        ui->loopToFillRadioButton->setChecked(false);
        ui->enterOnceOnlyRadioButton->setChecked(true);
    }
    else if (! ui->loopToFillRadioButton->isEnabled())
    {
        ui->loopToFillRadioButton->setEnabled(true);
        ui->loopToFillRadioButton->setChecked(true);
        ui->enterOnceOnlyRadioButton->setChecked(false);
    }
    else
    {
        ui->loopToFillRadioButton->setEnabled(true);
    }

    if (!ui->loopToFillRadioButton->isChecked() && !ui->enterOnceOnlyRadioButton->isChecked())
        ui->loopToFillRadioButton->setChecked(true);

    if (!ui->adjustTempoToFitRadioButton->isChecked() && !ui->useEstablishedTempoRadioButton->isChecked())
        ui->adjustTempoToFitRadioButton->setChecked(true);

    if (!pattern->isEmpty())
    {
        float thisDifference = 0, lastDifference = std::numeric_limits<float>::max(), lowestDifference = std::numeric_limits<float>::max();
        int repetitions = 1;
        while (true) {
            thisDifference = abs(totalMsSelected - (repetitions * pattern->lengthInMsAtTempo()));

            if (thisDifference < lowestDifference)
                lowestDifference = thisDifference;

            if (thisDifference > lastDifference)
                break;

            lastDifference = thisDifference;
            ++repetitions;
        }
    }

}

void SplitDialog::newValueSelected(BeatValue * newValue)
{
    pattern->append(newValue);
    refreshPatternDisplay();
    setButtonState();
}

void SplitDialog::refreshPatternDisplay()
{
    qDeleteAll(ui->selectedValuesFrame->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
    int framesPerBeat = pattern->isEmpty() ? 1 : lowestCommonMultiple(pattern->listOfDenominators());
    int labelCounter = 0;
    for (int i = 0; i < pattern->lengthInStrokes(); ++i)
    {
        QLabel * beatLabel = new QLabel("O", ui->selectedValuesFrame);
        ui->selectedValuesLayout->addWidget(beatLabel);
        ++labelCounter;
        int framesForThisBeat = pattern->at(i) ? roundToInt(pattern->at(i)->value() * framesPerBeat) : 1;
        for (int j = 1; j < framesForThisBeat; ++j)
        {
            QLabel * blankLabel = new QLabel(" ", ui->selectedValuesFrame);
            ui->selectedValuesLayout->addWidget(blankLabel);
            ++labelCounter;
        }
    }

    float beatsToReplace = editor->getContigousSelectionBeats();
    while (float(labelCounter) / float(framesPerBeat) < beatsToReplace)
    {
        QLabel * blankLabel = new QLabel(" ", ui->selectedValuesFrame);
        ui->selectedValuesLayout->addWidget(blankLabel);
        ++labelCounter;
    }
}

void SplitDialog::accept()
{
    editor->splitIntervals(*pattern, ui->loopToFillRadioButton->isChecked(), ui->adjustTempoToFitRadioButton->isChecked());
    QDialog::accept();
}

void SplitDialog::on_deleteButton_clicked()
{
    if (!pattern->isEmpty())
    {
        pattern->removeLast();
        refreshPatternDisplay();
        setButtonState();
    }
}
