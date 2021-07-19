#include "editorwindow.h"
#include "ui_editorwindow.h"
#include "beatdatamodel.h"
#include "intervaldatamodel.h"
#include "patterndatamodel.h"
#include "valuedatamodel.h"
#include "globals.h"
#include <QDateTime>
#include <QFont>
#include "beattimestamp.h"
#include "beatinterval.h"
#include "uniquebeatinterval.h"
#include <QDebug>
#include "config.h"
#include "beatanalysis.h"
#include "QMediaPlayer"
#include "helperfunctions.h"
#include "analysisoptionsdialog.h"
#include <QMessageBox>
#include "optimisationoptionsdialog.h"
#include "beatoptimisation.h"
#include "mainwindow.h"
#include <QtAlgorithms>
#include <QIntValidator>
#include <QStack>
#include <float.h>
#include <math.h>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QVariant>
#include "adddialog.h"
#include "adjustdialog.h"
#include "deletedialog.h"
#include "splitdialog.h"

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow),
    valueTableKeyboardHandler(new ValueTableKeyboardEventHandler(this)),
    controllingWidget(NULL),
    adjustValueValidator(NULL),
    strokeMeterScene(NULL),
    undoStackBookmark(0),
    beatsTabWasAutoSelected(false),
    beatModel(new BeatDataModel(this)),
    intervalModel(NULL),
    patternModel(NULL),
    valueModel(NULL)
{
    ui->setupUi(this);

    ui->beatValuesTable->installEventFilter(valueTableKeyboardHandler);

    ui->beatsTable->setModel(beatModel);
    setColumWidthsFromModel(ui->beatsTable);
    connect(ui->beatsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentBeatTimestampChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui->beatsTable->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),this,SLOT(on_selectedBeatTimestampsChanged()));

    ui->addIntervalValueComboBox->setValidator(new QIntValidator(0,INT_MAX,ui->addIntervalValueComboBox));

    adjustValueValidator = new QIntValidator(1,1,ui->adjustIntervalNewValueComboBox);
    ui->adjustIntervalNewValueComboBox->setValidator(adjustValueValidator);
    connect(ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox,SIGNAL(clicked()),this,SLOT(repopulateAdjustIntervalValueComboBox()));

    QFont consoleFont("Courier");
    consoleFont.setStyleHint(QFont::TypeWriter);
    ui->console->setFont(consoleFont);
    ui->console->setReadOnly(true);

    takeVideoOutput();

    strokeMeterScene = new QGraphicsScene(this);
    ui->strokeMeter->setScene(strokeMeterScene);
    connect(strokeMeterScene, SIGNAL(selectionChanged()),this,SLOT(on_selectedStrokeMarkersChanged()));
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(scrollStrokeMeterToTimestamp()));
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

void EditorWindow::takeVideoOutput()
{
    videoPlayer->stop();
    videoPlayer->setVideoOutput(ui->videoGoesHere);
    videoPlayer->pause();
}

void EditorWindow::mousePressEvent (QMouseEvent * ev)
{
    QObject * thingClicked = (QObject *)(qApp->widgetAt(QCursor::pos()));
    do
    {
        if (thingClicked == ui->videoGoesHere)
        {
            mainWindow->pressPlay();
            break;
        }
        thingClicked = thingClicked->parent();
    } while (thingClicked != NULL);

    ev->ignore();
}

void EditorWindow::closeEvent(QCloseEvent *)
{
    clear();
    mainWindow->takeVideoOutput();
}

void EditorWindow::clear()
{
    disconnect(ui->beatsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentBeatTimestampChanged(const QModelIndex &, const QModelIndex &)));
    disconnect(ui->beatsTable->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),this,SLOT(on_selectedBeatTimestampsChanged()));
    ui->beatsAndIntervalsTabs->setCurrentWidget(ui->beatsTab);
    while (beatModel->rowCount() > 0)
        beatModel->removeRow(0);
    beatModel->cancelQueuedDeletions();
    connect(ui->beatsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentBeatTimestampChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui->beatsTable->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),this,SLOT(on_selectedBeatTimestampsChanged()));

    if (ui->intervalsTable->model())
    {
        QAbstractItemModel * toDelete = ui->intervalsTable->model();
        QItemSelectionModel * toAlsoDelete = ui->intervalsTable->selectionModel();
        ui->intervalsTable->setModel(NULL);
        delete toDelete;
        delete toAlsoDelete;
        intervalModel = NULL;
    }

    if (ui->patternsTable->model())
    {
        QAbstractItemModel * toDelete = ui->patternsTable->model();
        QItemSelectionModel * toAlsoDelete = ui->patternsTable->selectionModel();
        ui->patternsTable->setModel(NULL);
        delete toDelete;
        delete toAlsoDelete;
        patternModel = NULL;
    }

    if (ui->beatValuesTable->model())
    {
        QAbstractItemModel * toDelete = ui->beatValuesTable->model();
        QItemSelectionModel * toAlsoDelete = ui->beatValuesTable->selectionModel();
        ui->beatValuesTable->setModel(NULL);
        delete toDelete;
        delete toAlsoDelete;
        valueModel = NULL;
    }

    BeatAnalysis::Configuration::tempoEstablished = false;

    ui->analyseButton->setEnabled(false);
    ui->adjustButton->setEnabled(false);
    ui->optimiseButton->setEnabled(false);
}

void EditorWindow::setBeatTimestamps (EventDataProxyModel * model, const QModelIndexList & eventIndexes)
{
    clear();
    beatModel->addBeats(model,eventIndexes);
    ui->analyseButton->setEnabled(true);
    if (!valueModel)
    {
        valueModel = new ValueDataModel(this);
        ui->beatValuesTable->setModel(valueModel);
        setColumWidthsFromModel(ui->beatValuesTable);
    }

    refreshStrokeMeter();
    ui->strokeMeter->centerOn(strokeMarkers.at(0));

    on_currentBeatTimestampChanged(ui->beatsTable->model()->index(0,0),QModelIndex());
    takeVideoOutput();
    on_analyseButton_clicked();
}

void EditorWindow::outputToConsole(QString text, bool onNewLine)
{
    if (onNewLine)
        ui->console->appendPlainText(text);
    else
    {
        if (ui->console->textCursor().atEnd() == false)
        {
            QTextCursor cursorAtEnd = ui->console->textCursor();
            cursorAtEnd.movePosition(QTextCursor::End);
            ui->console->setTextCursor(cursorAtEnd);
        }
        ui->console->insertPlainText(text);
    }
    ui->console->ensureCursorVisible();
    QCoreApplication::processEvents();
}

void EditorWindow::printConsoleBanner(QString text)
{
    outputToConsole("********************************************************************************",true);

    outputToConsole("* ",true);

    int paddingLength = 51 - text.length();
    int i = 0;
    for (i = 0; i < paddingLength / 2; ++i)
        outputToConsole(" ");
    outputToConsole(text);
    outputToConsole(" ");
    outputToConsole(QDateTime::currentDateTime().toString(" ddd dd MMM yyyy hh:mm:ss"));
    for ( ; i < paddingLength; ++i)
        outputToConsole(" ");
    outputToConsole("* ",false);

    outputToConsole("********************************************************************************",true);
}

void EditorWindow::on_selectedBeatTimestampsChanged()
{
    //TODO: This is a good candidate for optimisation - this has lots of loops and can be very slow with large selections
    if (!controllingWidget)
        controllingWidget = ui->beatsTable;
    bool thisWasClicked = (controllingWidget == ui->beatsTable);

    QModelIndex current = ui->beatsTable->currentIndex();
    if (current == QModelIndex())
        return;

    if (multipleRowsSelected(ui->beatsTable->selectionModel()->selectedRows()))
    {
        //deal with multiple beats
        int i = 0;

        ui->beatTimestampSpinbox->setRange(INT_MIN,INT_MAX);
        ui->beatTimestampSpinbox->setValue(0);
        ui->beatTimestampSpinbox->setButtonSymbols(QAbstractSpinBox::PlusMinus);

        //do we have more than one beat type selected?
        bool multipleBeatTypesSelected = false;
        char firstType = beatTimestamps[current.row()].eventData.type();
        QModelIndexList selection = ui->beatsTable->selectionModel()->selectedIndexes();
        int size = selection.size();
        for (i = 0; i < size; ++i)
        {
            if (beatTimestamps[selection[i].row()].eventData.type() != firstType)
            {
                multipleBeatTypesSelected = true;
                break;
            }
        }
        if (multipleBeatTypesSelected)
        {
            ui->beatTypeSpinbox->setRange(-1,127);
            ui->beatTypeSpinbox->setSpecialValueText(QString("No change"));
            ui->beatTypeSpinbox->setValue(-1);
        }
        else
        {
            ui->beatTypeSpinbox->setRange(0,127);
            ui->beatTypeSpinbox->setSpecialValueText(QString(""));
            ui->beatTypeSpinbox->setValue(beatTimestamps[current.row()].eventData.type());
        }


        //do we have more than one beat value selected?
        bool multipleBeatValuesSelected = false;
        short firstValue = beatTimestamps[current.row()].eventData.value;
        selection = ui->beatsTable->selectionModel()->selectedIndexes();
        size = selection.size();

        for (i = 0; i < size; ++i)
        {
            if (beatTimestamps[selection[i].row()].eventData.value != firstValue)
            {
                multipleBeatValuesSelected = true;
                break;
            }
        }
        if (multipleBeatValuesSelected)
        {
            ui->beatValueSpinbox->setRange(-1,SHRT_MAX);
            ui->beatValueSpinbox->setSpecialValueText(QString("No change"));
            ui->beatValueSpinbox->setValue(-1);
        }
        else
        {
            ui->beatValueSpinbox->setRange(0,SHRT_MAX);
            ui->beatValueSpinbox->setSpecialValueText(QString(""));
            ui->beatValueSpinbox->setValue(beatTimestamps[current.row()].eventData.value);
        }



        //do we have both optional and mandatory beats selected?
        bool optionalAndMandatoryBeatsSelected = false;
        bool firstBeatOptionality = beatTimestamps[current.row()].eventData.isOptional();
        selection = ui->beatsTable->selectionModel()->selectedIndexes();
        size = selection.size();
        for (i = 0; i < size; ++i)
        {
            if (beatTimestamps[selection[i].row()].eventData.isOptional() != firstBeatOptionality)
            {
                optionalAndMandatoryBeatsSelected = true;
                break;
            }
        }
        if (optionalAndMandatoryBeatsSelected)
        {
            ui->beatOptionalCheckbox->setTristate(true);
            ui->beatOptionalCheckbox->setCheckState(Qt::PartiallyChecked);
        }
        else
        {
            ui->beatOptionalCheckbox->setTristate(false);
            ui->beatOptionalCheckbox->setChecked(beatTimestamps[current.row()].eventData.isOptional());
        }
        ui->newBeatButton->setEnabled(false);
    }
    else if (! ui->beatsTable->selectionModel()->selectedRows().isEmpty())
    {
        //deal with just one beat:
        ui->beatTimestampSpinbox->setRange(0,INT_MAX);
        ui->beatTimestampSpinbox->setPrefix("");
        ui->beatTimestampSpinbox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        ui->beatTimestampSpinbox->setValue(beatTimestamps.at(current.row()).eventData.timestamp);
        ui->beatTypeSpinbox->setValue(beatTimestamps.at(current.row()).eventData.type());
        ui->beatTypeSpinbox->setRange(0,127);
        ui->beatTypeSpinbox->setSpecialValueText(QString(""));
        ui->beatValueSpinbox->setValue(beatTimestamps.at(current.row()).eventData.value);
        ui->beatValueSpinbox->setRange(0,SHRT_MAX);
        ui->beatValueSpinbox->setSpecialValueText(QString(""));
        ui->beatOptionalCheckbox->setTristate(false);
        ui->beatOptionalCheckbox->setChecked(beatTimestamps.at(current.row()).eventData.isOptional());
        ui->newBeatButton->setEnabled(true);
    }

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::on_currentBeatTimestampChanged(const QModelIndex & current, const QModelIndex &)
{
    if (!controllingWidget)
        controllingWidget = ui->beatsTable;
    bool thisWasClicked = (controllingWidget == ui->beatsTable);

    if ( ! currentlyPlaying &&
         thisWasClicked &&
         current != QModelIndex() &&
         current.column() == 0)
    {
        ui->centralStack->setCurrentWidget(ui->videoPage);
        seekToTimestamp(current.data().toLongLong());
        scrollStrokeMeterToTimestamp();
        ui->intervalsTable->selectRow(current.row());
    }

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::on_currentBeatIntervalChanged(const QModelIndex & current, const QModelIndex &)
{
    if (!controllingWidget)
        controllingWidget = ui->intervalsTable;
    bool thisWasClicked = (controllingWidget == ui->intervalsTable);

    if (thisWasClicked && current != QModelIndex())
    {
        QModelIndex startingIndex = ui->beatsTable->model()->index(current.row(),0);
        QModelIndex endingIndex = ui->beatsTable->model()->index(current.row()+1,0);
        if ( ! currentlyPlaying)
        {
            ui->centralStack->setCurrentWidget(ui->videoPage);
            seekToTimestamp(startingIndex.data().toLongLong());
            scrollStrokeMeterToTimestamp();
        }
        ui->beatsTable->selectionModel()->select(startingIndex,QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        ui->beatsTable->selectionModel()->select(endingIndex,QItemSelectionModel::Select/* | QItemSelectionModel::Rows*/);
        ui->beatsTable->scrollTo(startingIndex,QAbstractItemView::PositionAtCenter);
    }

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::on_selectedBeatIntervalsChanged()
{
    if (!controllingWidget)
        controllingWidget = ui->intervalsTable;
    bool thisWasClicked = (controllingWidget == ui->intervalsTable);

    repopulateAdjustIntervalValueComboBox();
    repopulateSplitIntervalValueComboBox();

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::on_currentPatternChanged(const QModelIndex & current, const QModelIndex &)
{
    if (!controllingWidget)
        controllingWidget = ui->patternsTable;
    bool thisWasClicked = (controllingWidget == ui->patternsTable);

    if (thisWasClicked)
    {
        QModelIndex timestampIndex = ui->beatsTable->model()->index(beatPatterns[current.row()].startsAtBeat(),0);
        QModelIndex intervalIndex = ui->intervalsTable->model()->index(beatPatterns[current.row()].startsAtBeat(),1);
        ui->beatsTable->selectionModel()->select(timestampIndex,QItemSelectionModel::ClearAndSelect/* | QItemSelectionModel::Rows*/);
        ui->intervalsTable->selectionModel()->select(intervalIndex,QItemSelectionModel::ClearAndSelect/* | QItemSelectionModel::Rows*/);
        ui->beatsTable->scrollTo(timestampIndex,QAbstractItemView::PositionAtCenter);
        ui->intervalsTable->scrollTo(intervalIndex);

        seekToTimestamp(timestampIndex.data().toLongLong());
        scrollStrokeMeterToTimestamp();
        ui->centralStack->setCurrentWidget(ui->videoPage);

        int i = 1;
        for (i = 1; i < beatPatterns[current.row()].totalStrokesCovered(); ++i)
        {
            timestampIndex = ui->beatsTable->model()->index(beatPatterns[current.row()].startsAtBeat() + i,0);
            ui->beatsTable->selectionModel()->select(timestampIndex,QItemSelectionModel::Select/* | QItemSelectionModel::Rows*/);
            intervalIndex = ui->intervalsTable->model()->index(beatPatterns[current.row()].startsAtBeat() + i,1);
            ui->intervalsTable->selectionModel()->select(intervalIndex,QItemSelectionModel::Select/* | QItemSelectionModel::Rows*/);
        }
        ui->beatsTable->scrollTo(timestampIndex);
        ui->intervalsTable->scrollTo(intervalIndex);
    }

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::prepareForReanalysis()
{
    uniqueBeatIntervals.clear();
    beatPatterns.clear();
    BeatAnalysis::Configuration::tempoEstablished = false;
}

void EditorWindow::runBeatAnalysis()
{
    prepareForReanalysis();

    initialiseBeatIntervals();

    if ( ! BeatAnalysis::calculateUniqueBeatIntervals())
    {
        printConsoleBanner("Analysis Failed");
        return;
    }

    if ( ! BeatAnalysis::calculateTempo())
    {
        printConsoleBanner("Analysis Failed");
        return;
    }

    BeatAnalysis::calculateBreakValues();
    if ( ! BeatAnalysis::analyseUniqueBeatIntervals())
    {
        printConsoleBanner("Analysis Failed");
        return;
    }
    repopulateAddIntervalValueComboBox();

    BeatAnalysis::analysePatterns();
    BeatAnalysis::consolidatePatterns();

    ui->beatsAndIntervalsTabs->setCurrentWidget(ui->intervalsTab);
    if (!intervalModel)
        initialiseIntervalsTable();
    if (!patternModel)
        initialisePatternsTable();
    refreshStrokeMeter();

    valueModel->recalculateCounts();

    ui->adjustButton->setEnabled(true);
    ui->optimiseButton->setEnabled(true);

    printConsoleBanner("Analysis Complete");
    ui->intervalsTable->repaint();
    ui->patternsTable->repaint();
}

void EditorWindow::autoReanalyse()
{
    printConsoleBanner("Beginning Automatic Reanalysis");
    prepareForReanalysis();

    initialiseBeatIntervals();

    if ( ! BeatAnalysis::calculateUniqueBeatIntervals())
    {
        refreshStrokeMeter();
        ui->centralStack->setCurrentWidget(ui->consolePage);
        printConsoleBanner("Automatic Reanalysis Failed");
        return;
    }

    if ( ! BeatAnalysis::calculateTempo())
    {
        refreshStrokeMeter();
        ui->centralStack->setCurrentWidget(ui->consolePage);
        printConsoleBanner("Automatic Reanalysis Failed");
        return;
    }

//    BeatAnalysis::calculateBreakValues();
//    if ( ! BeatAnalysis::analyseUniqueBeatIntervals())
//    {
//        ui->centralStack->setCurrentWidget(ui->consolePage);
//        printConsoleBanner("Automatic Reanalysis Failed");
//        return;
//    }
    repopulateAddIntervalValueComboBox();

    BeatAnalysis::analysePatterns();
    BeatAnalysis::consolidatePatterns();

    if (!intervalModel)
        initialiseIntervalsTable();
    if (!patternModel)
        initialisePatternsTable();
    refreshStrokeMeter();

    valueModel->recalculateCounts();

    ui->adjustButton->setEnabled(true);
    ui->optimiseButton->setEnabled(true);

    printConsoleBanner("Automatic Reanalysis Complete");
    ui->intervalsTable->repaint();
    ui->patternsTable->repaint();
}

void EditorWindow::on_analyseButton_clicked()
{
    printConsoleBanner("Starting Analysis");

    AnalysisOptionsDialog optDiag(this);
    if (optDiag.exec() == QDialog::Rejected)
    {
        printConsoleBanner("Analysis Canceled");
        return;
    }
    ui->centralStack->setCurrentWidget(ui->consolePage);

    runBeatAnalysis();
}

bool EditorWindow::initialiseBeatIntervals()
{
    outputToConsole(QString("%1 timestamps, ").arg(beatTimestamps.size()),true);

    if (beatTimestamps.size() <= 1)
        return false;

    beatIntervals.resize(beatTimestamps.size() - 1);
    int i = 0;
    for (i = 0; i < beatIntervals.size(); ++i)
    {
        beatIntervals[i] = BeatInterval(i);
        if (i)
        {
            beatIntervals[i].prev = &beatIntervals[i-1];
            beatIntervals[i-1].next = &beatIntervals[i];
        }
    }
    outputToConsole(QString("%1 intervals, ").arg(beatIntervals.size()));
    return true;
}

void EditorWindow::on_consoleButtons_clicked(QAbstractButton *)
{
    ui->centralStack->setCurrentWidget(ui->videoPage);
}

void EditorWindow::on_consoleButton_clicked()
{
    ui->centralStack->setCurrentWidget(ui->consolePage);
}

void EditorWindow::on_beatTimestampSpinbox_valueChanged(int newValue)
{
    if (multipleRowsSelected(ui->beatsTable->selectionModel()->selectedRows()))
    {
        if (newValue > 0)
            ui->beatTimestampSpinbox->setPrefix("+");
        else if (newValue == 0)
            ui->beatTimestampSpinbox->setPrefix("+/-");
        else //(newValue < 0)
            ui->beatTimestampSpinbox->setPrefix(""); //minus is prepended automatically
    }
}

void EditorWindow::on_newBeatButton_clicked()
{
    beatModel->addEvent(Event(ui->beatTimestampSpinbox->value(),
                              ui->beatTypeSpinbox->value(),
                              ui->beatValueSpinbox->value(),
                              ui->beatOptionalCheckbox->isChecked()));
    autoReanalyse();
}

//!
//! \brief EditorWindow::clearTimestampBasedModelSelections clears selections and
//! current items in models which directly relate to a timestamp (stroke markers, beat timestamps and beat values)
//! so that changes to those models don't cause the vedeo to seek
//!
void EditorWindow::clearTimestampBasedModelSelections()
{
    ui->beatsTable->clearSelection();
    ui->beatsTable->selectionModel()->clearCurrentIndex();
    ui->intervalsTable->clearSelection();
    ui->intervalsTable->selectionModel()->clearCurrentIndex();
    strokeMeterScene->clearSelection();
}

void EditorWindow::on_deleteBeatButton_clicked()
{
    if (multipleRowsSelected(ui->beatsTable->selectionModel()->selectedRows()))
    {
        //handle multiple deletions:
        QVector<int> indexesToDelete;
        int i = 0;
        for (i = 0; i < ui->beatsTable->selectionModel()->selectedRows().size(); ++i)
        {
            if (! indexesToDelete.contains(ui->beatsTable->selectionModel()->selectedRows()[i].row()))
                indexesToDelete.append(ui->beatsTable->selectionModel()->selectedRows()[i].row());
        }
        qSort(indexesToDelete);
        clearTimestampBasedModelSelections();
        for (i = indexesToDelete.size(); i > 0; --i) //work backwards to avoid shifting indices each time an item is deleted
            beatModel->removeBeat(indexesToDelete[i-1]);
    }
    else
    {
        int removalIndex = ui->beatsTable->selectionModel()->currentIndex().row();
        clearTimestampBasedModelSelections();
        beatModel->removeBeat(removalIndex);
    }
    autoReanalyse();
}

void EditorWindow::on_changeBeatButton_clicked()
{
    if (multipleRowsSelected(ui->beatsTable->selectionModel()->selectedRows()))
    {
        //change multiple events
        int i = 0;
        QVector<int> alteredIndexes;
        for (i = 0; i < ui->beatsTable->selectionModel()->selectedRows().size(); ++i)
        {
            int thisRow = ui->beatsTable->selectionModel()->selectedRows()[i].row();
            if (alteredIndexes.contains(thisRow))
                continue;
            Event workingEvent = beatTimestamps[thisRow].eventData;
            workingEvent.timestamp += ui->beatTimestampSpinbox->value();
            if (ui->beatTypeSpinbox->value() != ui->beatTypeSpinbox->minimum())
                workingEvent.setType(ui->beatTypeSpinbox->value());
            if (ui->beatValueSpinbox->value() != ui->beatValueSpinbox->minimum())
                workingEvent.value = ui->beatValueSpinbox->value();
            if (ui->beatOptionalCheckbox->checkState() != Qt::PartiallyChecked)
                workingEvent.setOptional(ui->beatOptionalCheckbox->isChecked());
            beatModel->changeEventAt(thisRow,workingEvent);
            alteredIndexes.append(thisRow);
        }
    }
    else
    {
        //just the one please, Mrs. Wimbley
        beatModel->changeEventAt(ui->beatsTable->selectionModel()->currentIndex().row(),
                                Event(ui->beatTimestampSpinbox->value(),
                                      ui->beatTypeSpinbox->value(),
                                      ui->beatValueSpinbox->value(),
                                      ui->beatOptionalCheckbox->isChecked()));
    }
    autoReanalyse();
}

void EditorWindow::on_optimiseButton_clicked()
{
    printConsoleBanner("Optimisation Started");

    if (intervalModel->containsUnknownIntervals())
    {
        int buttonClicked = QMessageBox::critical(this, tr("Unknown Intervals"),"There are currently unknown intervals loaded. If you optimise the timestamps now, it is VERY likely they will be out of time with the music. Click [Ignore] to continue.", QMessageBox::Ignore,QMessageBox::Cancel);
        if (buttonClicked == QMessageBox::Cancel)
        {
            printConsoleBanner("Optimisation Canceled");
            return;
        }
    }
    else if (intervalModel->containsPoorlyMatchedIntervals())
    {
        int buttonClicked = QMessageBox::warning(this, tr("Poorly-matched Intervals"),"There are currently poorly-matched intervals loaded. If these have been incorrectly recognised, the optimised timestamps might be out of time with the music. Click [Ignore] to continue.", QMessageBox::Ignore,QMessageBox::Cancel);
        if (buttonClicked == QMessageBox::Cancel)
        {
            printConsoleBanner("Optimisation Canceled");
            return;
        }
    }

    OptimisationOptionsDialog optDiag(this);
    if (optDiag.exec() == QDialog::Rejected)
    {
        printConsoleBanner("Optimisation Canceled");
        return;
    }

    createRollbackSnapshot();

    ui->centralStack->setCurrentWidget(ui->consolePage);

    if ( BeatOptimisation::Configuration::outputTempoProvided    &&
         BeatOptimisation::Configuration::startingTimestampProvided )
    {
        BeatOptimisation::Configuration::setImprovedValuesFromProvided();
        outputToConsole(QString("Using provided start timestamp (%1) and tempo (%2BPM).").arg(BeatOptimisation::Configuration::providedStartingTimestamp).arg(BeatOptimisation::Configuration::providedOutputTempo,0,'f',2),true);
    }
    else
        BeatOptimisation::optimiseBeats();

    if (BeatOptimisation::Configuration::roundToNearestBPM)
    {
        float bpm = (60 * 1000) / BeatOptimisation::improvedTempoInterval;
        if (BeatOptimisation::Configuration::roundToEvenBPM)
            bpm /= 2;
        int roundedBPM = roundToInt(bpm);
        if (BeatOptimisation::Configuration::roundToEvenBPM)
            roundedBPM *= 2;
        outputToConsole(QString("Rounded tempo to %1BPM. Reoptimising...").arg(roundedBPM),true);
        BeatOptimisation::Configuration::outputTempoProvided = true;
        BeatOptimisation::Configuration::providedOutputTempo = roundedBPM;
        BeatOptimisation::optimiseBeats();
        BeatOptimisation::Configuration::outputTempoProvided = false;
        BeatOptimisation::Configuration::providedOutputTempo = 0;
    }

    BeatOptimisation::applyBeats();
    createRollbackSnapshot();
    refreshStrokeMeter();

    printConsoleBanner("Optimisation Complete");
    ui->intervalsTable->repaint();
}

void EditorWindow::on_cancelButton_clicked()
{
    clearUndoStack();
    close();
}

void EditorWindow::on_applyButton_clicked()
{
    beatModel->writeChangesToOriginalModel();
    clearUndoStack();
    close();
}

void EditorWindow::on_addBeatValueButton_clicked()
{
    BeatValue newValue(1,1,QString("New Beat Value"));
    valueModel->addValue(newValue);
    repopulateAddIntervalValueComboBox();
    //TODO: Scroll to show added value
}

void EditorWindow::on_deleteBeatValueButton_clicked()
{
    int selectedRow = ui->beatValuesTable->selectionModel()->selectedRows().at(0).row();
    if (beatValues[selectedRow].preset == false)
    {
        valueModel->removeRow(selectedRow);
        repopulateAddIntervalValueComboBox();
    }
}

void EditorWindow::repopulateAddIntervalValueComboBox()
{
    ui->addIntervalValueComboBox->clear();
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if (beatValues[i].active)
            ui->addIntervalValueComboBox->addItem(QString::number(beatValues[i].getLength(),'f',0) + " (" + beatValues[i].name + ")");
    }
}

void EditorWindow::repopulateSplitIntervalValueComboBox()
{
    if (ui->intervalsTable->selectionModel() == NULL)
        return;

    disconnect(ui->splitIntervalComboBox_0,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(on_splitIntervalComboBox_0_currentIndexChanged(const QString &)));
    ui->splitIntervalComboBox_0->setEnabled(true);
    ui->splitIntervalButton->setEnabled(true);
    ui->splitIntervalComboBox_0->clear();
    ui->splitIntervalComboBox_0->addItem(QString(""));

    if ( ! containsNonContiguousRows(ui->intervalsTable->selectionModel()->selectedRows()) &&
         BeatAnalysis::Configuration::currentBPM != 0)
    {
        int firstRow, lastRow;
        identifyFirstAndLastRows(ui->intervalsTable->selectionModel()->selectedRows(),firstRow, lastRow);
        int totalMsSelected = beatTimestamps[lastRow + 1].eventData.timestamp - beatTimestamps[firstRow].eventData.timestamp;

        for (int i = 0; i < beatValues.size(); ++i)
        {
            if (beatValues[i].active &&
                    ( beatValues[i].getLength() < totalMsSelected ||
                      UniqueBeatInterval(totalMsSelected).matchesThisInterval(beatValues[i])
                      ))
                ui->splitIntervalComboBox_0->addItem(beatValues[i].name);
        }
        ui->splitIntervalComboBox_0->clearEditText();
        on_splitIntervalComboBox_0_currentIndexChanged(QString());
    }
    else
    {
        //disable stuff for non-contiguous rows (or if tempo unknown)
        ui->splitIntervalComboBox_0->setEnabled(false);
        ui->splitIntervalButton->setEnabled(false);
    }
    connect(ui->splitIntervalComboBox_0,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(on_splitIntervalComboBox_0_currentIndexChanged(const QString &)));
}

void EditorWindow::repopulateAdjustIntervalValueComboBox()
{
    ui->adjustIntervalButton->setEnabled(true);
    ui->adjustIntervalNewValueComboBox->setEnabled(true);
    ui->adjustIntervalNewValueComboBox->clear();
    if (ui->intervalsTable->selectionModel() == NULL ||
            multipleRowsSelected(ui->intervalsTable->selectionModel()->selectedRows()) ||
            BeatAnalysis::Configuration::currentBPM == 0)
    {
        ui->adjustIntervalButton->setEnabled(false);
        ui->adjustIntervalNewValueComboBox->setEnabled(false);
        return;
    }
    int currentRow = ui->intervalsTable->selectionModel()->currentIndex().row();
    if (currentRow < 0 ||
        currentRow >= intervalModel->rowCount())
        return;
    if (ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox->isChecked() ||
            intervalModel->rowCount() == 1)
    {
        if (ui->adjustIntervalSmartAdjustRadioButton->isChecked() ||
                ui->adjustIntervalMoveBothRadioButton->isChecked())
            ui->adjustIntervalMoveEndRadioButton->setChecked(true);
        ui->adjustIntervalSmartAdjustRadioButton->setEnabled(false);
        ui->adjustIntervalMoveBothRadioButton->setEnabled(false);
    }
    else if (currentRow == intervalModel->rowCount()-1 ||
             currentRow == 0)
    {
        if (ui->adjustIntervalSmartAdjustRadioButton->isChecked())
            ui->adjustIntervalMoveBothRadioButton->setChecked(true);
        ui->adjustIntervalSmartAdjustRadioButton->setEnabled(false);
        ui->adjustIntervalMoveBothRadioButton->setEnabled(true);
    }
    else
    {
        ui->adjustIntervalSmartAdjustRadioButton->setEnabled(true);
        ui->adjustIntervalMoveBothRadioButton->setEnabled(true);
    }

    int totalMsAvailable = 0;
    if (ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox->isChecked() ||
            (ui->adjustIntervalMoveEndRadioButton->isChecked() &&
             currentRow == intervalModel->rowCount()-1) ||
            (currentRow == 0 &&
             ui->adjustIntervalMoveBeginningRadioButton->isChecked()))
        totalMsAvailable = INT_MAX;
    else
    {
        if (ui->adjustIntervalMoveEndRadioButton->isChecked())
            totalMsAvailable = beatTimestamps[currentRow + 2].eventData.timestamp - beatTimestamps[currentRow].eventData.timestamp;
        else if (ui->adjustIntervalMoveBeginningRadioButton->isChecked())
            totalMsAvailable = beatTimestamps[currentRow + 1].eventData.timestamp - beatTimestamps[currentRow - 1].eventData.timestamp;
        else if (ui->adjustIntervalMoveBothRadioButton->isChecked())
        {
            if (currentRow == 0)
                totalMsAvailable = beatIntervals[currentRow].getLength() + (2 * beatIntervals[currentRow + 1].getLength());
            else if (currentRow == intervalModel->rowCount() - 1 ||
                beatIntervals[currentRow - 1].getLength() < beatIntervals[currentRow + 1].getLength())
                totalMsAvailable = beatIntervals[currentRow].getLength() + (2 * beatIntervals[currentRow - 1].getLength());
            else
                totalMsAvailable = beatIntervals[currentRow].getLength() + (2 * beatIntervals[currentRow + 1].getLength());
        }
        else //ui->adjustIntervalSmartAdjustRadioButton->isChecked()
            totalMsAvailable = beatTimestamps[currentRow + 2].eventData.timestamp - beatTimestamps[currentRow - 1].eventData.timestamp;
    }

    adjustValueValidator->setTop(totalMsAvailable - 1);

    int minDifference = INT_MAX;
    QString closestMatch = "";
    for (int i = 0; i < beatValues.size(); ++i)
    {
        if (beatValues[i].active &&
                beatValues[i].getLength() < totalMsAvailable)
        {
            QString entryString = QString::number(beatValues[i].getLength(),'f',0) + " (" + beatValues[i].name + ")";
            ui->adjustIntervalNewValueComboBox->addItem(entryString);
            int difference = 0.5 + (abs(beatValues[i].getLength() - beatIntervals[currentRow].getLength()));
            if (difference < minDifference)
            {
                minDifference = difference;
                closestMatch = entryString;
            }
        }
    }
    if (!closestMatch.isEmpty())
        ui->adjustIntervalNewValueComboBox->setEditText(closestMatch);
}

void EditorWindow::initialiseIntervalsTable()
{
    intervalModel = new IntervalDataModel(this);
    ui->intervalsTable->setModel(intervalModel);
    setColumWidthsFromModel(ui->intervalsTable);
    connect(ui->intervalsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentBeatIntervalChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui->intervalsTable->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),this,SLOT(on_selectedBeatIntervalsChanged()));
}

void EditorWindow::initialisePatternsTable()
{
    patternModel = new PatternDataModel(this);
    ui->patternsTable->setModel(patternModel);
    setColumWidthsFromModel(ui->patternsTable);
    connect(ui->patternsTable->selectionModel(),SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),this,SLOT(on_currentPatternChanged(const QModelIndex &, const QModelIndex &)));
}

void EditorWindow::on_addIntervalValueComboBox_currentTextChanged(const QString &arg1)
{
    QString fixedString = arg1;
    bool stringNeedsChanging = false;
    while (fixedString[0].isDigit() == false && fixedString.length() > 0)
    {
        fixedString.remove(0,1);
        stringNeedsChanging = true;
    }
    for (int i = 0; i < fixedString.length(); ++i)
    {
        if (fixedString[i].isDigit() == false)
        {
            fixedString.truncate(i);
            stringNeedsChanging = true;
            break;
        }
    }
    if (stringNeedsChanging)
        ui->addIntervalValueComboBox->setCurrentText(fixedString);
}

void EditorWindow::on_deleteIntervalButton_clicked()
{
    bool dealingWithMultipleRows = false;
    if (multipleRowsSelected(ui->intervalsTable->selectionModel()->selectedRows()))
    {
        if (containsNonContiguousRows(ui->intervalsTable->selectionModel()->selectedRows()))
            return;
        dealingWithMultipleRows = true;
    }
    int firstRow, lastRow;
    identifyFirstAndLastRows(ui->intervalsTable->selectionModel()->selectedRows(),firstRow, lastRow);

    //clear selections so that we don't seek the video as a result of new model row values
    clearTimestampBasedModelSelections();

    int totalTimeDeleted = 0;
    if ( ui->deleteIntervalMoveLaterIntervalsRadioButton->isChecked() ||
              ui->deleteIntervalMoveEarlierIntervalsRadioButton->isChecked() )
        totalTimeDeleted = beatTimestamps[lastRow + 1].eventData.timestamp - beatTimestamps[firstRow].eventData.timestamp;
    
    if (ui->deleteIntervalMergeWithFollowingRadioButton->isChecked() ||
            ui->deleteIntervalMergePreceedingWithFollowingRadioButton->isChecked() ||
            ui->deleteIntervalMoveLaterIntervalsRadioButton->isChecked())
    {
        beatModel->removeBeat(lastRow + 1);
    }
    if (dealingWithMultipleRows)
    {
        for (int i = 0; i < lastRow - firstRow; ++i)
            beatModel->removeBeat(firstRow + 1);
    }
    if (ui->deleteIntervalMergeWithPreceedingRadioButton->isChecked() ||
            ui->deleteIntervalMergePreceedingWithFollowingRadioButton->isChecked() ||
            ui->deleteIntervalMoveEarlierIntervalsRadioButton->isChecked())
    {
        beatModel->removeBeat(firstRow);
    }
    if (ui->deleteIntervalMoveLaterIntervalsRadioButton->isChecked())
    {
        for (int i = firstRow + 1; i < beatTimestamps.size(); ++i) {
            beatTimestamps[i].eventData.timestamp -= totalTimeDeleted;
        }
    }
    else if (ui->deleteIntervalMoveEarlierIntervalsRadioButton->isChecked())
    {
        for (int i = 0; i < firstRow; ++i) {
            beatTimestamps[i].eventData.timestamp += totalTimeDeleted;
        }
    }
    autoReanalyse();
}

void EditorWindow::on_addIntervalButton_clicked()
{
    if (multipleRowsSelected(ui->intervalsTable->selectionModel()->selectedRows()) || //multiple rows selected
            ui->intervalsTable->selectionModel()->selectedRows().isEmpty() || //no rows selected
            ui->addIntervalValueComboBox->currentText().isEmpty() || //value box blank
            ( ui->addIntervalMoveForwardRadioButton->isChecked() &&
              beatModel->data(beatModel->index(ui->intervalsTable->selectionModel()->selectedIndexes()[0].row(),0)).toInt() - ui->addIntervalValueComboBox->currentText().toInt() < 0 ) //adding would cause negative timestamp
              )
        return;

    int insertionIndex;
    if (ui->addIntervalBeforeRadioButton->isChecked())
        insertionIndex = ui->intervalsTable->selectionModel()->selectedIndexes().first().row();
    else if (ui->addIntervalAfterRadioButton->isChecked())
        insertionIndex = ui->intervalsTable->selectionModel()->selectedIndexes().first().row() + 1;
    else
        return; //somehow neither radio button is checked

    Event newEvent = beatModel->eventFromRow(insertionIndex);

    if (ui->addIntervalMoveForwardRadioButton->isChecked())
    {
        beatModel->addEvent(newEvent);
        for (int i = 0; i <= insertionIndex; ++i) {
            beatModel->changeTimestampAt(i, beatTimestamps[i].eventData.timestamp - ui->addIntervalValueComboBox->currentText().toInt());
        }
    }
    else if (ui->addIntervalMoveBackRadioButton->isChecked())
    {
        beatModel->addEvent(newEvent);
        for (int i = insertionIndex + 1; i < beatTimestamps.size(); ++i) {
            beatModel->changeTimestampAt(i, beatTimestamps[i].eventData.timestamp + ui->addIntervalValueComboBox->currentText().toInt());
        }
    }
    else
        return; //somehow neither radio button is checked

    autoReanalyse();
}

int EditorWindow::getContigousSelectionMilliseconds()
{
    if (containsNonContiguousRows(ui->intervalsTable->selectionModel()->selectedRows()))
        return -1;

    int firstRow, lastRow;
    identifyFirstAndLastRows(ui->intervalsTable->selectionModel()->selectedRows(),firstRow, lastRow);

    return beatTimestamps[lastRow + 1].eventData.timestamp - beatTimestamps[firstRow].eventData.timestamp;
}

float EditorWindow::getContigousSelectionBeats()
{
    if (containsNonContiguousRows(ui->intervalsTable->selectionModel()->selectedRows()))
        return -1;

    int firstRow, lastRow;
    identifyFirstAndLastRows(ui->intervalsTable->selectionModel()->selectedRows(),firstRow, lastRow);

    BeatPattern wholeSelection;
    for (int i = firstRow; i <= lastRow; ++i)
    {
        if (!beatIntervals[i].isKnownBeatValue()) {
            return -1;
        } else {
            wholeSelection.append(beatIntervals[i].getValue());
        }
    }
    return wholeSelection.lengthInBeats();
}

void EditorWindow::on_splitIntervalComboBox_0_currentIndexChanged(const QString & newText)
{
    if (stackOfComboBoxes.isEmpty())
        stackOfComboBoxes.push(ui->splitIntervalComboBox_0);

    int totalMsSelected = getContigousSelectionMilliseconds();
    float totalBeatsSelected = getContigousSelectionBeats();

    if (totalMsSelected == -1 /*non-contigous selection*/ || beatValues.isEmpty())
        //we have a problem...
        return;
    BeatValue * shortestActiveValue = calculateShortestActiveBeatValue();

    if (shortestActiveValue == NULL)
        //we have a problem...
        return;

    bool foundEmptyComboBox = false;
    bool deleteRest = false;
    float remainingMsToAllocate = totalMsSelected;
    float remainingBeatsToAllocate = totalBeatsSelected;

    int i = 0;
    for (i = 0; i < stackOfComboBoxes.size(); ++i) {
        BeatValue * value = getBeatValueByName(stackOfComboBoxes[i]->currentText());
        if (value == NULL)
        {
            //we've encountered an empty or invalid combobox before all of the selected time has been accounted for - leave user to fill in more
            //return;
            if (i) //not for the first ComboBox
                foundEmptyComboBox = true;
            deleteRest = true;
            break;
        }
        remainingMsToAllocate -= value->getLength();
        remainingBeatsToAllocate -= value->value();
        bool thisMatches = UniqueBeatInterval(remainingMsToAllocate).isKnownBeatValue();
        if (remainingMsToAllocate < shortestActiveValue->getLength() &&
                !thisMatches)
        {
            deleteRest = true;
            break;
        }
    }

    if (deleteRest)
    {
        //we used up all of the selected time, and don't need any more ComboBoxes
        //or we have a blank or invalid ComboBox and shouldn't have any beyond that
        while (stackOfComboBoxes.size() > i + 1)
        {
            QComboBox * thisBox = stackOfComboBoxes.pop();
            disconnect(thisBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(on_splitIntervalComboBox_0_currentIndexChanged(const QString &)));
            ui->splitIntervalsComboBoxesLayout->removeWidget(thisBox);
            delete thisBox;
        }
    }
    else
    {
        //we still have more time to assign to other values, so we'll need another box:
        QComboBox * thisBox = new QComboBox(this);
        QComboBox * previousTop = stackOfComboBoxes.top();
        ui->splitIntervalsComboBoxesLayout->addWidget(thisBox);
        stackOfComboBoxes.push(thisBox);
        thisBox->addItem(QString(""));
        for (int i = 0; i < beatValues.size(); ++i)
        {
            if (beatValues[i].active &&
                    (beatValues[i].getLength() < remainingMsToAllocate ||
                            UniqueBeatInterval(remainingMsToAllocate).matchesThisInterval(beatValues[i]) ||
                            beatValues[i].value() <= remainingBeatsToAllocate + 0.02 //margin for rounding errors
                     ))
                thisBox->addItem(beatValues[i].name);
        }
        if (thisBox->count() == 2 &&
                newText != QString(""))
            //auto-fill if only one possible option, and the user didn't deliberately set the value to blank
            thisBox->setCurrentIndex(1);
        else
        {
            UniqueBeatInterval remainingInterval(remainingMsToAllocate);
            bool intervalFound = false;
            for (int i = 0; i < beatValues.size(); ++i)
            {
                if (beatValues[i].name == previousTop->currentText() &&
                        remainingInterval.matchesThisInterval(beatValues[i]))
                    thisBox->setCurrentText(beatValues[i].name);
            }

            if (!intervalFound)
                thisBox->clearEditText();
        }


        connect(thisBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(on_splitIntervalComboBox_0_currentIndexChanged(const QString &)));
    }
    if (foundEmptyComboBox)
    {
        //delete the empty box and recreate it (by calling this function again) to ensure it contains correct values
        QComboBox * emptyBox = stackOfComboBoxes.pop();
        disconnect(emptyBox,SIGNAL(currentIndexChanged(const QString &)),this,SLOT(on_splitIntervalComboBox_0_currentIndexChanged(const QString &)));
        ui->splitIntervalsComboBoxesLayout->removeWidget(emptyBox);
        emptyBox->deleteLater();
        on_splitIntervalComboBox_0_currentIndexChanged(newText);
    }

    //if we only have one empty combo box, try to fill it with the complete value of the selected intervals (if that's a known value)
    if (stackOfComboBoxes.size() == 1 &&
            ui->splitIntervalComboBox_0->currentText() == QString(""))
    {
        UniqueBeatInterval totalInterval(totalMsSelected);
        if (totalInterval.isKnownBeatValue())
            ui->splitIntervalComboBox_0->setCurrentText(totalInterval.getValue()->name);
    }
}

BeatPattern EditorWindow::createSplitPatternFromComboBoxes()
{
    BeatPattern enteredPattern;
    for (int i = 0; i < stackOfComboBoxes.size(); ++i)
    {
        qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack in loop." << endl;
        BeatValue * valueToAppend = getBeatValueByName(stackOfComboBoxes[i]->currentText());
        if (valueToAppend != NULL)
        {
            enteredPattern.append(valueToAppend);
            qDebug() << "Added " << stackOfComboBoxes[i]->currentText() << " to the pattern." << endl;
        }
        else
        {
            if (i != stackOfComboBoxes.size() - 1)
            {
                qDebug() << "Could not add " << stackOfComboBoxes[i]->currentText() << " to the pattern." << endl;
            }
            break;
        }
    }
    return enteredPattern;

}

void EditorWindow::on_splitIntervalButton_clicked()
{
    if (containsNonContiguousRows(ui->intervalsTable->selectionModel()->selectedRows()))
        return;

    bool stretchPattern = false;
    bool loopPattern = false;

    const QComboBox * const topBox = stackOfComboBoxes.top();
    bool topIsNull = (topBox == NULL);
    /*bool topHasNullText = false;
    if (!topIsNull)
    {
        topHasNullText = (topBox->currentText() == NULL);
    }*/

    if (topIsNull/* || topHasNullText*/)
    {
        //something's gone wrong, abort to prevent crash
        return;
    }

    BeatPattern givenPattern = createSplitPatternFromComboBoxes();

    //check if selected intervals are long enough to fill selected time
    qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack." << endl;
    if (stackOfComboBoxes.top()->currentText() == NULL || stackOfComboBoxes.top()->currentText() == QString(""))
    {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Length mismatch");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText("The selected intervals in the table are longer than the pattern you've entered.");
        msgBox.setInformativeText("What would you like to do?\nSelect 'Stretch' to stretch the entered pattern so that it repeats once over the length of the selected intervals.\nSelect 'Loop' to repeat the entered pattern as many times as needed to fill the selected intervals.");
        QPushButton * stretchButton = msgBox.addButton("Stretch",QMessageBox::AcceptRole);
        QPushButton * loopButton = msgBox.addButton("Loop",QMessageBox::DestructiveRole);
        msgBox.addButton("Cancel",QMessageBox::RejectRole);
        msgBox.exec();
        if (msgBox.clickedButton() == stretchButton)
            stretchPattern = true;
        else if (msgBox.clickedButton() == loopButton)
            loopPattern = true;
        else
            return;
    }
    else
        //pattern is aproximately the correct length.
        if (QMessageBox::question(this,"Fit entered pattern to selected timestamps?","Select Yes to fit the entered pattern to the precise length of the selected intervals.\nSelect No to offset all intervals from the beginning of the selection using the established tempo"))
            stretchPattern = true;

    splitIntervals(givenPattern, loopPattern, stretchPattern);
}

void EditorWindow::splitIntervals(BeatPattern & givenPattern, bool loopPattern, bool stretchPattern)
{
    //remove intervening timestamps (between beginning and end of selection)
    int firstRow, lastRow;
//    qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack before identifying rows." << endl;
    identifyFirstAndLastRows(ui->intervalsTable->selectionModel()->selectedRows(),firstRow, lastRow);
//    qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack after identifying rows." << endl;
//    qDebug() << "first and last rows: " << firstRow << " and " << lastRow << endl;
    Event eventToCopy = beatTimestamps[firstRow].eventData;
    int beginningTimestamp = beatTimestamps[firstRow].eventData.timestamp;
    int endingTimestamp = beatTimestamps[lastRow + 1].eventData.timestamp;
//    qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack before removing timestamps." << endl;
//    qDebug() << "removing timestamps between: " << beginningTimestamp << " and " << endingTimestamp <<
//                " total length: " << endingTimestamp - beginningTimestamp << endl;
    for (int i = beatTimestamps.size() - 1; i >= 0; --i)
    {
        if (beatTimestamps[i].eventData.timestamp > beginningTimestamp &&
            beatTimestamps[i].eventData.timestamp < endingTimestamp)
            beatModel->removeBeat(i);
    }
//    qDebug() << stackOfComboBoxes.size() << " items detected in the combo box stack after removing timestamps." << endl;

    //insert timestamps according to user selection
    float currentTimestamp = beginningTimestamp;
    int i = 0;
    int addedTimestampsCount = 0;
    BeatPattern insertedPattern; //contains the actual values added, which may include repetitions, and
                                 //will likely be missing a beat at the end if the pattern length was well-matched to the selected timespan
    BeatValue * shortestActiveBeatValue = calculateShortestActiveBeatValue();
    if (shortestActiveBeatValue == NULL)
        //we have a problem...
        return;

    qDebug() << "We have " << stackOfComboBoxes.size() << " intervals in the pattern." << endl;
    while (i < givenPattern.size())
    {
        qDebug() << "length of " << givenPattern[i]->name << " at " << BeatAnalysis::Configuration::currentBPM << "BPM is " << givenPattern[i]->getLength() << endl;
        currentTimestamp += givenPattern[i]->getLength();
        qDebug() << "Advancing to " << currentTimestamp << endl;
        if (currentTimestamp > endingTimestamp)
        {
            qDebug() << "Not creating timestamp here because we're past the end\n";
            break;
        }

        //check if we're too close to the end of the selection to bother creating a timestamp
        float remainingTimeSelected = endingTimestamp - currentTimestamp;
        if (remainingTimeSelected < shortestActiveBeatValue->getLength() &&
                ! UniqueBeatInterval(remainingTimeSelected).matchesThisInterval(*shortestActiveBeatValue))
        {
            qDebug() << "Not creating timestamp here because we're near the end\n";
            break;
        }
        beatModel->addEvent(Event(roundToInt(currentTimestamp),eventToCopy.type(),eventToCopy.value,eventToCopy.isOptional()));
        insertedPattern.append(givenPattern[i]);

        ++i;
        ++addedTimestampsCount;

        if (loopPattern)
            i %= givenPattern.size();
    }

    autoReanalyse();
    QCoreApplication::processEvents();

    if (stretchPattern)
    {
        //check for unneeded last timestamp (which would land on top of the end of the selection if adjusted)
        if (addedTimestampsCount % givenPattern.size() == 0) //we entered a pattern (or multiple thereof), complete with ending timestamp
        {
            int newLastRow = firstRow + (addedTimestampsCount - 1);
            qDebug() << "Removing beat at row " << newLastRow + 1 << ", difference between supposedly identical beats: " <<
                        beatIntervals[newLastRow].getIntLength() << "\n";
            beatModel->removeBeat(newLastRow + 1); // + 1 because this timestamp would have ended up at the END of the interval on row lastRow
            --addedTimestampsCount;
        }
        autoReanalyse();
        QCoreApplication::processEvents();
        float totalDifference = endingTimestamp - currentTimestamp;
        qDebug() << "Total difference to be accounted for: " << totalDifference << endl;
        float beatsProcessedSoFar = 0;
        for (int j = 0; j < addedTimestampsCount; ++j)
        {
            beatsProcessedSoFar += insertedPattern[j]->value();
            float differencePerBeat = totalDifference / insertedPattern.lengthInBeats();
            int differenceAddedForThisBeat = differencePerBeat * beatsProcessedSoFar;
            qDebug() << "Adjusting: " << differenceAddedForThisBeat << endl;
            // + 1 because we're dealing with the END of intervals - the first interval we deal with is the one at the END of the interval on row firstRow
            beatModel->changeTimestampAt(firstRow + 1 + j, beatModel->eventFromRow(firstRow + 1 + j).timestamp + differenceAddedForThisBeat);
            autoReanalyse();
            QCoreApplication::processEvents();
        }
    }

    autoReanalyse();
}

void EditorWindow::on_adjustIntervalNewValueComboBox_currentIndexChanged(const QString &arg1)
{
    QString fixedString = arg1;
    bool stringNeedsChanging = false;
    while (fixedString[0].isDigit() == false && fixedString.length() > 0)
    {
        fixedString.remove(0,1);
        stringNeedsChanging = true;
    }
    for (int i = 0; i < fixedString.length(); ++i)
    {
        if (fixedString[i].isDigit() == false)
        {
            fixedString.truncate(i);
            stringNeedsChanging = true;
            break;
        }
    }
    if (stringNeedsChanging)
        ui->adjustIntervalNewValueComboBox->setCurrentText(fixedString);
}

void EditorWindow::on_adjustIntervalButton_clicked()
{
    QString comboBoxInput = ui->adjustIntervalNewValueComboBox->currentText();
    int spaceInText = comboBoxInput.indexOf(" ");
    if (spaceInText >= 0)
        comboBoxInput.truncate(spaceInText);
    int adjustedIntervalValue = comboBoxInput.toInt();
    if (adjustedIntervalValue <= 0)
    {
        qDebug() << "Could not convert Integer " << ui->adjustIntervalNewValueComboBox->currentText() << " : " << comboBoxInput;
        return;
    }
    int currentRow = ui->intervalsTable->selectionModel()->currentIndex().row();
    if (ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox->isChecked())
    {
        if (ui->adjustIntervalMoveEndRadioButton->isChecked())
        {
            int beginningTimestamp = beatIntervals[currentRow].endsAtTimestamp();
            int endTimestamp = beatIntervals[currentRow].startsAtTimestamp() + adjustedIntervalValue;
            for (int i = beatTimestamps.size() - 1; i >= 0; --i)
                if (beatTimestamps[i].eventData.timestamp > beginningTimestamp &&
                    beatTimestamps[i].eventData.timestamp <= endTimestamp)
                    beatModel->removeBeat(i);
        }
        else if (ui->adjustIntervalMoveBeginningRadioButton->isChecked())
        {
            int beginningTimestamp = beatIntervals[currentRow].endsAtTimestamp() - adjustedIntervalValue;
            if (beginningTimestamp < 0)
                beginningTimestamp = 0;
            int endTimestamp = beatIntervals[currentRow].startsAtTimestamp();
            for (int i = beatTimestamps.size() - 1; i >= 0; --i)
            {
                if (beatTimestamps[i].eventData.timestamp >= beginningTimestamp &&
                    beatTimestamps[i].eventData.timestamp < endTimestamp)
                {
                    beatModel->removeBeat(i);
                    --currentRow;
                }
            }
        }
    }

    if (ui->adjustIntervalMoveEndRadioButton->isChecked())
    {
        int indexToChange = currentRow + 1;
        BeatInterval currentInterval = beatIntervals[currentRow];
        int currentTimestamp = currentInterval.startsAtTimestamp();
        int modifiedTimestamp = currentTimestamp + adjustedIntervalValue;
        beatModel->changeTimestampAt(indexToChange, modifiedTimestamp);
    }
    else if (ui->adjustIntervalMoveBeginningRadioButton->isChecked())
    {
        int newTimestamp = beatIntervals[currentRow].endsAtTimestamp() - adjustedIntervalValue;
        if (newTimestamp < 0)
            newTimestamp = 0;
        beatModel->changeTimestampAt(currentRow, newTimestamp);
    }
    else if (ui->adjustIntervalMoveBothRadioButton->isChecked())
    {
        int totalAmountOfTimeToAdd = adjustedIntervalValue - beatIntervals[currentRow].getIntLength();
        int amountOfTimeToAddToBeginning = totalAmountOfTimeToAdd / 2; //this may round down due to int division
        int amountOfTimeToAddToEnd = totalAmountOfTimeToAdd - amountOfTimeToAddToBeginning;
        int newBeginningTimestamp = beatIntervals[currentRow].startsAtTimestamp() - amountOfTimeToAddToBeginning;
        int newEndingTimestamp = beatIntervals[currentRow].endsAtTimestamp() + amountOfTimeToAddToEnd;
        if (newBeginningTimestamp < 0)
        {
            newEndingTimestamp += (0 - newBeginningTimestamp);
            newBeginningTimestamp = 0;
        }
        if (currentRow < intervalModel->rowCount() - 1 &&
                newEndingTimestamp >= beatIntervals[currentRow + 1].endsAtTimestamp())
            newEndingTimestamp = beatIntervals[currentRow + 1].endsAtTimestamp() - 1;

        beatModel->changeTimestampAt(currentRow, newBeginningTimestamp);
        beatModel->changeTimestampAt(currentRow + 1, newEndingTimestamp);
    }
    else //ui->adjustIntervalSmartAdjustRadioButton->isChecked()
    {
        bool matchFound = false;
        float lowestError = FLT_MAX;
        int bestOffset = 1;
        float previousCentreOfInterval = beatIntervals[currentRow].startsAtTimestamp() + ((float) beatIntervals[currentRow].getIntLength() / 2);
        for (int i = 1; i + adjustedIntervalValue < beatIntervals[currentRow + 1].endsAtTimestamp(); ++i)
        {
            UniqueBeatInterval intervalBefore(i);
            UniqueBeatInterval intervalAfter(beatIntervals[currentRow + 1].endsAtTimestamp() - (i + beatIntervals[currentRow - 1].startsAtTimestamp()));
            if (intervalBefore.isKnownBeatValue() == false ||
                    intervalAfter.isKnownBeatValue() == false ||
                    intervalBefore.getValue()->active == false ||
                    intervalAfter.getValue()->active == false /*||
                    intervalBefore.getValue() != beatIntervals[currentRow - 1].getValue() ||
                    intervalAfter.getValue() != beatIntervals[currentRow + 1].getValue()*/
                    )
                continue;

            matchFound = true;
            float thisError = std::max(intervalBefore.deviationFromNearestKnownBeatValue(), intervalAfter.deviationFromNearestKnownBeatValue());
            if (thisError != FLT_MAX)
            {
                float midPoint = beatIntervals[currentRow - 1].startsAtTimestamp() + i + ((float)adjustedIntervalValue / 2);
                thisError *= 2;
                float shiftDistance = fabs(previousCentreOfInterval - midPoint);
                thisError += 0.01 * shiftDistance * shiftDistance;
            }
            if (thisError < lowestError)
            {
                lowestError = thisError;
                bestOffset = i;
            }
        }

        if ( ! matchFound)
            return;
        else
        {
            beatModel->changeTimestampAt(currentRow,beatIntervals[currentRow - 1].startsAtTimestamp() + bestOffset);
            beatModel->changeTimestampAt(currentRow + 1,beatIntervals[currentRow - 1].startsAtTimestamp() + bestOffset + adjustedIntervalValue);
        }
    }
    autoReanalyse();
}

void EditorWindow::scrollStrokeMeterToTimestamp()
{
    ui->strokeMeter->centerOn(currentTimecode() / STROKE_METER_SCALE_DIVISOR,0);
}

void EditorWindow::refreshStrokeMeter()
{
    while (strokeMarkers.size() != beatModel->rowCount())
    {
        if (strokeMarkers.size() > beatModel->rowCount())
        {
            QGraphicsEllipseItem * removeMe = strokeMarkers.last();
            strokeMeterScene->removeItem(removeMe);
            strokeMarkers.removeLast();
            delete removeMe;
        }
        else
        {
            QGraphicsEllipseItem * ellie = new QGraphicsEllipseItem(QRectF(0,0,STROKE_MARKER_DIAMETER,STROKE_MARKER_DIAMETER));
            QPen pen;
            pen.setWidth(2);
            ellie->setPen(pen);
            QBrush brush{QColor(Qt::lightGray)};
            ellie->setBrush(brush);
            ellie->setFlag(QGraphicsItem::ItemIsSelectable);
            strokeMarkers.append(ellie);
            strokeMeterScene->addItem(ellie);
        }
    }

    for (int i = 0; i < strokeMarkers.size(); ++i)
    {
        strokeMarkers.at(i)->setPos(beatTimestamps[i].eventData.timestamp / STROKE_METER_SCALE_DIVISOR ,0);
    }
    if (intervalModel && intervalModel->rowCount() > 0 /*== strokeMarkers.size() - 1*/)
    {
        for (int i = 0; i < strokeMarkers.size(); ++i)
        {
            if (i == 0)
            {
                QModelIndex index = intervalModel->index(i,0);
                QVariant variant = intervalModel->data(index,Qt::BackgroundRole);
                QBrush brush = qvariant_cast<QBrush>(variant);
                strokeMarkers[i]->setBrush(brush);
            }
            else if (i == strokeMarkers.size() - 1)
            {
                QModelIndex index = intervalModel->index(i-1,0);
                QVariant variant = intervalModel->data(index,Qt::BackgroundRole);
                QBrush brush = qvariant_cast<QBrush>(variant);
                strokeMarkers[i]->setBrush(brush);
            }
            else
            {
                QModelIndex indexBefore = intervalModel->index(i - 1 ,2);
                QModelIndex indexAfter = intervalModel->index(i ,2);
                int accuracyBefore = indexBefore.data().toInt();
                int accuracyAfter = indexAfter.data().toInt();
                QVariant variant;
                if (accuracyBefore < accuracyAfter)
                    variant = intervalModel->data(indexBefore,Qt::BackgroundRole);
                else
                    variant = intervalModel->data(indexAfter,Qt::BackgroundRole);
                QBrush brush = qvariant_cast<QBrush>(variant);
                strokeMarkers[i]->setBrush(brush);
            }
        }
        for (int i = 0; i < strokeMarkers.size(); ++i)
        {
             strokeMarkers.at(i)->update();
        }
    }

    QRectF bounds = strokeMeterScene->itemsBoundingRect();
    bounds.setLeft(bounds.left() - ui->strokeMeter->size().width());
    bounds.setRight(bounds.right() + ui->strokeMeter->size().width());
    strokeMeterScene->setSceneRect(bounds);
    strokeMeterScene->update();
}

void EditorWindow::on_selectedStrokeMarkersChanged()
{
    if (!controllingWidget)
        controllingWidget = ui->strokeMeter;
    bool thisWasClicked = (controllingWidget == ui->strokeMeter);

    if ( thisWasClicked && ! currentlyPlaying)
    {
        QList<QGraphicsItem *> selectedItems = strokeMeterScene->selectedItems();
        if (selectedItems.size() == 0)
            return;
        QGraphicsEllipseItem * selectedItem = (QGraphicsEllipseItem *)(selectedItems[0]);
        int selectedIndex = -1;
        for (int i = 0; i < strokeMarkers.size(); ++i)
        {
            if (selectedItem == strokeMarkers.at(i))
                selectedIndex = i;
        }
        if (selectedIndex == -1)
            return;

        QModelIndex beatIndex = beatModel->index(selectedIndex,0);
        ui->beatsTable->scrollTo(beatIndex,QAbstractItemView::PositionAtCenter);
        ui->beatsTable->selectionModel()->select(beatIndex,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current | QItemSelectionModel::Rows);
        ui->beatsTable->selectionModel()->setCurrentIndex(beatIndex,QItemSelectionModel::Current);
        seekToTimestamp(beatIndex.data().toInt());
        scrollStrokeMeterToTimestamp();
        if (intervalModel)
        {
            int modifiedIndex = selectedIndex;
            while (modifiedIndex >= intervalModel->rowCount()) //last marker is off end of table
                --modifiedIndex;
            QModelIndex intervalIndex = intervalModel->index(modifiedIndex,0);
            ui->intervalsTable->scrollTo(intervalIndex,QAbstractItemView::PositionAtCenter);
            ui->intervalsTable->selectionModel()->select(intervalIndex,QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current | QItemSelectionModel::Rows);
            ui->intervalsTable->selectionModel()->setCurrentIndex(intervalIndex,QItemSelectionModel::Current);
        }
        if (ui->beatsAndIntervalsTabs->currentWidget() == ui->intervalsTab && selectedIndex >= intervalModel->rowCount())
        {
            ui->beatsAndIntervalsTabs->setCurrentWidget(ui->beatsTab);
            beatsTabWasAutoSelected = true;
        }
        else if (beatsTabWasAutoSelected && ui->beatsAndIntervalsTabs->currentWidget() == ui->beatsTab && selectedIndex < intervalModel->rowCount())
        {
            ui->beatsAndIntervalsTabs->setCurrentWidget(ui->intervalsTab);
            beatsTabWasAutoSelected = false;
        }
    }

    if (thisWasClicked)
        controllingWidget = NULL;
}

void EditorWindow::on_actionAdd_triggered()
{
    AddDialog * ad = new AddDialog(this);
    ad->exec();
}

void EditorWindow::on_actionDelete_triggered()
{
    DeleteDialog * dd = new DeleteDialog(this);
    dd->exec();
}

void EditorWindow::on_actionDelete_timestamp_only_triggered()
{
    if (ui->beatsAndIntervalsTabs->currentWidget() == ui->beatsTab)
        on_deleteBeatButton_clicked();
    else
    {
        QRadioButton * selectedButton = nullptr;
        if (ui->deleteIntervalMergeWithFollowingRadioButton->isChecked())
            selectedButton = ui->deleteIntervalMergeWithFollowingRadioButton;
        if (ui->deleteIntervalMergeWithPreceedingRadioButton->isChecked())
            selectedButton = ui->deleteIntervalMergeWithPreceedingRadioButton;
        if (ui->deleteIntervalMergePreceedingWithFollowingRadioButton->isChecked())
            selectedButton = ui->deleteIntervalMergePreceedingWithFollowingRadioButton;
        if (ui->deleteIntervalMoveLaterIntervalsRadioButton->isChecked())
            selectedButton = ui->deleteIntervalMoveLaterIntervalsRadioButton;
        if (ui->deleteIntervalMoveEarlierIntervalsRadioButton->isChecked())
            selectedButton = ui->deleteIntervalMoveEarlierIntervalsRadioButton;

        ui->deleteIntervalMergeWithPreceedingRadioButton->setChecked(true);
        on_deleteIntervalButton_clicked();

        if (selectedButton != nullptr)
            selectedButton->setChecked(true);
    }
}

void EditorWindow::on_actionAdjust_triggered()
{
    AdjustDialog * ad = new AdjustDialog(this);
    ad->exec();
}

void EditorWindow::on_adjustButton_clicked()
{
    for (int j = 0; j < beatIntervals.length(); ++j) {
        BeatInterval interval = beatIntervals[j];
        if (!interval.isKnownBeatValue()) {
            ui->intervalsTable->selectRow(j);
            return;
        }
    }
    //all intervals are matched. Select the least well-matched
    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < beatIntervals.length(); ++j) {
            BeatInterval interval = beatIntervals[j];
            if ((100 - interval.deviationFromNearestKnownBeatValue()) <= i) {
                ui->intervalsTable->selectRow(j);
                return;
            }
        }
    }
}

void EditorWindow::on_actionSplit_triggered()
{
    SplitDialog * sd = new SplitDialog(this);
    sd->exec();
}

void EditorWindow::on_actionMost_Suspicious_Interval_triggered()
{
    on_adjustButton_clicked();
}

//!
//! \brief EditorWindow::createRollbackSnapshot creates a snapshot which can then be restored at a later point, effectively acting as an 'undo' action.
//!
void EditorWindow::createRollbackSnapshot()
{
    //the only things we need to save are the beatTimestamps and queued deletions:
    //beatIntervals are a thin layer that describes the gaps between the beatTimestamps
    //beatValues can be calculated from the beatIntervals
    //the beatTimestamps have their own copy of the event data, which contains everything we need
    //the `Event`s themselves have metadata, which is basically a way of persisting information the
    //editor calculates for itself, after the editor is closed, so this is not needed by the editor itself.
    EditorWin::RollbackSnapshot snapshot(beatTimestamps, beatModel->indexesToDelete);
    bool thisIsFirstSnapshot = undoStack.isEmpty();
    undoStack.push(snapshot);
    if (!thisIsFirstSnapshot)
        undoStackBookmark++;
}

void EditorWindow::applySnapshot(EditorWin::RollbackSnapshot snapshot)
{
    //based on setBeatTimestamps, but we don't necessarily want to do all of the work that function does
    clear();
    beatModel->addEventsFromSnapshot(snapshot);
    ui->analyseButton->setEnabled(true);
    valueModel = new ValueDataModel(this);
    ui->beatValuesTable->setModel(valueModel);
    setColumWidthsFromModel(ui->beatValuesTable);

    refreshStrokeMeter();

//    ui->strokeMeter->centerOn(strokeMarkers.at(0));

//    on_currentBeatTimestampChanged(ui->beatsTable->model()->index(0,0),QModelIndex());
    autoReanalyse();
}

void EditorWindow::on_actionUndo_triggered()
{
    if (undoStack.isEmpty())
        return;

    if (undoStackBookmark < 1)
        //already at oldest change
        return;

    --undoStackBookmark;
    applySnapshot(undoStack[undoStackBookmark]);
}

void EditorWindow::on_actionRedo_triggered()
{
    if (undoStack.size() < 2)
        return;

    if (undoStackBookmark >= undoStack.size() - 1)
        //already at newest change
        return;

    ++undoStackBookmark;
    applySnapshot(undoStack[undoStackBookmark]);
}

void EditorWindow::clearUndoStack()
{
    for (auto snapshot : undoStack)
    {
        snapshot.timestamps.clear();
    }
    undoStack.clear();
    undoStackBookmark = 0;
}
