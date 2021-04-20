#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#define SHORTCUT_ADD_INSERT_BEFORE_SELECTION "SHORTCUT_ADD_INSERT_BEFORE_SELECTION"
#define SHORTCUT_ADD_INSERT_AFTER_SELECTION "SHORTCUT_ADD_INSERT_AFTER_SELECTION"
#define SHORTCUT_ADD_MOVE_OTHERS_BACK "SHORTCUT_ADD_MOVE_OTHERS_BACK"
#define SHORTCUT_ADD_MOVE_OTHERS_FORWARD "SHORTCUT_ADD_MOVE_OTHERS_FORWARD"

#include <QDialog>
class QShortcut;
class BeatValue;

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = nullptr);
    ~AddDialog();

    virtual void accept() override;

private slots:
    void on_beforeRadioButton_toggled(bool checked);

    void on_afterRadioButton_toggled(bool checked);

    void on_shiftBackRadioButton_toggled(bool checked);

    void on_shiftForwardRadioButton_toggled(bool checked);

    void newValueSelected(BeatValue * newValue);

    void ensureSingleSelection();

private:
    Ui::AddDialog *ui;
    void setShortcuts();
    void setLabels();
    void showEditorAddPage();
    void setButtonState();
    QShortcut * addBeforeShortcut;
    QShortcut * addAfterShortcut;
    QShortcut * moveBackShortcut;
    QShortcut * moveForwardShortcut;
    QShortcut * doneShortcut;
};

#endif // ADDDIALOG_H
