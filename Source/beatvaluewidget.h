#ifndef BEATVALUEWIDGET_H
#define BEATVALUEWIDGET_H

#include <QFrame>
#include "beatvalue.h"
class QVBoxLayout;
class QLabel;
class QAbstractButton;

class BeatValueWidget : public QFrame
{
public:
    enum SelectionMode {
        RepeatableSelection,
        SingleSelection,
        NoSelection
    };
private:
    Q_OBJECT
public:
    explicit BeatValueWidget(QWidget *parent, BeatValue value, SelectionMode mode);
    void deselect();
    BeatValue getValue() {return value;};

signals:
    void selected();
    void selected(BeatValue * value);

public slots:
    void select();

private slots:
    void on_selected(bool on_selected);

private:
    BeatValue value;
    QVBoxLayout * layout;
    QLabel * label;
    SelectionMode selectionMode;
    QAbstractButton * selectionButton;
};

#endif // BEATVALUEWIDGET_H
