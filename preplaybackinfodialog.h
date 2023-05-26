#ifndef PREPLAYBACKINFODIALOG_H
#define PREPLAYBACKINFODIALOG_H

#include <QDialog>

namespace Ui {
class PrePlaybackInfoDialog;
}

class PrePlaybackInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrePlaybackInfoDialog(QWidget *parent = nullptr);
    ~PrePlaybackInfoDialog();
    void displayList(QList<QString> list);

private:
    Ui::PrePlaybackInfoDialog *ui;
};

#endif // PREPLAYBACKINFODIALOG_H
