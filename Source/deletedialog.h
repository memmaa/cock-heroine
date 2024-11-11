#ifndef DELETEDIALOG_H
#define DELETEDIALOG_H

#include <QDialog>
class EditorWindow;
class QShortcut;

namespace Ui {
class DeleteDialog;
}

class DeleteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteDialog(EditorWindow *parent = nullptr);
    ~DeleteDialog();

    virtual void accept() override;

private slots:
    void on_deleteIntervalMergeWithFollowingRadioButton_toggled(bool checked);

    void on_deleteIntervalMergeWithPreceedingRadioButton_toggled(bool checked);

    void on_deleteIntervalMergePreceedingWithFollowingRadioButton_toggled(bool checked);

    void on_deleteIntervalMoveLaterIntervalsRadioButton_toggled(bool checked);

    void on_deleteIntervalMoveEarlierIntervalsRadioButton_toggled(bool checked);

private:
    Ui::DeleteDialog *ui;
    EditorWindow * editor;
    void showEditorDeletePage();
    void setLabels();
    void setShortcuts();
    void setButtonState();
    QShortcut * mergeFollowingShortcut;
    QShortcut * mergePrecedingShortcut;
    QShortcut * mergeBothShortcut;
    QShortcut * moveLaterShortcut;
    QShortcut * moveEarlierShortcut;
    QShortcut * doneShortcut;
};

#endif // DELETEDIALOG_H
