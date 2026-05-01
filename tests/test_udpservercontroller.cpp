#include <QtTest>

#include "udpservercontroller.h"

#include <QHostAddress>
#include <QMetaType>
#include <QNetworkDatagram>
#include <QSignalSpy>
#include <QUdpSocket>

Q_DECLARE_METATYPE(QHostAddress)

/*!
 * \class TestUdpServerController
 * \brief Тесты контроллера UDP-сервера.
 *
 * \details
 * Проверяет:
 * - успешный запуск приёма UDP-дейтаграмм;
 * - остановку приёма;
 * - ошибки валидации параметров;
 * - защиту от повторного запуска;
 * - получение одной и нескольких UDP-дейтаграмм.
 */
class TestUdpServerController : public QObject
{
    Q_OBJECT

private slots:
    /*!
     * \brief Регистрирует типы, используемые в сигналах.
     */
    void initTestCase();

    /*!
     * \brief Проверяет успешный запуск UDP-сервера.
     */
    void startReceiving_withValidParams_startsServerAndEmitsReceivingStarted();

    /*!
     * \brief Проверяет остановку запущенного UDP-сервера.
     */
    void stopReceiving_whenServerIsRunning_stopsServerAndEmitsReceivingStopped();

    /*!
     * \brief Проверяет, что остановка незапущенного сервера не эмитит сигнал stopped.
     */
    void stopReceiving_whenServerIsNotRunning_doesNothing();

    /*!
     * \brief Проверяет ошибку запуска с некорректным адресом.
     */
    void startReceiving_withNullAddress_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет ошибку запуска с нулевым портом.
     */
    void startReceiving_withZeroPort_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет ошибку повторного запуска уже запущенного сервера.
     */
    void startReceiving_whenAlreadyRunning_returnsFalseAndEmitsError();

    /*!
     * \brief Проверяет получение одной UDP-дейтаграммы.
     */
    void datagramReceived_whenDatagramSent_emitsDatagramReceived();

    /*!
     * \brief Проверяет получение нескольких UDP-дейтаграмм.
     */
    void datagramReceived_whenSeveralDatagramsSent_emitsForEachDatagram();

private:
    /*!
     * \brief Находит свободный локальный UDP-порт.
     * \param port Сюда будет записан найденный порт.
     * \return true, если порт найден.
     */
    bool findFreeLocalUdpPort_(quint16* port) const;

    /*!
     * \brief Отправляет UDP-дейтаграмму на localhost.
     * \param port Порт получателя.
     * \param payload Данные дейтаграммы.
     * \return true, если отправка была успешно инициирована.
     */
    bool sendLocalDatagram_(quint16 port, const QByteArray& payload) const;
};

//--------------------------------------------------------------------------

void TestUdpServerController::initTestCase()
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<quint16>("quint16");
    qRegisterMetaType<UdpDatagramInfo>("UdpDatagramInfo");
}

//--------------------------------------------------------------------------

void TestUdpServerController::startReceiving_withValidParams_startsServerAndEmitsReceivingStarted()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    UdpServerController controller;

    QSignalSpy startedSpy(&controller, &UdpServerController::receivingStarted);
    QSignalSpy stoppedSpy(&controller, &UdpServerController::receivingStopped);
    QSignalSpy datagramSpy(&controller, &UdpServerController::datagramReceived);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(startedSpy.isValid());
    QVERIFY(stoppedSpy.isValid());
    QVERIFY(datagramSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                                  port);

    QVERIFY(result);
    QVERIFY(controller.isReceiving());

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(stoppedSpy.count(), 0);
    QCOMPARE(datagramSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);

    const QList<QVariant> arguments = startedSpy.takeFirst();

    QCOMPARE(arguments.at(0).value<QHostAddress>(),
             QHostAddress(QHostAddress::LocalHost));
    QCOMPARE(arguments.at(1).toUInt(), static_cast<uint>(port));
}

//--------------------------------------------------------------------------

void TestUdpServerController::stopReceiving_whenServerIsRunning_stopsServerAndEmitsReceivingStopped()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    UdpServerController controller;

    QSignalSpy startedSpy(&controller, &UdpServerController::receivingStarted);
    QSignalSpy stoppedSpy(&controller, &UdpServerController::receivingStopped);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(startedSpy.isValid());
    QVERIFY(stoppedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    QVERIFY(controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                      port));

    QVERIFY(controller.isReceiving());
    QCOMPARE(startedSpy.count(), 1);

    controller.stopReceiving();

    QVERIFY(!controller.isReceiving());
    QCOMPARE(stoppedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
}

//--------------------------------------------------------------------------

void TestUdpServerController::stopReceiving_whenServerIsNotRunning_doesNothing()
{
    UdpServerController controller;

    QSignalSpy stoppedSpy(&controller, &UdpServerController::receivingStopped);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(stoppedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    controller.stopReceiving();

    QVERIFY(!controller.isReceiving());
    QCOMPARE(stoppedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
}

//--------------------------------------------------------------------------

void TestUdpServerController::startReceiving_withNullAddress_returnsFalseAndEmitsError()
{
    UdpServerController controller;

    QSignalSpy startedSpy(&controller, &UdpServerController::receivingStarted);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(startedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.startReceiving(QHostAddress(), 50000);

    QVERIFY(!result);
    QVERIFY(!controller.isReceiving());

    QCOMPARE(startedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();

    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpServerController::startReceiving_withZeroPort_returnsFalseAndEmitsError()
{
    UdpServerController controller;

    QSignalSpy startedSpy(&controller, &UdpServerController::receivingStarted);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(startedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const bool result = controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                                  0);

    QVERIFY(!result);
    QVERIFY(!controller.isReceiving());

    QCOMPARE(startedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();

    QVERIFY(!errorMessage.isEmpty());
}

//--------------------------------------------------------------------------

void TestUdpServerController::startReceiving_whenAlreadyRunning_returnsFalseAndEmitsError()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    UdpServerController controller;

    QSignalSpy startedSpy(&controller, &UdpServerController::receivingStarted);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(startedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    QVERIFY(controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                      port));

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);

    const bool secondStartResult = controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                                             port);

    QVERIFY(!secondStartResult);
    QVERIFY(controller.isReceiving());

    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    const QString errorMessage = errorSpy.takeFirst().at(0).toString();

    QVERIFY(errorMessage.contains(QStringLiteral("уже запущен")));
}

//--------------------------------------------------------------------------

void TestUdpServerController::datagramReceived_whenDatagramSent_emitsDatagramReceived()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    UdpServerController controller;

    QSignalSpy datagramSpy(&controller, &UdpServerController::datagramReceived);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(datagramSpy.isValid());
    QVERIFY(errorSpy.isValid());

    QVERIFY(controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                      port));

    const QByteArray payload = QByteArrayLiteral("Hello server");

    QVERIFY(sendLocalDatagram_(port, payload));

    QTRY_COMPARE_WITH_TIMEOUT(datagramSpy.count(), 1, 1000);

    QCOMPARE(errorSpy.count(), 0);

    const QList<QVariant> arguments = datagramSpy.takeFirst();
    const UdpDatagramInfo datagramInfo = arguments.at(0).value<UdpDatagramInfo>();

    QCOMPARE(datagramInfo.data, payload);
    QVERIFY(!datagramInfo.senderAddress.isNull());
    QVERIFY(datagramInfo.senderPort != 0);
}

//--------------------------------------------------------------------------

void TestUdpServerController::datagramReceived_whenSeveralDatagramsSent_emitsForEachDatagram()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    UdpServerController controller;

    QSignalSpy datagramSpy(&controller, &UdpServerController::datagramReceived);
    QSignalSpy errorSpy(&controller, &UdpServerController::errorOccurred);

    QVERIFY(datagramSpy.isValid());
    QVERIFY(errorSpy.isValid());

    QVERIFY(controller.startReceiving(QHostAddress(QHostAddress::LocalHost),
                                      port));

    const QByteArray firstPayload = QByteArrayLiteral("first");
    const QByteArray secondPayload = QByteArrayLiteral("second");
    const QByteArray thirdPayload = QByteArrayLiteral("third");

    QVERIFY(sendLocalDatagram_(port, firstPayload));
    QVERIFY(sendLocalDatagram_(port, secondPayload));
    QVERIFY(sendLocalDatagram_(port, thirdPayload));

    QTRY_COMPARE_WITH_TIMEOUT(datagramSpy.count(), 3, 1000);

    QCOMPARE(errorSpy.count(), 0);

    QVector<QByteArray> receivedPayloads;

    for (int i = 0; i < datagramSpy.count(); ++i) {
        const UdpDatagramInfo datagramInfo =
                datagramSpy.at(i).at(0).value<UdpDatagramInfo>();

        receivedPayloads.push_back(datagramInfo.data);

        QVERIFY(!datagramInfo.senderAddress.isNull());
        QVERIFY(datagramInfo.senderPort != 0);
    }

    QVERIFY(receivedPayloads.contains(firstPayload));
    QVERIFY(receivedPayloads.contains(secondPayload));
    QVERIFY(receivedPayloads.contains(thirdPayload));
}

//--------------------------------------------------------------------------

bool TestUdpServerController::findFreeLocalUdpPort_(quint16* port) const
{
    if (port == nullptr) {
        return false;
    }

    QUdpSocket temporarySocket;

    const bool bindResult = temporarySocket.bind(QHostAddress(QHostAddress::LocalHost),
                                                 quint16(0));

    if (!bindResult) {
        return false;
    }

    if (temporarySocket.localPort() == 0) {
        return false;
    }

    *port = temporarySocket.localPort();
    temporarySocket.close();

    return true;
}

//--------------------------------------------------------------------------

bool TestUdpServerController::sendLocalDatagram_(quint16 port,
                                                 const QByteArray& payload) const
{
    QUdpSocket sender;

    const qint64 bytesWritten = sender.writeDatagram(payload,
                                                     QHostAddress(QHostAddress::LocalHost),
                                                     port);

    return bytesWritten == payload.size();
}

//--------------------------------------------------------------------------

QTEST_MAIN(TestUdpServerController)

#include "test_udpservercontroller.moc"
