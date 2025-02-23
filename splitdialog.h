#ifndef SPLITDIALOG_H
#define SPLITDIALOG_H

#define SHORTCUT_ADD_INSERT_BEFORE_SELECTION "SHORTCUT_ADD_INSERT_BEFORE_SELECTION"
#define SHORTCUT_ADD_INSERT_AFTER_SELECTION "SHORTCUT_ADD_INSERT_AFTER_SELECTION"
#define SHORTCUT_ADD_MOVE_OTHERS_BACK "SHORTCUT_ADD_MOVE_OTHERS_BACK"
#define SHORTCUT_ADD_MOVE_OTHERS_FORWARD "SHORTCUT_ADD_MOVE_OTHERS_FORWARD"

#include <QDialog>
class QShortcut;
class BeatValue;
class BeatPattern;

namespace Ui {
class SplitDialog;
}

class SplitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SplitDialog(QWidget *parent = nullptr);
    ~SplitDialog();

    virtual void accept() override;

private slots:
    void newValueSelected(BeatValue * newValue);

    void on_deleteButton_clicked();

private:
    Ui::SplitDialog *ui;
    void setShortcuts();
    void setLabels();
    void showEditorSplitPage();
    void setButtonState();
    void refreshPatternDisplay();
    QShortcut * addBeforeShortcut;
    QShortcut * addAfterShortcut;
    QShortcut * moveBackShortcut;
    QShortcut * moveForwardShortcut;
    QShortcut * doneShortcut;
    BeatPattern * pattern;
};

#endif // SPLITDIALOG_H
