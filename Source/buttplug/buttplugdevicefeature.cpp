#include "buttplugdevicefeature.h"
#include "buttplugfeatureparams.h"
#include "buttplugdeviceconfigdialog.h"
#include <QDebug>
#include <QTimer>

ButtplugDeviceFeature::ButtplugDeviceFeature(ButtplugDevice * device, ButtplugDeviceFeatureType type, int index, int featureLevels)
    :
    device(device),
    type(type),
    index(index),
    levels(featureLevels)
{
    //nothing here
}

ButtplugDeviceFeature::~ButtplugDeviceFeature()
{
    //or here
}

QString ButtplugDeviceFeature::getName()
{
    if (device->totalFeatures() == 1)
        return device->name;
    switch (type)
    {
    case VibratorMotor:
        if (device->numVibrators == 1)
            return QString("%1 (Vibrator)").arg(device->name);
        else
            return QString("%1 (Vibrator %2)").arg(device->name).arg(index + 1);
    case StrokerAxis:
        if (device->numLinearAxes == 1)
            return QString("%1 (Stroker)").arg(device->name);
        else
            return QString("%1 (Axis %2)").arg(device->name).arg(index + 1);
    case RotatorMotor:
        if (device->numRotationalAxes == 1)
            return QString("%1 (Rotation)").arg(device->name);
        else
            return QString("%1 (Rotator %2)").arg(device->name).arg(index + 1);
    }
    return QString("Wat.");
}

void ButtplugDeviceFeature::dispatch(Event event)
{
    ButtplugFeatureParams * params = getParamsFor(event);
    if (params == nullptr)
    {
        return;
    }

    params->handleEvent(event, this);
}

ButtplugFeatureParams * ButtplugDeviceFeature::getParamsFor(Event event)
{
    ButtplugFeatureParams * featureDefault = nullptr;
    ButtplugFeatureParams * deviceFeatureDefault = nullptr;
    ButtplugFeatureParams * specificFeatureEvent = nullptr;
    for (ButtplugFeatureParams * params : ButtplugDeviceConfigDialog::entries)
    {
        if (params->featureType == type) //ignore params for incompatible types of feature
        {
            if (params->isDefaultParams())
                featureDefault = params;
            else if (params->featureName == getName())
            {
                if (params->eventType == -1)
                    deviceFeatureDefault = params;
                else if (params->eventType == event.type())
                    specificFeatureEvent = params;
                //TODO: This is temporary - really params should have a type that is correct
                else
                    specificFeatureEvent = params;
            }
        }
    }
    if (specificFeatureEvent != nullptr)
        return specificFeatureEvent;
    else if (deviceFeatureDefault != nullptr)
        return deviceFeatureDefault;
    if (featureDefault == nullptr)
        qDebug() << "Oh dear... something went wrong: Can't find parameters for event " << event.toAsciiString() << " on feature " << getName();
    return featureDefault;
}

void ButtplugDeviceFeature::setVibration(double level)
{
    device->setVibration(index, level);
}
