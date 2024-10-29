#include "abstractnewbeatvaluewidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include "beatanalysis.h"
#include "helperfunctions.h"

AbstractNewBeatValueWidget::AbstractNewBeatValueWidget(float valueInBeats, QWidget * parent)
    :
      QFrame(parent),
      valueInBeats(valueInBeats)
{
//	TODO: setObjectName(QString::fromUtf8("sensible name goes here"));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    valueLayout = new QVBoxLayout(this);
    valueLayout->setObjectName(QString::fromUtf8("valueLayout"));
    valueFractionLayout = new QHBoxLayout();
    valueFractionLayout->setObjectName(QString::fromUtf8("valueFractionLayout"));

    valueLayout->addLayout(valueFractionLayout);

    valueNameLayout = new QHBoxLayout();
    valueNameLayout->setObjectName(QString::fromUtf8("valueNameLayout"));
    QSpacerItem * horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    valueNameLayout->addItem(horizontalSpacer_2);

    valueNameRadioButton = new QRadioButton(this);
    valueNameRadioButton->setObjectName(QString::fromUtf8("valueNameRadioButton"));
    connect(valueNameRadioButton, SIGNAL(toggled(bool)), this, SLOT(on_selected(bool)));

    valueNameLayout->addWidget(valueNameRadioButton);

    QSpacerItem * horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    valueNameLayout->addItem(horizontalSpacer);


    valueLayout->addLayout(valueNameLayout);
}

float AbstractNewBeatValueWidget::getCurrentValue()
{
    return (float) getNumerator() / getDivisor();
}

void AbstractNewBeatValueWidget::updateForValue(float newValueInBeats)
{
    valueInBeats = newValueInBeats;
    updateBackground();
}

QString AbstractNewBeatValueWidget::getName()
{
    return valueNameRadioButton->text();
}

void AbstractNewBeatValueWidget::select()
{
    valueNameRadioButton->setChecked(true);
}

void AbstractNewBeatValueWidget::deselect()
{
    valueNameRadioButton->setChecked(false);
}

void AbstractNewBeatValueWidget::updateBackground()
{
    float poop = poopness();
    QColor color("Dark Red");
    bool fail = true;
    if (poop < 100)
    {
        fail = false;
        float offness = poopness() / 100;
        short hue = 120 - (offness * 120);
        color = QColor::fromHsl(hue,255,191);
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

float AbstractNewBeatValueWidget::poopness()
{
    float deviation = percentageDifferenceBetween(getCurrentValue(), valueInBeats);
    bool acceptableProportion = (deviation <= BeatAnalysis::Configuration::maxPercentAcceptableBeatError);
    if (!acceptableProportion)
        return std::numeric_limits<float>::max();

    float beatsDifference = abs(valueInBeats - getCurrentValue());
    if (beatsDifference > 0.25 && BeatAnalysis::Configuration::allowHalfBeatsInBreaks)
        return std::numeric_limits<float>::max();

    return (100 * deviation) / BeatAnalysis::Configuration::maxPercentAcceptableBeatError;
}

float AbstractNewBeatValueWidget::matchness()
{
    if (!isAMatch())
        return 0;
    return 100 - poopness();
}

bool AbstractNewBeatValueWidget::isAMatch()
{
    return poopness() <= 100;
}

void AbstractNewBeatValueWidget::on_selected(bool checked)
{
    if (checked)
    {
        emit selected();
        emit selected(this);
    }
}

bool AbstractNewBeatValueWidget::isSelected()
{
    return valueNameRadioButton->isChecked();
}
