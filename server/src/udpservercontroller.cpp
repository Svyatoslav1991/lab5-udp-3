#include "udpservercontroller.h"

#include <QAbstractSocket>
#include <QMetaType>
#include <QNetworkDatagram>
#include <QUdpSocket>

UdpServerController::UdpServerController(QObject* parent)
    : QObject(parent)
    , socket_(new QUdpSocket(this))
{
    qRegisterMetaType<UdpDatagramInfo>("UdpDatagramInfo");

    connect(socket_, &QUdpSocket::readyRead,
            this, &UdpServerController::readPendingDatagrams_);
}

//--------------------------------------------------------------------------

bool UdpServerController::startReceiving(const QHostAddress& address, quint16 port)
{
    if (isReceiving()) {
        emit errorOccurred(QStringLiteral("UDP-сервер уже запущен."));
        return false;
    }

    if (!validateBindParams_(address, port)) {
        return false;
    }

    if (!socket_->bind(address, port)) {
        emit errorOccurred(QStringLiteral("Не удалось привязать UDP-сокет к %1:%2. Ошибка: %3")
                           .arg(address.toString())
                           .arg(port)
                           .arg(socket_->errorString()));
        return false;
    }

    emit receivingStarted(socket_->localAddress(), socket_->localPort());
    return true;
}

//--------------------------------------------------------------------------

void UdpServerController::stopReceiving()
{
    if (!isReceiving()) {
        return;
    }

    socket_->close();
    emit receivingStopped();
}

//--------------------------------------------------------------------------

bool UdpServerController::isReceiving() const
{
    return socket_->state() == QAbstractSocket::BoundState;
}

//--------------------------------------------------------------------------

void UdpServerController::readPendingDatagrams_()
{
    while (socket_->hasPendingDatagrams()) {
        const QNetworkDatagram datagram = socket_->receiveDatagram();

        if (!datagram.isValid()) {
            emit errorOccurred(QStringLiteral("Получена некорректная UDP-дейтаграмма."));
            continue;
        }

        UdpDatagramInfo datagramInfo;
        datagramInfo.data = datagram.data();
        datagramInfo.senderAddress = datagram.senderAddress();

        const int senderPort = datagram.senderPort();
        datagramInfo.senderPort = senderPort > 0
                ? static_cast<quint16>(senderPort)
                : 0;

        emit datagramReceived(datagramInfo);
    }
}

//--------------------------------------------------------------------------

bool UdpServerController::validateBindParams_(const QHostAddress& address, quint16 port)
{
    if (address.isNull()) {
        emit errorOccurred(QStringLiteral("Некорректный адрес привязки."));
        return false;
    }

    if (port == 0) {
        emit errorOccurred(QStringLiteral("Некорректный порт привязки."));
        return false;
    }

    return true;
}
