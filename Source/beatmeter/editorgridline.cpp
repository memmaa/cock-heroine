#include "editorgridline.h"
#include <QPen>
#include "beatanalysis.h"
#include "optionsdialog.h"

EditorGridLine::EditorGridLine(float beatsFromNow, float opacity, QGraphicsItem * parent)
    :
      QGraphicsLineItem(parent),
      beatsFromNow(beatsFromNow)
{
    reposition();

    setOpacity(opacity);

    QPen pen;
    pen.setWidth(1);
    setPen(pen);
}

void EditorGridLine::reposition()
{
    qreal xCoordinate = beatsFromNow * getPixelsPerBeat();
    int yHalfLength = (qreal) OptionsDialog::getBeatMeterHeight() / 2;
    QLineF line(xCoordinate, yHalfLength, xCoordinate, -yHalfLength);
    setLine(line);
}

float EditorGridLine::value()
{
    return beatsFromNow;
}

qreal EditorGridLine::getPixelsPerBeat()
{
    if (false == BeatAnalysis::Configuration::tempoEstablished)
        return (qreal) OptionsDialog::getBeatMeterSpeed() / 2;
    return (BeatAnalysis::Configuration::tempoInterval() * OptionsDialog::getBeatMeterSpeed()) / 1000;
}
