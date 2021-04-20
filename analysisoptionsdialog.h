#ifndef ANALYSISOPTIONSDIALOG_H
#define ANALYSISOPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QTime>
#include <QTimer>

namespace Ui {
class AnalysisOptionsDialog;
}

class AnalysisOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnalysisOptionsDialog(QWidget *parent = 0);
    ~AnalysisOptionsDialog();

private slots:
    void on_useProvidedTempoRadioButton_clicked();

    void on_automaticTempoRadioButton_clicked();

    void on_preferLongPatternsCheckBox_clicked();

    void on_buttons_accepted();

    void on_matchPatternMembersByActualValueCheckBox_clicked();

    void on_matchPatternMembersByNearestKnownValueCheckBox_clicked();

    void on_tapTempoInputButton_clicked();

    void onTempoTimeout();


    void on_tapTempoResetButton_clicked();

private:
    Ui::AnalysisOptionsDialog *ui;
    QSettings settings;
    short tapTempoCounter;
    float previousTempo;
    QTime * tapTempoTimer;
    QTimer * tempoTimeoutTimer;
};

#endif // ANALYSISOPTIONSDIALOG_H
