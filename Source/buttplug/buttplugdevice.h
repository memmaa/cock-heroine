#ifndef BUTTPLUGDEVICE_H
#define BUTTPLUGDEVICE_H

#include <QTimer>

class ButtplugInterface;
class ButtplugDeviceFeature;

class ButtplugDevice : public QObject
{
    Q_OBJECT

public:
    ButtplugDevice(QObject * parent = nullptr);
    ButtplugDevice(ButtplugInterface * parent = nullptr);
    ButtplugDevice(ButtplugInterface * parent,
                   QString name,
                   int index,
                   bool supportsStop,
                   QVector<int> vibratorLevels,
                   int linearAxes,
                   int rotaionalAxes,
                   bool supportsBatteryLevel,
                   bool supportsSignalStrength);
    ~ButtplugDevice();

    static int eventCollationTime;

    ButtplugInterface * parent;
    QString name;

    int index;
    bool supportsStop;
    bool supportsVibrate;
    int numVibrators;
    QVector<int> vibrationLevels;
    bool supportsLinear;
    int numLinearAxes;
    bool supportsRotate;
    int numRotationalAxes;
    bool supportsBatteryLevel;
    bool supportsSignalStrength;

    QVector<ButtplugDeviceFeature *> featureList;
    int totalFeatures();
    void populateFeatureList();

    QVector<double> currentVibratorIntensities;
//    QVector<double> currentStrokerSpeeds; //needed?
    QVector<double> currentRotationalIntensities;

    void setVibration(int index, double intensity);
    void setRotation(int index, double intensity);
    QTimer vibrateCmdTimer;

private slots:
    void sendVibrationCommand();
};

#endif // BUTTPLUGDEVICE_H
