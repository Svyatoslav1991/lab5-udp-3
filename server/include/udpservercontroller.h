#ifndef UDPSERVERCONTROLLER_H
#define UDPSERVERCONTROLLER_H

#include "udpdatagraminfo.h"

#include <QHostAddress>
#include <QObject>
#include <QString>

class QUdpSocket;

/*!
 * \class UdpServerController
 * \brief Контроллер UDP-сервера.
 *
 * \details
 * Класс инкапсулирует сетевую UDP-логику сервера:
 * - привязку QUdpSocket к адресу и порту;
 * - остановку приёма;
 * - чтение входящих UDP-дейтаграмм;
 * - передачу информации о принятых дейтаграммах в GUI через сигналы.
 *
 * Контроллер не зависит от MainWindow и не управляет элементами интерфейса.
 */
class UdpServerController : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Создаёт контроллер UDP-сервера.
     * \param parent Родительский QObject.
     */
    explicit UdpServerController(QObject* parent = nullptr);

    /*!
     * \brief Запускает приём UDP-дейтаграмм.
     * \param address Локальный адрес привязки.
     * \param port Локальный порт привязки.
     * \return true, если сокет успешно привязан к адресу и порту.
     */
    bool startReceiving(const QHostAddress& address, quint16 port);

    /*!
     * \brief Останавливает приём UDP-дейтаграмм.
     */
    void stopReceiving();

    /*!
     * \brief Проверяет, активен ли UDP-сервер.
     * \return true, если сокет находится в состоянии BoundState.
     */
    bool isReceiving() const;

signals:
    /*!
     * \brief Сигнал успешного запуска приёма.
     * \param address Фактический локальный адрес сокета.
     * \param port Фактический локальный порт сокета.
     */
    void receivingStarted(const QHostAddress& address, quint16 port);

    /*!
     * \brief Сигнал остановки приёма.
     */
    void receivingStopped();

    /*!
     * \brief Сигнал получения UDP-дейтаграммы.
     * \param datagramInfo Информация о принятой дейтаграмме.
     */
    void datagramReceived(const UdpDatagramInfo& datagramInfo);

    /*!
     * \brief Сигнал ошибки UDP-сервера.
     * \param message Текст ошибки.
     */
    void errorOccurred(const QString& message);

private slots:
    /*!
     * \brief Считывает все ожидающие UDP-дейтаграммы из сокета.
     */
    void readPendingDatagrams_();

private:
    /*!
     * \brief Проверяет параметры запуска UDP-сервера.
     * \param address Адрес привязки.
     * \param port Порт привязки.
     * \return true, если параметры корректны.
     */
    bool validateBindParams_(const QHostAddress& address, quint16 port);

private:
    QUdpSocket* socket_;
};

#endif // UDPSERVERCONTROLLER_H
