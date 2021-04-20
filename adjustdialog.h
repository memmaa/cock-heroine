#ifndef ADJUSTDIALOG_H
#define ADJUSTDIALOG_H

#define SHORTCUT_ADJUST_MOVE_BEGINNING "SHORTCUT_ADJUST_MOVE_BEGINNING"
#define SHORTCUT_ADJUST_MOVE_END "SHORTCUT_ADJUST_MOVE_END"
#define SHORTCUT_ADJUST_MOVE_BOTH "SHORTCUT_ADJUST_MOVE_BOTH"
#define SHORTCUT_ADJUST_SMART_ADJUST "SHORTCUT_ADJUST_SMART_ADJUST"
#define SHORTCUT_ADJUST_OVERWRITE_OTHERS "SHORTCUT_ADJUST_OVERWIRTE_OTHERS"

#include <QDialog>
class QShortcut;
class BeatValue;

namespace Ui {
class AdjustDialog;
}

class AdjustDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdjustDialog(QWidget *parent = nullptr);
    ~AdjustDialog();

    virtual void accept() override;

private slots:
    void newValueSelected(BeatValue * newValue);

    void ensureSingleSelection();

    void on_moveBeginningRadioButton_toggled(bool checked);

    void on_moveEndRadioButton_toggled(bool checked);

    void on_moveBothRadioButton_toggled(bool checked);

    void on_smartAdjustRadioButton_toggled(bool checked);

    void on_allowOverwritesCheckBox_toggled(bool checked);

private:
    Ui::AdjustDialog *ui;
    void setShortcuts();
    void setLabels();
    void showEditorAdjustPage();
    void populateValues();
    void clearValues();
    void setButtonState();
    QVector<QShortcut *> valueShortcuts;
    QShortcut * moveEndShortcut;
    QShortcut * moveBeginningShortcut;
    QShortcut * moveBothShortcut;
    QShortcut * smartAdjustShortcut;
    QShortcut * overwriteOthersShortcut;
    QShortcut * doneShortcut;
};

#endif // ADJUSTDIALOG_H
