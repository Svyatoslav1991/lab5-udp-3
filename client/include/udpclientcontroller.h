#ifndef UDPCLIENTCONTROLLER_H
#define UDPCLIENTCONTROLLER_H

#include <QObject>
#include <QHostAddress>
#include <QString>

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
 * Класс не зависит от MainWindow и не управляет элементами интерфейса напрямую.
 */
class UdpClientController : public QObject
{
    Q_OBJECT

public:
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
     * \param datagram Данные дейтаграммы.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \param intervalMs Интервал отправки в миллисекундах.
     */
    void startPeriodicSending(const QByteArray& datagram,
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
     * \brief Отправляет сохранённую дейтаграмму по таймеру.
     */
    void sendStoredDatagram_();

private:
    QUdpSocket* socket_;
    QTimer* timer_;

    QByteArray storedDatagram_;
    QHostAddress storedAddress_;
    quint16 storedPort_;
};

#endif // UDPCLIENTCONTROLLER_H
