#ifndef UDPCLIENTCONTROLLER_H
#define UDPCLIENTCONTROLLER_H

#include <QByteArray>
#include <QHostAddress>
#include <QObject>
#include <QString>

#include <functional>

class QTimer;
class QUdpSocket;

/*!
 * \class UdpClientController
 * \brief Контроллер UDP-клиента.
 *
 * \details
 * Класс инкапсулирует сетевую UDP-логику клиента:
 * - отправку одной дейтаграммы;
 * - периодическую отправку дейтаграмм по таймеру;
 * - уведомление GUI об успешной отправке или ошибке.
 *
 * Контроллер не зависит от MainWindow и не управляет элементами интерфейса.
 */
class UdpClientController : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Функция, формирующая данные очередной UDP-дейтаграммы.
     *
     * \details
     * Используется при периодической отправке. Это позволяет при каждом
     * срабатывании таймера получать новые данные, например новое текущее время.
     */
    using DatagramFactory = std::function<QByteArray()>;

    /*!
     * \brief Создаёт контроллер UDP-клиента.
     * \param parent Родительский QObject.
     */
    explicit UdpClientController(QObject* parent = nullptr);

    /*!
     * \brief Отправляет одну UDP-дейтаграмму.
     * \param datagram Данные дейтаграммы.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \return true, если дейтаграмма была передана в сокет; иначе false.
     */
    bool sendDatagram(const QByteArray& datagram,
                      const QHostAddress& address,
                      quint16 port);

    /*!
     * \brief Запускает периодическую отправку UDP-дейтаграмм.
     * \param datagramFactory Функция формирования очередной дейтаграммы.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \param intervalMs Интервал отправки в миллисекундах.
     */
    void startPeriodicSending(DatagramFactory datagramFactory,
                              const QHostAddress& address,
                              quint16 port,
                              int intervalMs);

    /*!
     * \brief Останавливает периодическую отправку UDP-дейтаграмм.
     */
    void stopPeriodicSending();

    /*!
     * \brief Проверяет, активна ли периодическая отправка.
     * \return true, если таймер периодической отправки активен.
     */
    bool isPeriodicSendingActive() const;

signals:
    /*!
     * \brief Сигнал успешной отправки дейтаграммы.
     * \param datagram Отправленные данные.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \param bytesWritten Количество записанных байт.
     */
    void datagramSent(const QByteArray& datagram,
                      const QHostAddress& address,
                      quint16 port,
                      qint64 bytesWritten);

    /*!
     * \brief Сигнал ошибки UDP-клиента.
     * \param message Текст ошибки.
     */
    void errorOccurred(const QString& message);

private slots:
    /*!
     * \brief Отправляет очередную дейтаграмму по таймеру.
     */
    void sendNextPeriodicDatagram_();

private:
    /*!
     * \brief Проверяет параметры отправки.
     * \param datagram Данные дейтаграммы.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \return true, если параметры корректны.
     */
    bool validateSendingParams_(const QByteArray& datagram,
                                const QHostAddress& address,
                                quint16 port);

private:
    QUdpSocket* socket_;
    QTimer* timer_;

    DatagramFactory datagramFactory_;
    QHostAddress periodicAddress_;
    quint16 periodicPort_;
};

#endif // UDPCLIENTCONTROLLER_H
