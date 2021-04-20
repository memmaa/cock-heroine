#include "keyboardshortcutsdialog.h"
#include "ui_keyboardshortcutsdialog.h"
#include "mainwindow.h"
#include "globals.h"
#include <QLabel>
#include <QKeySequenceEdit>
#include <QSettings>
#include <QAction>
#include "customeventaction.h"
#include <QPushButton>
#include "editorwindow.h"

//TODO:
//This class and its associated ones need a bit of TLC. I started working on this because I know
//that customisable shortcuts are important to some users, but I haven't had the time or inclination
//to really implement it properly.
//TODO: The editor window, and its Add, Adjust, Delete and Split dialogs should all contribute in
//createActionsList (the editor window is there but commented out at the time of writing)
//TODO: The custom shortcut needs improving for long-press events.
//e.g. an edge shortcut ('E' by default) creates a type 5 event when pressed and type 6 when released, but
//an 'as-fast-as-you-can' ('L' by deafult) sets the value of the resulting type 3 event according to how long the key was held down.
//These are coded separately.
//Some thought and design needed on how this can be done generically.
KeyboardShortcutsDialog::KeyboardShortcutsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyboardShortcutsDialog)
{
    ui->setupUi(this);
    allActions = createActionsList();
    createWidgetList();
}

KeyboardShortcutsDialog::~KeyboardShortcutsDialog()
{
    delete ui;
    delete allActions;
}

QList<QAction*>* KeyboardShortcutsDialog::createActionsList()
{
    QList<QAction*> * retVal = new QList<QAction*>();
    retVal->append(*(mainWindow->getActionsList()));
    //retVal->append(EditorWindow::getActionsList());
    return retVal;
}

void KeyboardShortcutsDialog::createWidgetList()
{
    foreach( QAction * action, *allActions )
    {
        QHBoxLayout * horizontalLayout = new QHBoxLayout();
        QLabel * label = new QLabel(getActionID(action), this);
        QKeySequenceEdit * primaryKeySequenceEdit = new QKeySequenceEdit(this);
        QKeySequenceEdit * secondaryKeySequenceEdit = new QKeySequenceEdit(this);
        QList<QKeySequence> shortcuts = action->shortcuts();
        if (!shortcuts.isEmpty())
        {
            primaryKeySequenceEdit->setKeySequence(shortcuts.first());
        }
        if (shortcuts.size() > 1)
        {
            secondaryKeySequenceEdit->setKeySequence(shortcuts.at(1));
        }

        allPrimaryShortcuts.append(primaryKeySequenceEdit);
        allSecondaryShortcuts.append(secondaryKeySequenceEdit);

        horizontalLayout->addWidget(label);
        horizontalLayout->addWidget(primaryKeySequenceEdit);
        horizontalLayout->addWidget(secondaryKeySequenceEdit);

        ui->scrollableAreaLayout->addLayout(horizontalLayout);
    }

    for (auto cea : *mainWindow->customShortcuts)
    {
        QHBoxLayout * layout = cea->createGuiEditorLayout(this);
        ui->scrollableAreaLayout->addLayout(layout);
    }
    /*QSettings settings;
    settings.beginGroup("CustomShortcuts");
    foreach (const QString &key, settings.childKeys()) {
            CustomEventAction * cea = CustomEventAction::fromString(key, this);
            if (cea)
            {
                QHBoxLayout * layout = cea->createGuiEditorLayout(this);
                ui->scrollableAreaLayout->addLayout(layout);
                QKeySequence sequnce(settings.value(key).toString());
            }

        //XXX TODO: Do keyboard shortcut thingies here.
        }
    settings.endGroup();*/

    QPushButton * addCustom = new QPushButton(tr("Add Custom Action Shortcut"));
    ui->scrollableAreaLayout->addWidget(addCustom);
    connect(addCustom, SIGNAL(pressed()), this, SLOT(on_addCustomShortcutButtonClicked()));
}

void KeyboardShortcutsDialog::on_addCustomShortcutButtonClicked()
{
    CustomEventAction * newAction = new CustomEventAction(Event(0,1,128),true,true,mainWindow);
    mainWindow->customShortcuts->append(newAction);
    QHBoxLayout * layout = newAction->createGuiEditorLayout(this);
    ui->scrollableAreaLayout->addLayout(layout);
}

QString KeyboardShortcutsDialog::getActionContext(QAction *action)
{
    if (action->shortcutContext() == Qt::ApplicationShortcut)
        return "Global";
    else if (mainWindow->getActionsList()->contains(action))
        return "Main Window";
    else
        return"Unknown Context";
}

QString KeyboardShortcutsDialog::getActionID(QAction *action)
{
    return getActionContext(action) + ": " + action->text();
}

QString KeyboardShortcutsDialog::getActionPrimaryShortcutID(QAction *action)
{
    return getActionID(action) + "_PrimaryShortcut";
}

QString KeyboardShortcutsDialog::getActionSecondaryShortcutID(QAction *action)
{
    return getActionID(action) + "_SecondaryShortcut";
}

void KeyboardShortcutsDialog::applyShortcutPrefsFromUi()
{
    QSettings settings;
    settings.beginGroup("Shortcuts");
    for (int i = 0; i < allActions->size(); ++i)
    {
        QAction * action = allActions->at(i);
        if (!allPrimaryShortcuts.at(i)->keySequence().isEmpty())
            settings.setValue(getActionPrimaryShortcutID(action), allPrimaryShortcuts.at(i)->keySequence());
        if (!allSecondaryShortcuts.at(i)->keySequence().isEmpty())
            settings.setValue(getActionSecondaryShortcutID(action), allSecondaryShortcuts.at(i)->keySequence());
    }
    settings.endGroup();

    settings.beginGroup("CustomShortcuts");
    settings.remove("");

    for (auto shortcut : *mainWindow->customShortcuts)
    {
        if ( ! shortcut->getPrimaryKeySequenceEdit()->keySequence().isEmpty())
        {
            settings.setValue(shortcut->getPrimaryShortcutID(),shortcut->getPrimaryKeySequenceEdit()->keySequence());
        }
        if ( ! shortcut->getSecondaryKeySequenceEdit()->keySequence().isEmpty())
        {
            settings.setValue(shortcut->getSecondaryShortcutID(),shortcut->getSecondaryKeySequenceEdit()->keySequence());
        }
    }
    settings.endGroup();
}

void KeyboardShortcutsDialog::applyActionShortcutsFromPrefs()
{
    QSettings settings;
    settings.beginGroup("Shortcuts");
    QList<QAction *> * actions = createActionsList();
    for (int i = 0; i < actions->size(); ++i)
    {
        QList<QKeySequence> shortcuts;
        QAction * action = actions->at(i);
        QString primaryShortcutText = settings.value(getActionPrimaryShortcutID(action)).toString();
        if (!primaryShortcutText.isEmpty())
        {
            QKeySequence primaryShortcut = QKeySequence(primaryShortcutText);
            shortcuts.append(primaryShortcut);
        }
        QString secondaryShortcutText = settings.value(getActionSecondaryShortcutID(action)).toString();
        if (!secondaryShortcutText.isEmpty())
        {
            QKeySequence secondaryShortcut = QKeySequence(secondaryShortcutText);
            shortcuts.append(secondaryShortcut);
        }
        if (!shortcuts.isEmpty())
            action->setShortcuts(shortcuts);
    }
    delete actions;
    settings.endGroup();
    mainWindow->reregisterAllCustomShortcuts();
}

void KeyboardShortcutsDialog::accept()
{
    applyShortcutPrefsFromUi();
    applyActionShortcutsFromPrefs();
    QDialog::accept();
}
