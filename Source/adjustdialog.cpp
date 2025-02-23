#include "globals.h"
#include "adjustdialog.h"
#include "ui_adjustdialog.h"
#include "beatvaluewidget.h"
#include "editorwindow.h"
#include "ui_editorwindow.h"
#include <QShortcut>

AdjustDialog::AdjustDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdjustDialog)
{
    ui->setupUi(this);
    populateValues();
    setShortcuts();
    setLabels();
    showEditorAdjustPage();
    setButtonState();
}

AdjustDialog::AdjustDialog(BeatValue presetValue, QWidget *parent)
    :
    QDialog(parent),
    ui(new Ui::AdjustDialog)
{
    ui->setupUi(this);
    populateSingleValue(presetValue);
    setShortcuts();
    setLabels();
    showEditorAdjustPage();
    setButtonState();
}

void AdjustDialog::populateValues()
{
    QVector<BeatValue *> values;
    for (int i = 0; i < editor->ui->adjustIntervalNewValueComboBox->count(); ++i)
    {
        QString entry = editor->ui->adjustIntervalNewValueComboBox->itemText(i);
        BeatValue * value = getBeatValueFromDropdownEntry(entry);
        if (value != nullptr)
        {
            values.append(value);
        }
    }
    int numColumns = (values.length() + 1) / 2;
    int widgetsAdded = 0;
    QSettings settings;
    for (int i = 0; i < values.length(); ++i) {
        BeatValue * value = values[i];
        if (value->active)
        {
            int row = widgetsAdded / numColumns;
            int column = widgetsAdded % numColumns;
            QString shortcutString = settings.value(QString("ADJUST_DIALOG_SHORTCUT_ROW_%1_COLUMN_%2").arg(row).arg(column), defaultValueShortcuts[row][column]).toString();
            QKeySequence sequence(shortcutString);
            QShortcut * shortcut = new QShortcut(sequence,this);
            valueShortcuts.append(shortcut);
            BeatValueWidget * valueWidget = new BeatValueWidget(ui->valuesFrame, *value, BeatValueWidget::SingleSelection);
            connect(shortcut, SIGNAL(activated()), valueWidget, SLOT(select()));
            connect(valueWidget, SIGNAL(selected(BeatValue *)), this, SLOT(newValueSelected(BeatValue *)));
            connect(valueWidget, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
            ui->gridLayout->addWidget(valueWidget, row, column);
            ++widgetsAdded;
        }
    }
}

void AdjustDialog::populateSingleValue(BeatValue value)
{
    BeatValueWidget * valueWidget = new BeatValueWidget(ui->valuesFrame, value, BeatValueWidget::SingleSelection);
    connect(valueWidget, SIGNAL(selected(BeatValue *)), this, SLOT(newValueSelected(BeatValue *)));
    connect(valueWidget, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
    ui->gridLayout->addWidget(valueWidget, 1, 1);
    newValueSelected(&value);
}

void AdjustDialog::clearValues()
{
//    foreach (BeatValueWidget * child, ui->valuesFrame->findChildren<BeatValueWidget *>())
//    {
//        disconnect(child, SIGNAL(selected(BeatValue *)), this, SLOT(newValueSelected(BeatValue *)));
//        disconnect(child, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
//        ui->gridLayout->removeWidget(child);
//        child->deleteLater();
//    }
    qDeleteAll(ui->valuesFrame->findChildren<BeatValueWidget*>("", Qt::FindDirectChildrenOnly));
    foreach (QShortcut * shortcut, valueShortcuts)
    {
        shortcut->deleteLater();
    }
    valueShortcuts.clear();
}

void AdjustDialog::setShortcuts()
{
    QSettings settings;

    QString strKeyMoveEnd = settings.value(SHORTCUT_ADJUST_MOVE_END, "L").toString();
    QKeySequence keyMoveEnd(strKeyMoveEnd);
    moveEndShortcut = new QShortcut(keyMoveEnd,this);
    connect(moveEndShortcut, SIGNAL(activated()), ui->moveEndRadioButton, SLOT(click()));

    QString strKeyMoveBeginning = settings.value(SHORTCUT_ADJUST_MOVE_BEGINNING, "J").toString();
    QKeySequence keyMoveBeginning(strKeyMoveBeginning);
    moveBeginningShortcut = new QShortcut(keyMoveBeginning,this);
    connect(moveBeginningShortcut, SIGNAL(activated()), ui->moveBeginningRadioButton, SLOT(click()));

    QString strKeyMoveBoth = settings.value(SHORTCUT_ADJUST_MOVE_BOTH, "K").toString();
    QKeySequence keyMoveBoth(strKeyMoveBoth);
    moveBothShortcut = new QShortcut(keyMoveBoth,this);
    //don't connect here because shortcut will not always be valid

    QString strKeySmartAdjust = settings.value(SHORTCUT_ADJUST_SMART_ADJUST, "M").toString();
    QKeySequence keySmartAdjust(strKeySmartAdjust);
    smartAdjustShortcut = new QShortcut(keySmartAdjust,this);
    //don't connect here because shortcut will not always be valid

    QString strKeyAllowOverwrite = settings.value(SHORTCUT_ADJUST_OVERWRITE_OTHERS, "N").toString();
    QKeySequence keyAllowOverwite(strKeyAllowOverwrite);
    overwriteOthersShortcut = new QShortcut(keyAllowOverwite,this);
    connect(overwriteOthersShortcut, SIGNAL(activated()), ui->allowOverwritesCheckBox, SLOT(click()));

    doneShortcut = new QShortcut(editor->ui->actionAdjust->shortcut(),this);
    connect(doneShortcut, SIGNAL(activated()), this, SLOT(accept()));
}

void AdjustDialog::setLabels()
{
    //TODO: See DeleteDialog and do the same here
}

AdjustDialog::~AdjustDialog()
{
    delete ui;
}

void AdjustDialog::showEditorAdjustPage()
{
    editor->ui->intervalEditorTabs->setCurrentWidget(editor->ui->adjustIntervalsTab);
}

void AdjustDialog::setButtonState()
{
    if (editor->ui->adjustIntervalMoveEndRadioButton->isChecked())
        ui->moveEndRadioButton->setChecked(true);
    if (editor->ui->adjustIntervalMoveBeginningRadioButton->isChecked())
        ui->moveBeginningRadioButton->setChecked(true);

    if (editor->ui->adjustIntervalMoveBothRadioButton->isChecked())
        ui->moveBothRadioButton->setChecked(true);
    ui->moveBothRadioButton->setEnabled(editor->ui->adjustIntervalMoveBothRadioButton->isEnabled());
    if (ui->moveBothRadioButton->isEnabled())
    {
        disconnect(moveBothShortcut, SIGNAL(activated()), ui->moveBothRadioButton, SLOT(click()));
        connect(moveBothShortcut, SIGNAL(activated()), ui->moveBothRadioButton, SLOT(click()));
    }
    else
    {
        disconnect(moveBothShortcut, SIGNAL(activated()), ui->moveBothRadioButton, SLOT(click()));
    }

    if (editor->ui->adjustIntervalSmartAdjustRadioButton->isChecked())
        ui->smartAdjustRadioButton->setChecked(true);
    ui->smartAdjustRadioButton->setEnabled(editor->ui->adjustIntervalSmartAdjustRadioButton->isEnabled());
    if (ui->smartAdjustRadioButton->isEnabled())
    {
        disconnect(smartAdjustShortcut, SIGNAL(activated()), ui->smartAdjustRadioButton, SLOT(click()));
        connect(smartAdjustShortcut, SIGNAL(activated()), ui->smartAdjustRadioButton, SLOT(click()));
    }
    else
    {
        disconnect(smartAdjustShortcut, SIGNAL(activated()), ui->smartAdjustRadioButton, SLOT(click()));
    }

    //avoid infinite recursion
    bool oldState = ui->allowOverwritesCheckBox->blockSignals(true);
    ui->allowOverwritesCheckBox->setChecked(editor->ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox->isChecked());
    ui->allowOverwritesCheckBox->blockSignals(oldState);

    BeatValue * selectedValue = getBeatValueFromDropdownEntry(editor->ui->adjustIntervalNewValueComboBox->currentText());
    if (selectedValue == nullptr)
        return;
    foreach (BeatValueWidget * child, ui->valuesFrame->findChildren<BeatValueWidget *>())
    {
        if (child->getValue().name == selectedValue->name)
        {
            child->select();
            return;
        }
    }
}

void AdjustDialog::newValueSelected(BeatValue * newValue)
{
    editor->ui->adjustIntervalNewValueComboBox->setEditText(QString("%1 (%2)").arg(newValue->getLength(),0,'f',0).arg(newValue->name));
}

void AdjustDialog::ensureSingleSelection()
{
    foreach (BeatValueWidget * child, ui->valuesFrame->findChildren<BeatValueWidget *>())
    {
        if (sender() != child)
        {
            child->deselect();
        }
    }
}

void AdjustDialog::accept()
{
    editor->on_adjustIntervalButton_clicked();
    QDialog::accept();
}

void AdjustDialog::on_moveBeginningRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAdjustPage();
        editor->ui->adjustIntervalMoveBeginningRadioButton->setChecked(checked);
    }
}

void AdjustDialog::on_moveEndRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAdjustPage();
        editor->ui->adjustIntervalMoveEndRadioButton->setChecked(checked);
    }
}

void AdjustDialog::on_moveBothRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAdjustPage();
        editor->ui->adjustIntervalMoveBothRadioButton->setChecked(checked);
    }
}

void AdjustDialog::on_smartAdjustRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAdjustPage();
        editor->ui->adjustIntervalSmartAdjustRadioButton->setChecked(checked);
    }
}

void AdjustDialog::on_allowOverwritesCheckBox_toggled(bool)
{
    showEditorAdjustPage();
    clearValues();
    editor->ui->adjustIntervalAllowOverwritingOtherTimestampsCheckBox->click();
    populateValues();
    setButtonState();
}
