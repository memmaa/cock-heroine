#include "newbeatvaluewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QApplication>
#include "globals.h"
#include "uniquebeatinterval.h"
#include "beatanalysis.h"

NewBeatValueWidget::NewBeatValueWidget(QWidget *parent, float valueInBeats, int divisor)
    :
        AbstractNewBeatValueWidget(valueInBeats, parent),
        numerator(1),
        divisor(divisor)
{
    valueNumeratorLabel = new QLabel(this);
    valueNumeratorLabel->setObjectName(QString::fromUtf8("valueNumeratorLabel"));
    valueNumeratorLabel->setAlignment(Qt::AlignCenter);

    valueFractionLayout->addWidget(valueNumeratorLabel);

    QLabel * valueSlashLabel = new QLabel(this);
    valueSlashLabel->setObjectName(QString::fromUtf8("valueSlashLabel"));
    valueSlashLabel->setAlignment(Qt::AlignCenter);
    valueSlashLabel->setText(QApplication::translate("EnableIdentifyIntervalsDialog", "/", nullptr));

    valueFractionLayout->addWidget(valueSlashLabel);

    valueDivisorLabel = new QLabel(this);
    valueDivisorLabel->setObjectName(QString::fromUtf8("valueDivisorLabel"));
    valueDivisorLabel->setAlignment(Qt::AlignCenter);
    valueDivisorLabel->setText(QString::number(divisor));

    valueFractionLayout->addWidget(valueDivisorLabel);


    updateForValue(valueInBeats);
}

void NewBeatValueWidget::updateForValue(float newValueInBeats)
{
    valueInBeats = newValueInBeats;
    calculateNumerator();
    updateName();
    updateBackground();
}

QString NewBeatValueWidget::getName()
{
    if (hasExistingMatchingValue())
    {
        return AbstractNewBeatValueWidget::getName();
    }
    return QString();
}

float NewBeatValueWidget::getMatchResistance()
{
    return poopness() * divisor;
}

int NewBeatValueWidget::getNumerator()
{
    return numerator;
}

int NewBeatValueWidget::getDivisor()
{
    return divisor;
}

void NewBeatValueWidget::calculateNumerator()
{
    while(true)
    {
        float currentValue = getCurrentValue();
        float currentDiff = abs(valueInBeats - currentValue);
        int bigger = numerator + 1;
        float biggerValue = (float) bigger / divisor;
        float biggerDiff = abs(valueInBeats - biggerValue);
        if (biggerDiff < currentDiff)
        {
            ++numerator;
            continue;
        }
        if (numerator > 1)
        {
            int smaller = numerator - 1;
            float smallerValue = (float) smaller / divisor;
            float smallerDiff = abs(valueInBeats - smallerValue);
            if (smallerDiff < currentDiff)
            {
                --numerator;
                continue;
            }
        }
        //the numerator can't be improved by increasing or decreasing - we're done
        break;
    }
    valueNumeratorLabel->setText(QString::number(numerator));
}

bool NewBeatValueWidget::hasExistingMatchingValue()
{
    for (auto value : beatValues)
    {
        float thisValue = value.value();
        float myValue = getCurrentValue();
        if (thisValue == myValue)
        {
            return true;
        }
    }
    return false;
}

void NewBeatValueWidget::updateName()
{
    for (auto value : beatValues)
    {
        float thisValue = value.value();
        float myValue = getCurrentValue();
        if (thisValue == myValue)
        {
            valueNameRadioButton->setText(value.name);
            return;
        }
    }
    //no matches found - must be new
    valueNameRadioButton->setText(QApplication::translate("EnableIdentifyIntervalsDialog", "New value", nullptr));
}
