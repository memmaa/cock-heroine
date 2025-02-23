#ifndef NEWBEATVALUEWIDGET_H
#define NEWBEATVALUEWIDGET_H

#include "abstractnewbeatvaluewidget.h"

class QLabel;

class NewBeatValueWidget : public AbstractNewBeatValueWidget
{
    Q_OBJECT
public:
    NewBeatValueWidget(QWidget * parent, float valueInBeats, int divisor);
    void updateForValue(float newValueInBeats) override;
    QString getName() override;
    float getMatchResistance() override;
    int getNumerator() override;
    int getDivisor() override;
private:
    int numerator;
    int divisor;
    QLabel * valueNumeratorLabel;
    QLabel * valueDivisorLabel;
    void calculateNumerator();
    bool hasExistingMatchingValue();
    void updateName();
};

#endif // NEWBEATVALUEWIDGET_H
