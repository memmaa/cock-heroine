#include "globals.h"
#include "adddialog.h"
#include "ui_adddialog.h"
#include "beatvaluewidget.h"
#include "editorwindow.h"
#include "ui_editorwindow.h"
#include <QSettings>
#include <QShortcut>
#include <QDebug>

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
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
            QString shortcutString = settings.value(QString("ADD_DIALOG_SHORTCUT_ROW_%1_COLUMN_%2").arg(row).arg(column), defaultValueShortcuts[row][column]).toString();
            QKeySequence sequence(shortcutString);
            QShortcut * shortcut = new QShortcut(sequence,this);
            BeatValueWidget * valueWidget = new BeatValueWidget(ui->valuesFrame, value, BeatValueWidget::SingleSelection);
            connect(shortcut, SIGNAL(activated()), valueWidget, SLOT(select()));
            connect(valueWidget, SIGNAL(selected(BeatValue *)), this, SLOT(newValueSelected(BeatValue *)));
            connect(valueWidget, SIGNAL(selected()), this, SLOT(ensureSingleSelection()));
            ui->gridLayout->addWidget(valueWidget, row, column);
            ++widgetsAdded;
        }
    }
    setShortcuts();
    setLabels();
    showEditorAddPage();
    setButtonState();
}

void AddDialog::setShortcuts()
{
    QSettings settings;

    QString strKeyAddBefore = settings.value(SHORTCUT_ADD_INSERT_BEFORE_SELECTION, "K").toString();
    QKeySequence keyAddBefore(strKeyAddBefore);
    addBeforeShortcut = new QShortcut(keyAddBefore,this);
    connect(addBeforeShortcut, SIGNAL(activated()), ui->beforeRadioButton, SLOT(click()));

    QString strKeyAddAfter = settings.value(SHORTCUT_ADD_INSERT_AFTER_SELECTION, "J").toString();
    QKeySequence keyAddAfter(strKeyAddAfter);
    addAfterShortcut = new QShortcut(keyAddAfter,this);
    connect(addAfterShortcut, SIGNAL(activated()), ui->afterRadioButton, SLOT(click()));

    QString strKeyMoveBack = settings.value(SHORTCUT_ADD_MOVE_OTHERS_BACK, "L").toString();
    QKeySequence keyMoveBack(strKeyMoveBack);
    moveBackShortcut = new QShortcut(keyMoveBack,this);
    connect(moveBackShortcut, SIGNAL(activated()), ui->shiftBackRadioButton, SLOT(click()));

    QString strKeyMoveForward = settings.value(SHORTCUT_ADD_MOVE_OTHERS_FORWARD, "H").toString();
    QKeySequence keyMoveForward(strKeyMoveForward);
    moveForwardShortcut = new QShortcut(keyMoveForward,this);
    connect(moveForwardShortcut, SIGNAL(activated()), ui->shiftForwardRadioButton, SLOT(click()));

    doneShortcut = new QShortcut(editor->ui->actionAdd->shortcut(),this);
    connect(doneShortcut, SIGNAL(activated()), this, SLOT(accept()));
}

void AddDialog::setLabels()
{
    //TODO: See DeleteDialog and do the same here
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::showEditorAddPage()
{
    editor->ui->intervalEditorTabs->setCurrentWidget(editor->ui->addIntervalTab);
}

void AddDialog::setButtonState()
{
    if (editor->ui->addIntervalBeforeRadioButton->isChecked())
        ui->beforeRadioButton->setChecked(true);
    if (editor->ui->addIntervalAfterRadioButton->isChecked())
        ui->afterRadioButton->setChecked(true);
    if (editor->ui->addIntervalMoveBackRadioButton->isChecked())
        ui->shiftBackRadioButton->setChecked(true);
    if (editor->ui->addIntervalMoveForwardRadioButton->isChecked())
        ui->shiftForwardRadioButton->setChecked(true);
    BeatValueWidget * closestMatch = nullptr;
    float minDifference = std::numeric_limits<float>::max();
    foreach (BeatValueWidget * child, ui->valuesFrame->findChildren<BeatValueWidget *>())
    {
        float difference = abs(editor->ui->addIntervalValueComboBox->currentText().toFloat() - child->getValue().getLength());
        if (difference < minDifference)
        {
            closestMatch = child;
            minDifference = difference;
        }
    }
    if (closestMatch != nullptr)
        closestMatch->select();
}

void AddDialog::newValueSelected(BeatValue * newValue)
{
    qDebug() << "Value is " << newValue->name;
    editor->ui->addIntervalValueComboBox->setEditText(QString("%1").arg(newValue->getLength(),0,'f',0));
}

void AddDialog::ensureSingleSelection()
{
    foreach (BeatValueWidget * child, ui->valuesFrame->findChildren<BeatValueWidget *>())
    {
        if (sender() != child)
        {
            child->deselect();
        }
    }
}

void AddDialog::on_beforeRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAddPage();
        editor->ui->addIntervalBeforeRadioButton->setChecked(checked);
    }
}

void AddDialog::on_afterRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAddPage();
        editor->ui->addIntervalAfterRadioButton->setChecked(checked);
    }
}

void AddDialog::on_shiftBackRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAddPage();
        editor->ui->addIntervalMoveBackRadioButton->setChecked(checked);
    }
}

void AddDialog::on_shiftForwardRadioButton_toggled(bool checked)
{
    if (checked)
    {
        showEditorAddPage();
        editor->ui->addIntervalMoveForwardRadioButton->setChecked(checked);
    }
}

void AddDialog::accept()
{
    editor->on_addIntervalButton_clicked();
    QDialog::accept();
}
