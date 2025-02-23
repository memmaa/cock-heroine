#ifndef BUTTPLUGDEVICECONFIGDIALOG_H
#define BUTTPLUGDEVICECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ButtplugDeviceConfigDialog;
}

class VibratorPulseFeatureParams;
class ButtplugFeatureParams;
class ButtplugDeviceFeature;

class ButtplugDeviceConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ButtplugDeviceConfigDialog(QWidget *parent = nullptr);
    ~ButtplugDeviceConfigDialog();
    static QVector<ButtplugFeatureParams *> entries;
    static void readInConfigs();
    static VibratorPulseFeatureParams *getDefaultVibratorParams();
    void writeOutConfigs();

public slots:
    void refreshFeatureList();
    void saveAndClose();
    void deleteParams(ButtplugFeatureParams *);

private slots:
    void on_addButton_clicked();

private:
    Ui::ButtplugDeviceConfigDialog *ui;
    QHash<ButtplugFeatureParams *, QWidget *> nonDefaultPanels;
    void createNewVibratorParams(ButtplugDeviceFeature * feature);
};

#endif // BUTTPLUGDEVICECONFIGDIALOG_H
