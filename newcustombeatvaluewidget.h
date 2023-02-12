#ifndef NEWCUSTOMBEATVALUEWIDGET_H
#define NEWCUSTOMBEATVALUEWIDGET_H

#include "abstractnewbeatvaluewidget.h"

class QSpinBox;

class NewCustomBeatValueWidget : public AbstractNewBeatValueWidget
{
    Q_OBJECT
public:
    NewCustomBeatValueWidget(float valueInBeats, QWidget * parent = nullptr);
    void setValue(int numerator, int divisor);
    QString getName() override;
    int getNumerator() override;
    int getDivisor() override;
private slots:
    void on_numeratorValueChanged(int);
private:
    QSpinBox * valueNumeratorSpinBox;
    QSpinBox * valueDivisorSpinBox;
    float getMatchResistance() override;
    void updateBackground() override;
};

#endif // NEWCUSTOMBEATVALUEWIDGET_H
