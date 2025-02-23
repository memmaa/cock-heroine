#include "customeventaction.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QKeySequenceEdit>
#include <QCheckBox>
#include "mainwindow.h"

CustomEventAction::CustomEventAction(Event event, bool trigger, bool record, QObject *parent)
    :
    QAction(parent),
    actionEvent(event),
    shouldTrigger(trigger),
    shouldRecord(record),
    useSelectedEventValues(false)
{
    //this space intentionally left commented
    connect(this, SIGNAL(triggered(bool)), this, SLOT(performAction(bool)));
}

CustomEventAction::CustomEventAction(bool trigger, bool record, QObject *parent)
    :
    QAction(parent),
    shouldTrigger(trigger),
    shouldRecord(record),
    useSelectedEventValues(true)
{
    //this space intentionally left green
}

QHBoxLayout * CustomEventAction::createGuiEditorLayout(QWidget * parent)
{
    QHBoxLayout * horizontalLayout = new QHBoxLayout(parent);
    QLabel * eventTypeLabel = new QLabel("Type:", parent);

    horizontalLayout->addWidget(eventTypeLabel);

    eventTypeBox = new QSpinBox(parent);
    eventTypeBox->setObjectName(QString::fromUtf8("eventTypeBox"));
    eventTypeBox->setRange(-1, 127);
    eventTypeBox->setSpecialValueText(tr("User"));
    eventTypeBox->setValue(actionEvent.type());
    connect(eventTypeBox,SIGNAL(valueChanged(int)),this,SLOT(newEventType(int)));

    horizontalLayout->addWidget(eventTypeBox);

    QLabel * eventValueLabel = new QLabel("Value:", parent);
    eventValueLabel->setObjectName(QString::fromUtf8("eventValueLabel"));

    horizontalLayout->addWidget(eventValueLabel);

    eventValueBox = new QSpinBox(parent);
    eventValueBox->setObjectName(QString::fromUtf8("eventValueBox"));
    eventValueBox->setRange(-1, 32767);
    eventValueBox->setSpecialValueText(tr("User"));
    eventValueBox->setValue(actionEvent.value);
    connect(eventValueBox,SIGNAL(valueChanged(int)),this,SLOT(newEventValue(int)));

    horizontalLayout->addWidget(eventValueBox);

    triggerCheckbox = new QCheckBox("Trigger:", parent);
    triggerCheckbox->setChecked(shouldTrigger);
    connect(triggerCheckbox,SIGNAL(stateChanged(int)),this,SLOT(newTriggerValue(int)));

    horizontalLayout->addWidget(triggerCheckbox);

    recordCheckbox = new QCheckBox("Record:", parent);
    recordCheckbox->setChecked(shouldRecord);
    connect(recordCheckbox,SIGNAL(stateChanged(int)),this,SLOT(newRecordValue(int)));

    horizontalLayout->addWidget(recordCheckbox);

    QLabel * label = new QLabel(QString::fromUtf8("Shortcuts:"), parent);
    primaryKeySequenceEdit = new QKeySequenceEdit(parent);
    secondaryKeySequenceEdit = new QKeySequenceEdit(parent);
    QList<QKeySequence> shortcutsList = shortcuts();
    if (!shortcutsList.isEmpty())
    {
        primaryKeySequenceEdit->setKeySequence(shortcutsList.first());
    }
    if (shortcutsList.size() > 1)
    {
        secondaryKeySequenceEdit->setKeySequence(shortcutsList.at(1));
    }

    horizontalLayout->addWidget(label);
    horizontalLayout->addWidget(primaryKeySequenceEdit);
    horizontalLayout->addWidget(secondaryKeySequenceEdit);

    return horizontalLayout;
}

CustomEventAction * CustomEventAction::fromString(const QString & string, QObject * parent)
{
    QRegularExpression re("event_aXX,(\\d+|XX),(\\d+|XX)([TF])([TF])_(Primary|Secondary)Shortcut");
    QRegularExpressionMatch match = re.match(string);
    if (!match.hasMatch())
    {
        return nullptr;
    }
    bool trigger = match.captured(3) == "T";
    bool record = match.captured(4) == "T";
    if (match.captured(1) == "XX")
    {
        //we're using the user-specified event values in the UI
        CustomEventAction * retVal = new CustomEventAction(trigger,record,parent);
        return retVal;
    }
    //we should have an encoded event.
    unsigned char type = match.captured(1).toInt();
    short value = match.captured(2).toInt();
    Event event(0,type,value);
    CustomEventAction * retVal = new CustomEventAction(event,trigger,record,parent);
    return retVal;
}

QString CustomEventAction::getCustomActionID() {
    QString retVal = QString("event_aXX,%1,%2%3%4")
                                        .arg(useSelectedEventValues ? QString("XX") : QString("%1").arg(actionEvent.type()))
                                           .arg(useSelectedEventValues ? QString("XX") : QString("%1").arg(actionEvent.value))
                                             .arg(shouldTrigger ? "T" : "F")
                                               .arg(shouldRecord ? "T" : "F");
    return retVal;
}

QString CustomEventAction::getPrimaryShortcutID()
{
    return getCustomActionID() + "_PrimaryShortcut";
}

QString CustomEventAction::getSecondaryShortcutID()
{
    return getCustomActionID() + "_SecondaryShortcut";
}

void CustomEventAction::addShortcut(QKeySequence newShortcut)
{
    auto shortcutList = shortcuts();
    shortcutList.append(newShortcut);
    setShortcuts(shortcutList);
}

void CustomEventAction::newEventType(int newType)
{
    static bool stacked = false;
    if (!stacked)
    {
        stacked = true;
        if (newType == -1)
        {
            eventValueBox->setValue(-1);
            useSelectedEventValues = true;
        }
        else
        {
            useSelectedEventValues = false;
            actionEvent.setType(static_cast<unsigned char>(newType));
            if (eventValueBox->value() == -1)
            {
                eventValueBox->setValue(0);
            }
        }
        stacked = false;
    }
}

void CustomEventAction::newEventValue(int newValue)
{
    static bool stacked = false;
    if (!stacked)
    {
        stacked = true;
        if (newValue == -1)
        {
            eventTypeBox->setValue(-1);
            useSelectedEventValues = true;
        }
        else
        {
            actionEvent.value = static_cast<short>(newValue);
            if (eventTypeBox->value() == -1)
            {
                eventTypeBox->setValue(1);
            }
        }
        stacked = false;
    }
}

void CustomEventAction::newRecordValue(int newValue)
{
    shouldRecord = newValue == Qt::Checked;
}

void CustomEventAction::newTriggerValue(int newValue)
{
    shouldTrigger = newValue == Qt::Checked;
}

void CustomEventAction::performAction(bool /*'checkState', but we don't use it*/)
{
    if (useSelectedEventValues)
    {
        actionEvent = mainWindow->createEventNow();
    }
    else
    {
        actionEvent.timestamp = currentTimecode();
    }
    if (shouldTrigger)
    {
        mainWindow->triggerEvent(actionEvent);
    }
    if (shouldRecord)
    {
        mainWindow->addEventToTable(actionEvent);
    }
}

bool CustomEventAction::operator== (const CustomEventAction & other) const {
    return actionEvent == other.actionEvent
                    && shouldTrigger == other.shouldTrigger
                    && shouldRecord == other.shouldRecord
                    && useSelectedEventValues == other.useSelectedEventValues;
}
