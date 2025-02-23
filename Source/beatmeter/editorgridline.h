#ifndef EDITORGRIDLINE_H
#define EDITORGRIDLINE_H

#include <QGraphicsLineItem>

class EditorGridLine : public QGraphicsLineItem
{
//    Q_OBJECT
public:
    EditorGridLine(float beatsFromNow, float opacity, QGraphicsItem *parent = nullptr);
    void reposition();
    float value();
private:
    float beatsFromNow;
    qreal getPixelsPerBeat();
};

#endif // EDITORGRIDLINE_H
