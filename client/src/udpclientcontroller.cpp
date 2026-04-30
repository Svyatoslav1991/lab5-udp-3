#include "udpclientcontroller.h"

#include <QTimer>
#include <QUdpSocket>

UdpClientController::UdpClientController(QObject* parent)
    : QObject(parent)
    , socket_(new QUdpSocket(this))
    , timer_(new QTimer(this))
    , periodicPort_(0)
{
    connect(timer_, &QTimer::timeout,
            this, &UdpClientController::sendNextPeriodicDatagram_);
}

//--------------------------------------------------------------------------

bool UdpClientController::sendDatagram(const QByteArray& datagram,
                                       const QHostAddress& address,
                                       quint16 port)
{
    if (!validateSendingParams_(datagram, address, port)) {
        return false;
    }

    const qint64 bytesWritten = socket_->writeDatagram(datagram, address, port);

    if (bytesWritten == -1) {
        emit errorOccurred(QStringLiteral("Ошибка отправки дейтаграммы: %1")
                           .arg(socket_->errorString()));
        return false;
    }

    emit datagramSent(datagram, address, port, bytesWritten);
    return true;
}

//--------------------------------------------------------------------------

void UdpClientController::startPeriodicSending(DatagramFactory datagramFactory,
                                               const QHostAddress& address,
                                               quint16 port,
                                               int intervalMs)
{
    if (!datagramFactory) {
        emit errorOccurred(QStringLiteral("Не задан источник данных для периодической отправки."));
        return;
    }

    if (intervalMs <= 0) {
        emit errorOccurred(QStringLiteral("Интервал отправки должен быть больше нуля."));
        return;
    }

    datagramFactory_ = std::move(datagramFactory);
    periodicAddress_ = address;
    periodicPort_ = port;

    if (sendDatagram(datagramFactory_(), periodicAddress_, periodicPort_)) {
        timer_->start(intervalMs);
    }
}

//--------------------------------------------------------------------------

void UdpClientController::stopPeriodicSending()
{
    timer_->stop();
}

//--------------------------------------------------------------------------

bool UdpClientController::isPeriodicSendingActive() const
{
    return timer_->isActive();
}

//--------------------------------------------------------------------------

void UdpClientController::sendNextPeriodicDatagram_()
{
    if (!datagramFactory_) {
        emit errorOccurred(QStringLiteral("Не задан источник данных для периодической отправки."));
        stopPeriodicSending();
        return;
    }

    sendDatagram(datagramFactory_(), periodicAddress_, periodicPort_);
}

//--------------------------------------------------------------------------

bool UdpClientController::validateSendingParams_(const QByteArray& datagram,
                                                 const QHostAddress& address,
                                                 quint16 port)
{
    if (datagram.isEmpty()) {
        emit errorOccurred(QStringLiteral("Нельзя отправить пустую дейтаграмму."));
        return false;
    }

    if (address.isNull()) {
        emit errorOccurred(QStringLiteral("Некорректный адрес получателя."));
        return false;
    }

    if (port == 0) {
        emit errorOccurred(QStringLiteral("Некорректный порт получателя."));
        return false;
    }

    return true;
}
