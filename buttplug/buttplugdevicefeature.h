#ifndef BUTTPLUGDEVICEFEATURE_H
#define BUTTPLUGDEVICEFEATURE_H

#include "buttplugdevice.h"

#include "event.h"
class ButtplugDevice;
class ButtplugFeatureParams;

enum ButtplugDeviceFeatureType
{
    VibratorMotor,
    StrokerAxis,
    RotatorMotor
};

class ButtplugDeviceFeature : public QObject
{
    Q_OBJECT

public:
    ButtplugDeviceFeature();
    ButtplugDeviceFeature(ButtplugDevice *device, ButtplugDeviceFeatureType type, int index, int featureLevels = 1);
    ~ButtplugDeviceFeature();

    ButtplugDevice * device;
    ButtplugDeviceFeatureType type;
    int index;
    int levels;

    QString getName();
    void dispatch(Event event);
    ButtplugFeatureParams * getParamsFor(Event event);
    void setVibration(double level);
};

#endif // BUTTPLUGDEVICEFEATURE_H
