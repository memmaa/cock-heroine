#ifndef BUTTPLUGINTERFACE_H
#define BUTTPLUGINTERFACE_H

#include <QObject>
#include <QWebSocket>
#include "buttplugdevice.h"
#include <QJsonObject>

class ButtplugDeviceFeature;

class ButtplugInterface : public QObject
{
    Q_OBJECT
public:
    explicit ButtplugInterface(QObject *parent = nullptr);
    void openSocket(QString address = "ws://127.0.0.1:12345");
    void closeSocket();
    QVector<ButtplugDeviceFeature *> getAllFeatures();
    ButtplugDeviceFeature * getFeatureByName(const QString & name);

public slots:
    void serverConnected();
    void serverDisconnected();
    void serverError(QAbstractSocket::SocketError error);
    void processTextMessage(QString message);
    void processDataMessage(QByteArray message);
    void startScanning();
    void stopScanning();
    void stopAllDevices();
    void sendStopDeviceCmd(int deviceIndex);
    //!
    //! \brief sendVibrateCmd vibrates all the vibrators on...
    //! \param deviceIndex ..the specified device (by id) at...
    //! \param intensity ...the specified intensity (0.0-1.0).
    //!
    void sendVibrateCmdAll(int deviceIndex, double intensity);
    void sendVibrateCmd(int deviceIndex, QVector<double> intensities);
    void vibrateAll(double intensity);

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError error, QString message);
    void deviceAdded(int deviceIndex);
    void deviceRemoved(int deviceIndex);

private:
    void sendTextMessage(QString message);
    int getNewRequestId();
    void registerRequestSent(unsigned int id, QJsonObject request);
    QJsonObject registerResponseReceived(unsigned int id);
    void handleOk(QJsonObject payload);
    void handleError(QJsonObject payload);
    QJsonObject createMessageWithPayload(QString messageType, QJsonObject payload);
    QJsonObject sendMessageWithPayload(QString messageType, QJsonObject payload);
    void sendPing();
    void sendHandshake();
    void handleServerInfo(QJsonObject payload);
    void handleScanningFinished();
    void requestDeviceList();
    void handleDeviceList(QJsonObject payload);
    void handleDeviceAdded(QJsonObject payload);
    void handleNewDeviceInfo(QJsonObject deviceJson);
    void handleDeviceRemoved(QJsonObject payload);

    QWebSocket socket;
    QMap<unsigned int, QJsonObject> activeRequests;
    //!
    //! \brief currentlyConnected are we currently connected to the server?
    //!
    bool currentlyConnected = false;
    //!
    //! \brief currentlyScanning are we currently scanning on bluetooth or the like?
    //!
    bool currentlyScanning = false;
    QMap<unsigned int, ButtplugDevice *> attachedDevicesById;
    QMap<QString, ButtplugDevice *> attachedDevicesByName;
};

#endif // BUTTPLUGINTERFACE_H
