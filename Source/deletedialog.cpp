#include "deletedialog.h"
#include "ui_deletedialog.h"
#include "editorwindow.h"
#include "ui_editorwindow.h"
#include <QShortcut>

DeleteDialog::DeleteDialog(EditorWindow *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDialog),
    editor(parent)
{
    ui->setupUi(this);
    setShortcuts();
    setLabels();
    showEditorDeletePage();
    setButtonState();
}

void DeleteDialog::setShortcuts()
{
    QSettings settings;

    QString strKeyDelMergeFol = settings.value("DELETE_MERGE_WITH_FOLLOWING", "Y").toString();
    QKeySequence keyDelMergeFol(strKeyDelMergeFol);
    mergeFollowingShortcut = new QShortcut(keyDelMergeFol,this);
    connect(mergeFollowingShortcut, SIGNAL(activated()), ui->deleteIntervalMergeWithFollowingRadioButton, SLOT(click()));

    QString strKeyDelMergePre = settings.value("DELETE_MERGE_WITH_PRECEDING", "U").toString();
    QKeySequence keyDelMergePre(strKeyDelMergePre);
    mergePrecedingShortcut = new QShortcut(keyDelMergePre,this);
    connect(mergePrecedingShortcut, SIGNAL(activated()), ui->deleteIntervalMergeWithPreceedingRadioButton, SLOT(click()));

    QString strKeyDelMergeBoth = settings.value("DELETE_MERGE_WITH_BOTH", "I").toString();
    QKeySequence keyDelMergeBoth(strKeyDelMergeBoth);
    mergeBothShortcut = new QShortcut(keyDelMergeBoth,this);
    connect(mergeBothShortcut, SIGNAL(activated()), ui->deleteIntervalMergePreceedingWithFollowingRadioButton, SLOT(click()));

    QString strKeyDelMoveLater = settings.value("DELETE_MOVE_LATER", "O").toString();
    QKeySequence keyDelMoveLater(strKeyDelMoveLater);
    moveLaterShortcut = new QShortcut(keyDelMoveLater,this);
    connect(moveLaterShortcut, SIGNAL(activated()), ui->deleteIntervalMoveLaterIntervalsRadioButton, SLOT(click()));

    QString strKeyDelMoveEarlier = settings.value("DELETE_MOVE_EARLIER", "P").toString();
    QKeySequence keyDelMoveEarlier(strKeyDelMoveEarlier);
    moveEarlierShortcut = new QShortcut(keyDelMoveEarlier,this);
    connect(moveEarlierShortcut, SIGNAL(activated()), ui->deleteIntervalMoveEarlierIntervalsRadioButton, SLOT(click()));

    doneShortcut = new QShortcut(editor->ui->actionDelete->shortcut(),this);
    connect(doneShortcut, SIGNAL(activated()), this, SLOT(accept()));
}

void DeleteDialog::setLabels()
{
    //label the buttons clearly with keyboard shortcuts. Not everyone likes to use the keyboard, but it makes editing a lot quicker if you do...
    ui->deleteIntervalMergeWithFollowingRadioButton->setText(tr("(%1) Merge with following interval (remove the beat at the end of interval)").arg(mergeFollowingShortcut->key().toString()));
    ui->deleteIntervalMergeWithPreceedingRadioButton->setText(tr("(%1) Merge with Preceding interval (remove the beat at the beginning of interval)").arg(mergePrecedingShortcut->key().toString()));
    ui->deleteIntervalMergePreceedingWithFollowingRadioButton->setText(tr("(%1) Merge preceding interval with following interval (remove both ends of interval)").arg(mergeBothShortcut->key().toString()));
    ui->deleteIntervalMoveLaterIntervalsRadioButton->setText(tr("(%1) Move later beats to fill the gap (all beats to the right of the selected beat will be moved left)").arg(moveLaterShortcut->key().toString()));
    ui->deleteIntervalMoveEarlierIntervalsRadioButton->setText(tr("(%1) Move earlier beats to fill the gap (all beats to the left of the selected beat will be moved right)").arg(moveEarlierShortcut->key().toString()));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("(%1) OK").arg(doneShortcut->key().toString()));
}

void DeleteDialog::setButtonState()
{
    if (editor->ui->deleteIntervalMergeWithFollowingRadioButton->isChecked())
        ui->deleteIntervalMergeWithFollowingRadioButton->setChecked(true);
    if (editor->ui->deleteIntervalMergeWithPreceedingRadioButton->isChecked())
        ui->deleteIntervalMergeWithPreceedingRadioButton->setChecked(true);
    if (editor->ui->deleteIntervalMergePreceedingWithFollowingRadioButton->isChecked())
        ui->deleteIntervalMergePreceedingWithFollowingRadioButton->setChecked(true);
    if (editor->ui->deleteIntervalMoveLaterIntervalsRadioButton->isChecked())
        ui->deleteIntervalMoveLaterIntervalsRadioButton->setChecked(true);
    if (editor->ui->deleteIntervalMoveEarlierIntervalsRadioButton->isChecked())
        ui->deleteIntervalMoveEarlierIntervalsRadioButton->setChecked(true);
}

DeleteDialog::~DeleteDialog()
{
    disconnect(mergeFollowingShortcut, SIGNAL(activated()), ui->deleteIntervalMergeWithFollowingRadioButton, SLOT(click()));
    disconnect(mergePrecedingShortcut, SIGNAL(activated()), ui->deleteIntervalMergeWithPreceedingRadioButton, SLOT(click()));
    disconnect(mergeBothShortcut, SIGNAL(activated()), ui->deleteIntervalMergePreceedingWithFollowingRadioButton, SLOT(click()));
    disconnect(moveLaterShortcut, SIGNAL(activated()), ui->deleteIntervalMoveLaterIntervalsRadioButton, SLOT(click()));
    disconnect(moveEarlierShortcut, SIGNAL(activated()), ui->deleteIntervalMoveEarlierIntervalsRadioButton, SLOT(click()));
    disconnect(doneShortcut, SIGNAL(activated()), this, SLOT(accept()));
    delete ui;
}

void DeleteDialog::showEditorDeletePage()
{
    editor->ui->intervalEditorTabs->setCurrentWidget(editor->ui->deleteIntervalsTab);
}

void DeleteDialog::on_deleteIntervalMergeWithFollowingRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorDeletePage();
        editor->ui->deleteIntervalMergeWithFollowingRadioButton->setChecked(checked);
    }
}

void DeleteDialog::on_deleteIntervalMergeWithPreceedingRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorDeletePage();
        editor->ui->deleteIntervalMergeWithPreceedingRadioButton->setChecked(checked);
    }
}

void DeleteDialog::on_deleteIntervalMergePreceedingWithFollowingRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorDeletePage();
        editor->ui->deleteIntervalMergePreceedingWithFollowingRadioButton->setChecked(checked);
    }
}

void DeleteDialog::on_deleteIntervalMoveLaterIntervalsRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorDeletePage();
        editor->ui->deleteIntervalMoveLaterIntervalsRadioButton->setChecked(checked);
    }
}

void DeleteDialog::on_deleteIntervalMoveEarlierIntervalsRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorDeletePage();
        editor->ui->deleteIntervalMoveEarlierIntervalsRadioButton->setChecked(checked);
    }
}

void DeleteDialog::accept()
{
    editor->on_deleteIntervalButton_clicked();
    QDialog::accept();
}
