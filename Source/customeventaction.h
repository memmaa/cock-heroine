#ifndef CUSTOMEVENTACTION_H
#define CUSTOMEVENTACTION_H

#include <QAction>
class QHBoxLayout;
class QKeySequenceEdit;
class QSpinBox;
class QCheckBox;
class QWidget;

#include "event.h"

class CustomEventAction : public QAction
{
    Q_OBJECT
public:
    bool operator== (const CustomEventAction & other) const;
    CustomEventAction(Event event, bool trigger, bool record, QObject *parent);
    CustomEventAction(bool trigger, bool record, QObject *parent);
    QHBoxLayout * createGuiEditorLayout(QWidget *parent);
    QKeySequenceEdit * getPrimaryKeySequenceEdit() {return primaryKeySequenceEdit;}
    QKeySequenceEdit * getSecondaryKeySequenceEdit() {return secondaryKeySequenceEdit;}
    QString getCustomActionID();
    QString getPrimaryShortcutID();
    QString getSecondaryShortcutID();
    static CustomEventAction * fromString(const QString & string, QObject *parent);
    void addShortcut(QKeySequence newShortcut);

public slots:
    void newTriggerValue(int newValue);
    void newRecordValue(int newValue);
    void newEventType(int newValue);
    void newEventValue(int newValue);
    void performAction(bool checked);

private:
    Event actionEvent;
    bool shouldTrigger;
    bool shouldRecord;
    bool useSelectedEventValues;
    QWidget * originatingWidget = nullptr;
    QSpinBox * eventTypeBox;
    QSpinBox * eventValueBox;
    QCheckBox * triggerCheckbox;
    QCheckBox * recordCheckbox;
    QKeySequenceEdit * primaryKeySequenceEdit;
    QKeySequenceEdit * secondaryKeySequenceEdit;
};

#endif // CUSTOMEVENTACTION_H
