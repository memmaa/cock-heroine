#ifndef ENABLEIDENTIFYINTERVALSDIALOG_H
#define ENABLEIDENTIFYINTERVALSDIALOG_H

#include <QDialog>

class AbstractNewBeatValueWidget;
class NewCustomBeatValueWidget;
class BeatValue;

namespace Ui {
class EnableIdentifyIntervalsDialog;
}

class EnableIdentifyIntervalsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnableIdentifyIntervalsDialog(int valueInMs, double currentTempo, QWidget *parent = nullptr);
    ~EnableIdentifyIntervalsDialog();

public slots:
    void accept() override;

private slots:
    void on_tempoSpinBox_valueChanged(double arg1);
    void on_intervalNumeratorSpinBox_valueChanged(int);
    AbstractNewBeatValueWidget * selectBestMatch();
    void ensureSingleSelection();
    void newValueSelected(AbstractNewBeatValueWidget *);

    void on_intervalNameEdit_textChanged(const QString &arg1);

    void on_originalTempoButton_clicked();

    void on_perfectMatchTempoButton_clicked();

    void on_minimumNecessaryChangeTempoButton_clicked();

    void on_updateTempoCheckBox_stateChanged(int arg1);

    void on_openAdjustDialogCheckBox_stateChanged(int);

    void on_openAdjustDialogCheckBox_clicked();

private:
    Ui::EnableIdentifyIntervalsDialog *ui;
    int valueInMs;
    double tempo;
    const double originalTempo;
    double lastNonOriginalTempo;
    double valueInBeats;
    QVector<AbstractNewBeatValueWidget *> valueWidgets;
    NewCustomBeatValueWidget * customValueWidget;
    AbstractNewBeatValueWidget * currentlySelectedWidget;
    void calculateValueInBeats();
    double getSelectedValueInBeats();
    void updateDescriptiveText(AbstractNewBeatValueWidget *);
    void updateCheckboxes();
    static BeatValue * getExistingMatchingValue(float value);
    static bool hasExistingMatchingValue(float value);
    BeatValue getSelectedValue();
    BeatValue createSelectedValue();
    double getPerfectMatchTempo();
    QVector<BeatValue> getMaskingValues();
    QString getMaskingValuesString();
    bool selectedValueIsMasked();
    bool tempoChangedLots();
    QString askAboutBadMatch();
    bool askAboutBigTempoChange();
    bool confirmChoiceToDisableMaskingValues();
    QString askAboutMaskingValues();
};

#endif // ENABLEIDENTIFYINTERVALSDIALOG_H
