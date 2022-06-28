#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void populateUi();
    virtual void accept();
    void setControlsFromPreferences();
    void setPreferencesFromControls();
    static bool emitEstimSignal();
    static QString getEstimOutputDevice();
    static bool connectToHandy();
    static int handySyncBaseOffset();
    static int handyFullStrokeDuration();
    static int getStrokeLengthProportion();
    static QString getHandyApiBase();
    static bool useFullStrokes();
    static int getRangeTop();
    static int getRangeBottom();
    static int getRangeAnchor();
    static int getBeatMeterHeight();
    static int getBeatMeterWidth();
    static int getBeatMeterSpeed();
    static int getBeatMarkerDiameter();
    static double getBeatMeterFrameRate();

private slots:
    void on_startingIntensitySpinBox_valueChanged(int value);

    void on_endingIntensitySlider_valueChanged(int value);

    void on_endingIntensitySpinBox_valueChanged(int value);

    void on_startingIntensitySlider_valueChanged(int value);

    void on_obscureBottomScreenPortionWidthSlider_valueChanged(int value);

    void on_obscureBottomScreenPortionWidthSpinBox_valueChanged(double value);

    void on_connectToHandyCheckBox_stateChanged(int checkState);

    void on_syncOffsetSlider_valueChanged(int value);

    void on_syncOffsetSpinBox_valueChanged(int value);

    void on_bottomOfRangeSlider_valueChanged(int value);

    void on_bottomOfRangeSpinBox_valueChanged(int value);

    void on_topOfRangeSlider_valueChanged(int value);

    void on_topOfRangeSpinBox_valueChanged(int value);

private:
    Ui::OptionsDialog *ui;
    QWidget * triggeringWidget;
};

#endif // OPTIONSDIALOG_H
