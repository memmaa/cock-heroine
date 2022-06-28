#include "buttpluginterface.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include "buttplugdevicefeature.h"

ButtplugInterface::ButtplugInterface(QObject *parent) : QObject(parent)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(serverConnected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(serverDisconnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(serverError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(textMessageReceived(QString)), this, SLOT(processTextMessage(QString)));
    connect(&socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(processDataMessage(QByteArray)));
}

void ButtplugInterface::openSocket(QString address)
{
    QUrl serverLocation(address);
    socket.open(serverLocation);
}

void ButtplugInterface::closeSocket()
{
    socket.close(QWebSocketProtocol::CloseCodeNormal, "User disconnected");
}

QVector<ButtplugDeviceFeature *> ButtplugInterface::getAllFeatures()
{
    QVector<ButtplugDeviceFeature *> list;
    for (ButtplugDevice * device : attachedDevicesById)
    {
        list.append(device->featureList);
    }
    return list;
}

ButtplugDeviceFeature *ButtplugInterface::getFeatureByName(const QString &name)
{
    auto list = getAllFeatures();
    for (auto feature : list)
        if (feature->getName() == name)
            return feature;
    return nullptr;
}

void ButtplugInterface::serverConnected()
{
    qDebug() << "It's allliiiiiivvve!! (sending handshake)";
    currentlyConnected = true;
    sendHandshake();
}

void ButtplugInterface::serverDisconnected()
{
    qDebug() << "I'm sorry, he's dead Jim.";
    currentlyConnected = false;
}

void ButtplugInterface::serverError(QAbstractSocket::SocketError error)
{
    qDebug() << "Oh no! There was error: " << error;
}

void ButtplugInterface::sendTextMessage(QString message)
{
    /*int bytes = */socket.sendTextMessage(message);
//    qDebug() << "Sent " << bytes << " bytes:";
//    qDebug() << message;
}

void ButtplugInterface::processDataMessage(QByteArray message)
{
    qDebug() << "We got Data (we don't handle this): " << message;
}

void ButtplugInterface::processTextMessage(QString message)
{
//    qDebug() << "We got text (handle this): " << message;
    QJsonParseError * err = NULL;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), err);
    if (jsonDoc.isNull())
    {
        qDebug() << "Parse error when handling server response: " << err;
        return;
    }
    if (!jsonDoc.isArray())
    {
        qDebug() << "Server response was not an array. Expected an array.";
        return;
    }
    for (QJsonValueRef arrayItem : jsonDoc.array())
    {
        if (!arrayItem.isObject())
        {
            qDebug() << "Array item was not an object. Expected an object.";
            continue;
        }
        if (arrayItem.toObject().contains("ServerInfo"))
        {
            handleServerInfo(arrayItem.toObject()["ServerInfo"].toObject());
        }
        else if (arrayItem.toObject().contains("Ok"))
        {
            handleOk(arrayItem.toObject()["Ok"].toObject());
        }
        else if (arrayItem.toObject().contains("Error"))
        {
            handleError(arrayItem.toObject()["Error"].toObject());
        }
        else if (arrayItem.toObject().contains("ScanningFinished"))
        {
            handleScanningFinished();
        }
        else if (arrayItem.toObject().contains("DeviceList"))
        {
            handleDeviceList(arrayItem.toObject()["DeviceList"].toObject());
        }
        else if (arrayItem.toObject().contains("DeviceAdded"))
        {
            handleDeviceAdded(arrayItem.toObject()["DeviceAdded"].toObject());
        }
        else if (arrayItem.toObject().contains("DeviceRemoved"))
        {
            handleDeviceRemoved(arrayItem.toObject()["DeviceRemoved"].toObject());
        }
    }
}

int ButtplugInterface::getNewRequestId()
{
    static unsigned int reqId = 0;
    ++reqId;
    if (reqId == 0)
        ++reqId;
    return reqId;
}

void ButtplugInterface::registerRequestSent(unsigned int id, QJsonObject request)
{
    if (activeRequests.contains(id))
        qDebug() << "Warning: duplicate request id: " << id << ", disarding old request: " << activeRequests[id];
    if (id != 0)
        activeRequests.insert(id, request);
}

QJsonObject ButtplugInterface::registerResponseReceived(unsigned int id)
{
    if (id == 0)
    {
        return QJsonObject();
    }

    if (!activeRequests.contains(id))
    {
        qDebug() << "Warning: response with id " << id << " not expected";
        return QJsonObject();
    }

    return activeRequests.take(id);
}

void ButtplugInterface::handleOk(QJsonObject payload)
{
    int id = payload["Id"].toInt();
    registerResponseReceived(id);
//    qDebug() << "Request " << id << " OK! :-)";
}

void ButtplugInterface::handleError(QJsonObject payload)
{
    int id = payload["Id"].toInt();
    registerResponseReceived(id);
    qDebug() << "Oh dear! Request " << id << " had an error:";
    int errorCode = payload["ErrorCode"].toInt();
    QString errorMessage = payload["ErrorMessage"].toString();
    qDebug() << "Error " << errorCode << ": " << errorMessage;
}

QJsonObject ButtplugInterface::createMessageWithPayload(QString messageType, QJsonObject payload)
{
    QJsonObject message;
    message[messageType] = payload;
    return message;
}

QJsonObject ButtplugInterface::sendMessageWithPayload(QString messageType, QJsonObject payload)
{
    QJsonObject message = createMessageWithPayload(messageType, payload);
    QJsonArray messages;
    messages.append(message);
    QJsonDocument document(messages);
    sendTextMessage(document.toJson());
    return message;
}

void ButtplugInterface::sendPing()
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    QJsonObject message = sendMessageWithPayload("Ping", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::sendHandshake()
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    payload["ClientName"] = "Cock Heroine";
    payload["MessageVersion"] = 2;
    QJsonObject message = sendMessageWithPayload("RequestServerInfo", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::handleServerInfo(QJsonObject payload)
{
    registerResponseReceived(payload["Id"].toInt());
    qDebug() << "Connected to " << payload["ServerName"].toString();
    qDebug() << "Server supports protocol v" << payload["MessageVersion"].toInt();
    int maxPingTime = payload["MaxPingTime"].toInt();
    if (maxPingTime != 0)
    {
        qDebug() << "Warning: Server specified a minimum ping time. Cock Heronine doesn't support this yet, so this connection will probably not work.";
    }
    qDebug() << "Let's see what ya got!";
    requestDeviceList();
}

void ButtplugInterface::startScanning()
{
    if (!currentlyConnected)
    {
        qDebug() << "Attempted to start scanning when not connected to server. Bad.";
        return;
    }
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    QJsonObject message = sendMessageWithPayload("StartScanning", payload);
    registerRequestSent(id, message);
    qDebug() << "Scanning for upto 2 minutes...";
    currentlyScanning = true;
    //TODO: handle this in the 'Ok' message so we know it's actually scanning
    QTimer::singleShot(120 * 1000, this, SLOT(stopScanning()));
}

void ButtplugInterface::stopScanning()
{
    if (!currentlyScanning)
    {
        qDebug() << "Info: Scanning requested to stop when no scanning in progress.";
        return;
    }
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    QJsonObject message = sendMessageWithPayload("StopScanning", payload);
    registerRequestSent(id, message);
    qDebug() << "Finished scanning.";
    //TODO: handle this in the 'Ok' message so we know it's actually stopped scanning
    currentlyScanning = false;
}

//!
//! \brief ButtplugInterface::handleScanningFinished this method has no parameters because we don't need to do anything with the message
//!
void ButtplugInterface::handleScanningFinished()
{
    currentlyScanning = false;
}

void ButtplugInterface::requestDeviceList()
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    QJsonObject message = sendMessageWithPayload("RequestDeviceList", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::handleDeviceList(QJsonObject payload)
{
    attachedDevicesById.clear();
    registerResponseReceived(payload["Id"].toInt());
    QJsonArray devices = payload["Devices"].toArray();
    for (auto device : devices)
    {
        handleNewDeviceInfo(device.toObject());
    }
}

void ButtplugInterface::handleDeviceAdded(QJsonObject payload)
{
    //id should be 0, so don't need to register
    handleNewDeviceInfo(payload);
}

void ButtplugInterface::handleNewDeviceInfo(QJsonObject deviceJson)
{
    QString name = deviceJson["DeviceName"].toString();
//    device.name = name;
    int index = deviceJson["DeviceIndex"].toInt();
//    device.index = index;
    bool supportsStop = false;
    bool supportsBatteryLevel = false;
    bool supportsSignalStrength = false;
    QVector<int> vibrationLevels(0);
    int numLinearAxes = 0;
    int numRotationalAxes = 0;
    QJsonObject features = deviceJson["DeviceMessages"].toObject();
    for (QString feature : features.keys())
    {
        if (feature == "StopDeviceCmd")
            supportsStop = true;
        else if (feature == "BatteryLevelCmd")
            supportsBatteryLevel = true;
        else if (feature == "RSSILevelCmd")
            supportsSignalStrength = true;
        else if (feature == "VibrateCmd" ||
                 feature == "LinearCmd" ||
                 feature == "RotateCmd")
        {
            QJsonObject featureAttributes = features[feature].toObject();
            if (feature == "VibrateCmd")
            {
                for (QString attribute : featureAttributes.keys())
                {
                    if (attribute == "FeatureCount")
                    {
                        if (vibrationLevels.isEmpty())
                        {
                            vibrationLevels.resize(featureAttributes["FeatureCount"].toInt());
                            vibrationLevels.fill(1);
                        }
                    }
                    else if (attribute == "StepCount")
                    {
                        QJsonValue countsVal = featureAttributes["StepCount"];
                        if (!countsVal.isArray())
                        {
                            qDebug() << "Expected array for StepCount attribute, got " << countsVal;
                            break;
                        }
                        QJsonArray array = countsVal.toArray();
                        for (int i = 0; i < array.size(); ++i)
                            vibrationLevels[i] = array[i].toInt();
                    }
                    else
                        qDebug() << "Not sure what to do with " << attribute << " attribute of " << feature << " feature on " << name << ": " << featureAttributes[attribute];
                }
            }
            else if (feature == "LinearCmd")
            {
                for (QString attribute : featureAttributes.keys())
                {
                    if (attribute == "FeatureCount")
                        numLinearAxes = featureAttributes["FeatureCount"].toInt();
                    else
                        qDebug() << "Not sure what to do with " << attribute << " attribute of " << feature << " feature on " << name << ": " << featureAttributes[attribute];
                }
            }
            else if (feature == "RotateCmd")
            {
                for (QString attribute : featureAttributes.keys())
                {
                    if (attribute == "FeatureCount")
                        numRotationalAxes = featureAttributes["FeatureCount"].toInt();
                    else
                        qDebug() << "Not sure what to do with " << attribute << " attribute of " << feature << " feature on " << name << ": " << featureAttributes[attribute];
                }
            }
        }
        else
        {
            qDebug() << "Not sure what to do with " << feature << " on " << name << ": " << features[feature];
        }
    }

    ButtplugDevice * device = new ButtplugDevice(this,name,index,supportsStop,vibrationLevels,numLinearAxes,numRotationalAxes,supportsBatteryLevel,supportsSignalStrength);
    attachedDevicesById.insert(index,device);
    attachedDevicesByName.insert(name,device);
    qDebug() << name << " connected.";
    emit deviceAdded(index);
}

void ButtplugInterface::handleDeviceRemoved(QJsonObject payload)
{
    //id should be 0, so don't need to register
    int index = payload["DeviceIndex"].toInt();
    ButtplugDevice * device = attachedDevicesById[index];
    QString name = device->name;
    qDebug() << name << " disconnected";
    attachedDevicesByName.remove(name);
    attachedDevicesById.remove(index);
    device->deleteLater();
    emit deviceRemoved(index);
}

void ButtplugInterface::sendStopDeviceCmd(int deviceIndex)
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    payload["DeviceIndex"] = deviceIndex;
    QJsonObject message = sendMessageWithPayload("StopDeviceCmd", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::stopAllDevices()
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    QJsonObject message = sendMessageWithPayload("StopAllDevices", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::sendVibrateCmdAll(int deviceIndex, double intensity)
{
    if (!attachedDevicesById.contains(deviceIndex))
    {
        qDebug() << "Invalid device index specified: " << deviceIndex;
        return;
    }
    ButtplugDevice * device = attachedDevicesById[deviceIndex];
    int numberOfVibrators = device->numVibrators;
    QVector<double> intensities;
    for (int i = 0; i < numberOfVibrators; ++i)
        intensities.append(intensity);
    sendVibrateCmd(deviceIndex, intensities);
}

void ButtplugInterface::sendVibrateCmd(int deviceIndex, QVector<double> intensities)
{
    QJsonObject payload;
    int id = getNewRequestId();
    payload["Id"] = id;
    payload["DeviceIndex"] = deviceIndex;
    QJsonArray speeds;
    int i = 0;
    for (double intensity : intensities)
    {
        speeds.append(QJsonObject{
                          {"Index", i},
                          {"Speed", intensity}
                      });
        ++i;
    }
    payload["Speeds"] = speeds;
    QJsonObject message = sendMessageWithPayload("VibrateCmd", payload);
    registerRequestSent(id, message);
}

void ButtplugInterface::vibrateAll(double intensity)
{
    for (ButtplugDevice * device : attachedDevicesById)
    {
        if (device->supportsVibrate)
            sendVibrateCmdAll(device->index, intensity);
    }
}

