#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>
#include "eventdataproxymodel.h"
#include "valuetablekeyboardeventhandler.h"
#include <QStack>
#include "beattimestamp.h"

class BeatDataModel;
class IntervalDataModel;
class PatternDataModel;
class ValueDataModel;
class QAbstractButton;
class QTableView;
class QComboBox;
class QIntValidator;
class QGraphicsScene;
class QAbstractGraphicsShapeItem;
class QGraphicsEllipseItem;
class EditorGridLine;
class BeatPattern;

namespace Ui {
class EditorWindow;
}

namespace EditorWin
{
struct RollbackSnapshot
{
    RollbackSnapshot(){}; //needed because Qt
    RollbackSnapshot(QVector<BeatTimestamp> t, QVector<int> d) : timestamps(t), deletions(d) {timestamps.detach();};
    QVector<BeatTimestamp> timestamps;
    QVector<int> deletions;
};
}

class EditorWindow : public QMainWindow
{
    Q_OBJECT

    friend class ValueTableKeyboardEventHandler;
    friend class DeleteDialog;
    friend class AddDialog;
    friend class AdjustDialog;
    friend class SplitDialog;

public:
    explicit EditorWindow(QWidget *parent = 0);
    ~EditorWindow();
    void mousePressEvent (QMouseEvent *);
    void closeEvent(QCloseEvent *);
    void takeVideoOutput();
    void clear();
    void setBeatTimestamps (EventDataProxyModel * model, const QModelIndexList & eventIndexes);
    bool initialiseBeatIntervals();
    void outputToConsole(QString text, bool onNewLine = false);
    void printConsoleBanner(QString text);
    void prepareForReanalysis();
    void runBeatAnalysis();
    void autoReanalyse();
    static void getActionsList();
    int getContigousSelectionMilliseconds();
    float getContigousSelectionBeats();
    BeatPattern createSplitPatternFromComboBoxes();
    void splitIntervals(BeatPattern & givenPattern, bool loopPattern, bool stretchPattern);
    void createRollbackSnapshot();

private slots:
    void on_analyseButton_clicked();

    void on_consoleButtons_clicked(QAbstractButton *);

    void on_currentBeatTimestampChanged(const QModelIndex &, const QModelIndex &);
    void on_selectedBeatTimestampsChanged();

    void on_currentBeatIntervalChanged(const QModelIndex &, const QModelIndex &);
    void on_selectedBeatIntervalsChanged();

    void on_currentPatternChanged(const QModelIndex &, const QModelIndex &);

    void on_cancelButton_clicked();

    void on_beatTimestampSpinbox_valueChanged(int newValue);

    void on_newBeatButton_clicked();

    void on_deleteBeatButton_clicked();

    void on_changeBeatButton_clicked();

    void on_optimiseButton_clicked();

    void on_consoleButton_clicked();

    void on_applyButton_clicked();

    void on_addBeatValueButton_clicked();

    void on_deleteBeatValueButton_clicked();

    void on_addIntervalValueComboBox_currentTextChanged(const QString &arg1);

    void on_deleteIntervalButton_clicked();
    
    void on_addIntervalButton_clicked();

    void on_splitIntervalComboBox_0_currentIndexChanged(const QString &newText);

    void on_splitIntervalButton_clicked();

    void repopulateAddIntervalValueComboBox();
    void repopulateAdjustIntervalValueComboBox();
    void repopulateSplitIntervalValueComboBox();
    void initialiseIntervalsTable();
    void initialisePatternsTable();

    void on_adjustIntervalNewValueComboBox_currentIndexChanged(const QString &arg1);

    void on_adjustIntervalButton_clicked();

    void centerItemOnTimestamp(QAbstractGraphicsShapeItem *item, long timestamp);
    void refreshStrokeMeter();
    void updateNowMarkers();
    void handleTimerTick();
    void scrollStrokeMeterToTimestamp(long timestamp = currentTimecode());
    void on_selectedStrokeMarkersChanged();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

    void on_actionDelete_timestamp_only_triggered();

    void on_actionAdjust_triggered();

    void on_adjustButton_clicked();

    void on_actionSplit_triggered();

    void on_actionMost_Suspicious_Interval_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionPlay_Pause_triggered();

    void on_actionSkip_Forward_triggered();

    void on_actionSkip_Backward_triggered();

    void on_actionSkip_to_First_Event_triggered();

    void on_actionSkip_to_Last_Event_triggered();

    void on_actionIdentify_Enable_interval_triggered();

private:
    qreal getXCoordinateForTimestamp(long timestamp);
    Ui::EditorWindow *ui;
    ValueTableKeyboardEventHandler * valueTableKeyboardHandler;
    QWidget * controllingWidget;
    QStack<QComboBox *> stackOfComboBoxes;
    QIntValidator * adjustValueValidator;
    QGraphicsScene * strokeMeterScene;
    QAbstractGraphicsShapeItem * createStrokeMarker();
    QAbstractGraphicsShapeItem * createNowMarker();
    QAbstractGraphicsShapeItem * createNowAnchor();
    QVector<QAbstractGraphicsShapeItem *> strokeMarkers;
    QAbstractGraphicsShapeItem * nowMarker;
    QAbstractGraphicsShapeItem * nowAnchor;
    QVector<EditorGridLine *> gridLines;
    qreal convertPixelsToBeats(int pixels);
    void clearGridLines();
    bool alreadyHaveGridLineForValue(qreal value);
    void ensureGridLineCount();
    void addGridlinesAtIntervals(qreal divisor, qreal opacity);
    void clearTimestampBasedModelSelections();
    QStack<EditorWin::RollbackSnapshot> undoStack;
    int undoStackBookmark;
    void applySnapshot(EditorWin::RollbackSnapshot);
    void clearUndoStack();
    bool beatsTabWasAutoSelected;

public:
    friend class BeatDataModel; //for creating and restoring snapshots
    BeatDataModel * beatModel;
    IntervalDataModel * intervalModel;
    PatternDataModel * patternModel;
    ValueDataModel * valueModel;
};

#endif // EDITORWINDOW_H
