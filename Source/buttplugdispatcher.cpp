#include "buttplugdispatcher.h"
#include "buttplugfeatureparams.h"
//#include "vibratorpulsefeatureparams.h"
//#include "buttplugdeviceconfigdialog.h"

ButtplugDispatcher::ButtplugDispatcher(ButtplugInterface * nterface, QObject * parent)
    :
    EventDispatcher(parent),
    nterface(nterface)
{

}

void ButtplugDispatcher::dispatch(Event event)
{
    for (ButtplugDeviceFeature * feature : nterface->getAllFeatures())
        feature->dispatch(event);

//    float intensity = getIntensity(event);
//    nterface->vibrateAll(intensity);
//    QTimer::singleShot(180, nterface, SLOT(stopAllDevices()));
}

