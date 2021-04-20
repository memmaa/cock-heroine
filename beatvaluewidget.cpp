#include "beatvaluewidget.h"
#include <QVBoxLayout>
#include "beatvalue.h"
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>

BeatValueWidget::BeatValueWidget(QWidget *parent, BeatValue value, SelectionMode mode)
    :
    QFrame(parent),
    value(value),
    layout(new QVBoxLayout(this)),
    label(new QLabel(value.name, this)),
    selectionMode(mode)
{
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    switch (mode)
    {
    case RepeatableSelection:
        selectionButton = new QPushButton(tr("Add"),this);
        connect(selectionButton, SIGNAL(clicked(bool)), this, SLOT(on_selected(bool)));
        break;
    case SingleSelection:
        selectionButton = new QRadioButton(parent);
        connect(selectionButton, SIGNAL(clicked(bool)), this, SLOT(on_selected(bool)));
        break;
    case NoSelection:
        selectionButton = nullptr;
        break;
    }

    layout->addWidget(label);
    layout->addWidget(selectionButton);
}

void BeatValueWidget::select()
{
    switch (selectionMode) {
    case SingleSelection:
        if (!selectionButton->isChecked())
            selectionButton->click();
        selectionButton->setChecked(true);
        break;
    case RepeatableSelection:
        selectionButton->click();
        break;
    default:
        break;
    }
}

void BeatValueWidget::deselect()
{
    switch (selectionMode) {
    case SingleSelection:
        selectionButton->setChecked(false);
        break;
    default:
        break;
    }
}

void BeatValueWidget::on_selected(bool is_selected)
{
    if (is_selected || selectionMode == RepeatableSelection)
    {
        emit selected();
        emit selected(&value);
    }
}
