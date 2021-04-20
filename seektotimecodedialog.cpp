#include "seektotimecodedialog.h"
#include "ui_seektotimecodedialog.h"

SeekToTimecodeDialog::SeekToTimecodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeekToTimecodeDialog)
{
    ui->setupUi(this);
}

SeekToTimecodeDialog::~SeekToTimecodeDialog()
{
    delete ui;
}

void SeekToTimecodeDialog::updateTimecode(int newTimecode)
{
    QTime tempTime = QTime(0,0,0,0).addMSecs(newTimecode);
    ui->timeEdit->setTime(tempTime);
}

void SeekToTimecodeDialog::on_buttonBox_accepted()
{
    QDialog::done( - ui->timeEdit->time().msecsTo(QTime(0,0,0,0)));
}

void SeekToTimecodeDialog::on_buttonBox_rejected()
{
    QDialog::done(-1);
}
