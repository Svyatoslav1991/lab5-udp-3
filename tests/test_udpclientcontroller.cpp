#include <QtTest>

#include "udpclientcontroller.h"

#include <QHostAddress>
#include <QNetworkDatagram>
#include <QSignalSpy>
#include <QUdpSocket>

Q_DECLARE_METATYPE(QHostAddress)

/*!
 * \class TestUdpClientController
 * \brief Тесты контроллера UDP-клиента.
 *
 * \details
 * Проверяет:
 * - успешную отправку одной UDP-дейтаграммы;
 * - получение дейтаграммы локальным QUdpSocket;
 * - сигналы успешной отправки и ошибок;
 * - валидацию входных параметров;
 * - периодическую отправку через DatagramFactory;
 * - остановку периодической отправки.
 */
class TestUdpClientController : public QObject
{
    Q_OBJECT

private slots:
    /*!
     * \brief Регистрирует типы, которые используются в сигналах.
     */
    void initTestCase();

    /*!
     * \brief Проверяет успешную отправку UDP-дейтаграммы на локальный сокет.
     */
    void sendDatagram_sendsDataToReceiverAndEmitsDatagramSent();

    /*!
     * \brief Проверяет ошибку при отправке пустой дейтаграммы.
     */
    void sendDatagram_withEmptyDatagram_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет ошибку при отправке на некорректный адрес.
     */
    void sendDatagram_withNullAddress_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет ошибку при отправке на нулевой порт.
     */
    void sendDatagram_withZeroPort_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет, что периодическая отправка не запускается без фабрики дейтаграмм.
     */
    void startPeriodicSending_withoutFactory_emitsErrorAndDoesNotStartTimer();

    /*!
     * \brief Проверяет, что периодическая отправка не запускается с некорректным интервалом.
     */
    void startPeriodicSending_withInvalidInterval_emitsErrorAndDoesNotStartTimer();

    /*!
     * \brief Проверяет, что периодическая отправка не запускается, если первая дейтаграмма некорректна.
     */
    void startPeriodicSending_withInvalidFirstDatagram_emitsErrorAndDoesNotStartTimer();

    /*!
     * \brief Проверяет, что DatagramFactory вызывается для каждой периодической отправки.
     */
    void startPeriodicSending_usesFactoryForEveryDatagram();

    /*!
     * \brief Проверяет остановку активной периодической отправки.
     */
    void stopPeriodicSending_stopsActiveTimer();

private:
    /*!
     * \brief Привязывает UDP-сокет-приёмник к localhost и свободному порту.
     * \param receiver Сокет-приёмник.
     * \param port Сюда будет записан выбранный локальный порт.
     * \return true, если привязка прошла успешно.
     */
    bool bindLocalReceiver_(QUdpSocket& receiver, quint16* port) const;
};

//--------------------------------------------------------------------------

void TestUdpClientController::initTestCase()
{
    qRegisterMetaType<QByteArray>("QByteArray");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<quint16>("quint16");
    qRegisterMetaType<qint64>("qint64");
}

//--------------------------------------------------------------------------

void TestUdpClientController::sendDatagram_sendsDataToReceiverAndEmitsDatagramSent()
{
    QUdpSocket receiver;
    quint16 receiverPort = 0;

    QVERIFY2(bindLocalReceiver_(receiver, &receiverPort),
             qPrintable(receiver.errorString()));

    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const QByteArray payload = QByteArrayLiteral("Hello UDP");

    const bool result = controller.sendDatagram(payload,
                                                QHostAddress(QHostAddress::LocalHost),
                                                receiverPort);

    QVERIFY(result);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(sentSpy.count(), 1);

    QTRY_VERIFY_WITH_TIMEOUT(receiver.hasPendingDatagrams(), 1000);

    const QNetworkDatagram receivedDatagram = receiver.receiveDatagram();

    QVERIFY(receivedDatagram.isValid());
    QCOMPARE(receivedDatagram.data(), payload);

    const QList<QVariant> arguments = sentSpy.takeFirst();

    QCOMPARE(arguments.at(0).toByteArray(), payload);
    QCOMPARE(arguments.at(1).value<QHostAddress>(), QHostAddress(QHostAddress::LocalHost));
    QCOMPARE(arguments.at(2).toUInt(), static_cast<uint>(receiverPort));
    QCOMPARE(arguments.at(3).toLongLong(), static_cast<qint64>(payload.size()));
}

//--------------------------------------------------------------------------

void TestUdpClientController::sendDatagram_withEmptyDatagram_returnsFalseAndEmitsError()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.sendDatagram(QByteArray(),
                                                QHostAddress(QHostAddress::LocalHost),
                                                50000);

    QVERIFY(!result);
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::sendDatagram_withNullAddress_returnsFalseAndEmitsError()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.sendDatagram(QByteArrayLiteral("payload"),
                                                QHostAddress(),
                                                50000);

    QVERIFY(!result);
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::sendDatagram_withZeroPort_returnsFalseAndEmitsError()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.sendDatagram(QByteArrayLiteral("payload"),
                                                QHostAddress(QHostAddress::LocalHost),
                                                0);

    QVERIFY(!result);
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::startPeriodicSending_withoutFactory_emitsErrorAndDoesNotStartTimer()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    controller.startPeriodicSending(UdpClientController::DatagramFactory(),
                                    QHostAddress(QHostAddress::LocalHost),
                                    50000,
                                    100);

    QVERIFY(!controller.isPeriodicSendingActive());
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::startPeriodicSending_withInvalidInterval_emitsErrorAndDoesNotStartTimer()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    controller.startPeriodicSending(
        []() {
            return QByteArrayLiteral("payload");
        },
        QHostAddress(QHostAddress::LocalHost),
        50000,
        0);

    QVERIFY(!controller.isPeriodicSendingActive());
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::startPeriodicSending_withInvalidFirstDatagram_emitsErrorAndDoesNotStartTimer()
{
    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    controller.startPeriodicSending(
        []() {
            return QByteArray();
        },
        QHostAddress(QHostAddress::LocalHost),
        50000,
        100);

    QVERIFY(!controller.isPeriodicSendingActive());
    QCOMPARE(sentSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();
    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpClientController::startPeriodicSending_usesFactoryForEveryDatagram()
{
    QUdpSocket receiver;
    quint16 receiverPort = 0;

    QVERIFY2(bindLocalReceiver_(receiver, &receiverPort),
             qPrintable(receiver.errorString()));

    UdpClientController controller;

    QSignalSpy sentSpy(&controller, &UdpClientController::datagramSent);
    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(sentSpy.isValid());
    QVERIFY(errorSpy.isValid());

    int counter = 0;

    controller.startPeriodicSending(
        [&counter]() {
            ++counter;
            return QStringLiteral("packet_%1").arg(counter).toUtf8();
        },
        QHostAddress(QHostAddress::LocalHost),
        receiverPort,
        50);

    QVERIFY(controller.isPeriodicSendingActive());

    QTRY_VERIFY_WITH_TIMEOUT(sentSpy.count() >= 3, 1000);

    controller.stopPeriodicSending();

    QVERIFY(!controller.isPeriodicSendingActive());
    QCOMPARE(errorSpy.count(), 0);

    const QByteArray firstPayload = sentSpy.at(0).at(0).toByteArray();
    const QByteArray secondPayload = sentSpy.at(1).at(0).toByteArray();
    const QByteArray thirdPayload = sentSpy.at(2).at(0).toByteArray();

    QCOMPARE(firstPayload, QByteArrayLiteral("packet_1"));
    QCOMPARE(secondPayload, QByteArrayLiteral("packet_2"));
    QCOMPARE(thirdPayload, QByteArrayLiteral("packet_3"));

    QVERIFY(firstPayload != secondPayload);
    QVERIFY(secondPayload != thirdPayload);

    QTRY_VERIFY_WITH_TIMEOUT(receiver.hasPendingDatagrams(), 1000);

    QVector<QByteArray> receivedPayloads;

    while (receiver.hasPendingDatagrams()) {
        const QNetworkDatagram datagram = receiver.receiveDatagram();

        if (datagram.isValid()) {
            receivedPayloads.push_back(datagram.data());
        }
    }

    QVERIFY(receivedPayloads.contains(QByteArrayLiteral("packet_1")));
    QVERIFY(receivedPayloads.contains(QByteArrayLiteral("packet_2")));
    QVERIFY(receivedPayloads.contains(QByteArrayLiteral("packet_3")));
}

//--------------------------------------------------------------------------

void TestUdpClientController::stopPeriodicSending_stopsActiveTimer()
{
    QUdpSocket receiver;
    quint16 receiverPort = 0;

    QVERIFY2(bindLocalReceiver_(receiver, &receiverPort),
             qPrintable(receiver.errorString()));

    UdpClientController controller;

    QSignalSpy errorSpy(&controller, &UdpClientController::errorOccurred);

    QVERIFY(errorSpy.isValid());

    controller.startPeriodicSending(
        []() {
            return QByteArrayLiteral("payload");
        },
        QHostAddress(QHostAddress::LocalHost),
        receiverPort,
        100);

    QVERIFY(controller.isPeriodicSendingActive());

    controller.stopPeriodicSending();

    QVERIFY(!controller.isPeriodicSendingActive());
    QCOMPARE(errorSpy.count(), 0);
}

//--------------------------------------------------------------------------

bool TestUdpClientController::bindLocalReceiver_(QUdpSocket& receiver,
                                                 quint16* port) const
{
    if (port == nullptr) {
        return false;
    }

    const bool bindResult = receiver.bind(QHostAddress(QHostAddress::LocalHost),
                                          quint16(0));

    if (!bindResult) {
        return false;
    }

    if (receiver.localPort() == 0) {
        return false;
    }

    *port = receiver.localPort();
    return true;
}

//--------------------------------------------------------------------------

QTEST_MAIN(TestUdpClientController)

#include "test_udpclientcontroller.moc"
