#include "buttplugdevice.h"
#include <QVector>
#include "buttplugdevicefeature.h"
#include "buttpluginterface.h"

int ButtplugDevice::eventCollationTime = 1;

ButtplugDevice::ButtplugDevice(QObject * parent)
    :
    QObject(parent)
{

}

ButtplugDevice::ButtplugDevice(ButtplugInterface * parent)
    :
    QObject(parent),
    parent(parent),
    name(""),
    index(0),
    supportsStop(false),
    supportsVibrate(false),
    numVibrators(0),
    vibrationLevels(0),
    supportsLinear(false),
    numLinearAxes(0),
    supportsRotate(false),
    numRotationalAxes(0),
    supportsBatteryLevel(false),
    supportsSignalStrength(false),
    vibrateCmdTimer(this)
{
}

ButtplugDevice::ButtplugDevice(ButtplugInterface * parent,
                               QString name,
                               int index,
                               bool supportsStop,
                               QVector<int> vibratorLevels,
                               int linearAxes,
                               int rotaionalAxes,
                               bool supportsBatteryLevel,
                               bool supportsSignalStrength)
    :
    QObject(parent),
    parent(parent),
    name(name),
    index(index),
    supportsStop(supportsStop),
    supportsVibrate(!vibratorLevels.isEmpty()),
    numVibrators(vibratorLevels.size()),
    vibrationLevels(vibratorLevels),
    supportsLinear(linearAxes > 0),
    numLinearAxes(linearAxes),
    supportsRotate(rotaionalAxes > 0),
    numRotationalAxes(rotaionalAxes),
    supportsBatteryLevel(supportsBatteryLevel),
    supportsSignalStrength(supportsSignalStrength),
    vibrateCmdTimer(this)
{
    populateFeatureList();
    vibrateCmdTimer.setSingleShot(true);
    vibrateCmdTimer.setTimerType(Qt::PreciseTimer);
    connect(&vibrateCmdTimer, &QTimer::timeout, this, &ButtplugDevice::sendVibrationCommand);
}

ButtplugDevice::~ButtplugDevice()
{

}

int ButtplugDevice::totalFeatures()
{
    return numVibrators + numLinearAxes + numRotationalAxes;
}

void ButtplugDevice::populateFeatureList()
{
    for (int index = 0; index < numVibrators; ++index)
    {
        currentVibratorIntensities.append(0);
        ButtplugDeviceFeature * feature = new ButtplugDeviceFeature(this, ButtplugDeviceFeatureType::VibratorMotor, index, vibrationLevels[index]);
        featureList.append(feature);
    }

    for (int index = 0; index < numLinearAxes; ++index)
    {
        ButtplugDeviceFeature * feature = new ButtplugDeviceFeature(this, ButtplugDeviceFeatureType::StrokerAxis, index);
        featureList.append(feature);
    }

    for (int index = 0; index < numRotationalAxes; ++index)
    {
        currentRotationalIntensities.append(0);
        ButtplugDeviceFeature * feature = new ButtplugDeviceFeature(this, ButtplugDeviceFeatureType::RotatorMotor, index);
        featureList.append(feature);
    }
}

void ButtplugDevice::setVibration(int idx, double intensity)
{
    currentVibratorIntensities[idx] = intensity;
    vibrateCmdTimer.start(eventCollationTime);
}

void ButtplugDevice::sendVibrationCommand()
{
    parent->sendVibrateCmd(index, currentVibratorIntensities);
}
