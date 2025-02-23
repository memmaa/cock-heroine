#ifndef SEEKTOTIMECODEDIALOG_H
#define SEEKTOTIMECODEDIALOG_H

#include <QDialog>

namespace Ui {
class SeekToTimecodeDialog;
}

class SeekToTimecodeDialog : public QDialog
{
    Q_OBJECT

    friend class BetterLCDNumber;
public:
    explicit SeekToTimecodeDialog(QWidget *parent = 0);
    ~SeekToTimecodeDialog();

    void updateTimecode(int newTimecode);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SeekToTimecodeDialog *ui;
};

#endif // SEEKTOTIMECODEDIALOG_H
