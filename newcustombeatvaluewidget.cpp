#include "newcustombeatvaluewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QApplication>
#include "globals.h"
#include "uniquebeatinterval.h"
#include "beatanalysis.h"
#include <QSignalBlocker>

NewCustomBeatValueWidget::NewCustomBeatValueWidget(float valueInBeats, QWidget *parent)
    :
      AbstractNewBeatValueWidget(valueInBeats, parent)
{
    valueNumeratorSpinBox = new QSpinBox(this);
    valueNumeratorSpinBox->setObjectName(QString::fromUtf8("valueNumeratorSpinBox"));
    valueNumeratorSpinBox->setMinimum(1);
    valueNumeratorSpinBox->setMaximum(9999);
    valueNumeratorSpinBox->setValue(3);

    valueFractionLayout->addWidget(valueNumeratorSpinBox);
    connect(valueNumeratorSpinBox, SIGNAL(valueChanged(int)), this, SLOT(on_numeratorValueChanged(int)));

    QLabel * valueSlashLabel = new QLabel(this);
    valueSlashLabel->setObjectName(QString::fromUtf8("valueSlashLabel"));
    valueSlashLabel->setAlignment(Qt::AlignCenter);
    valueSlashLabel->setText(QApplication::translate("EnableIdentifyIntervalsDialog", "/", nullptr));

    valueFractionLayout->addWidget(valueSlashLabel);

    valueDivisorSpinBox = new QSpinBox(this);
    valueDivisorSpinBox->setObjectName(QString::fromUtf8("valueDivisorSpinBox"));
    valueDivisorSpinBox->setEnabled(false);
    valueDivisorSpinBox->setMinimum(1);
    valueDivisorSpinBox->setMaximum(9999);

    valueFractionLayout->addWidget(valueDivisorSpinBox);


    valueNameRadioButton->setText(QApplication::translate("EnableIdentifyIntervalsDialog", "Custom", nullptr));


    updateBackground();
}

void NewCustomBeatValueWidget::setValue(int numerator, int divisor)
{
    const QSignalBlocker blocker(valueNumeratorSpinBox);
    valueNumeratorSpinBox->setValue(numerator);
    valueDivisorSpinBox->setValue(divisor);
    updateBackground();
}

QString NewCustomBeatValueWidget::getName()
{
    return QString();
}

int NewCustomBeatValueWidget::getNumerator()
{
    return valueNumeratorSpinBox->value();
}

int NewCustomBeatValueWidget::getDivisor()
{
    return valueDivisorSpinBox->value();
}

void NewCustomBeatValueWidget::on_numeratorValueChanged(int)
{
    if (!valueNameRadioButton->isChecked())
        valueNameRadioButton->setChecked(true);
    updateBackground();
    emit selected(this);
}

float NewCustomBeatValueWidget::getMatchResistance()
{
    if (valueNameRadioButton->isChecked())
    {
        return 0;
    }
    return __FLT_MAX__;
}

#include "helperfunctions.h"
void NewCustomBeatValueWidget::updateBackground()
{
    QColor color;
    bool fail = false;
    float deviation = percentageDifferenceBetween(getCurrentValue(), valueInBeats);
    bool acceptableProportion = (deviation <= BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
    if (!acceptableProportion)
    {
        color = QColor("Dark Red");
        fail = true;
    }
    else
    {
        float beatsDifference = abs(valueInBeats - getCurrentValue());
        if (beatsDifference > 0.25 && BeatAnalysis::Configuration::allowHalfBeatsInBreaks)
        {
            color = QColor("Dark Red");
            fail = true;
        }
        else
        {
            float offness = deviation / BeatAnalysis::Configuration::maxPercentAcceptableBeatError;
            short hue = 120 - (offness * 120);
            color = QColor::fromHsl(hue,255,191);
        }
    }

    QString textColor;
    if (fail)
    {
        textColor = "white";
    }
    else
    {
        textColor = "black";
    }
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString rgbRep = QString("background-color: rgb(%1,%2,%3); color: %4").arg(r).arg(g).arg(b).arg(textColor);
    setStyleSheet(rgbRep);
}
