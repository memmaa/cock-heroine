#include "preplaybackinfodialog.h"
#include "ui_preplaybackinfodialog.h"

PrePlaybackInfoDialog::PrePlaybackInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrePlaybackInfoDialog)
{
    ui->setupUi(this);
}

PrePlaybackInfoDialog::~PrePlaybackInfoDialog()
{
    delete ui;
}

void PrePlaybackInfoDialog::displayList(QList<QString> list)
{
    QString labelText;
    for (int i = 0; i < list.length(); i++)
    {
        if (i)
            labelText += "\n";
        labelText += list[i];
    }
    ui->listLabel->setText(labelText);
}
