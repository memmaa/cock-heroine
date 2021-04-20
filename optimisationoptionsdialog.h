#ifndef OPTIMISATIONOPTIONSDIALOG_H
#define OPTIMISATIONOPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class OptimisationOptionsDialog;
}

class OptimisationOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptimisationOptionsDialog(QWidget *parent = 0);
    ~OptimisationOptionsDialog();

private slots:
    void on_automaticOptimisationRadioButton_clicked();

    void on_useProvidedOutputTempoRadioButton_clicked();

    void on_manualStartTimestampCheckBox_toggled(bool checked);

    void on_buttons_accepted();

    void on_roundBPMCheckBox_clicked(bool checked);

private:
    Ui::OptimisationOptionsDialog *ui;
    QSettings settings;
};

#endif // OPTIMISATIONOPTIONSDIALOG_H
