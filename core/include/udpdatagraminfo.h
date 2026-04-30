#ifndef UDPDATAGRAMINFO_H
#define UDPDATAGRAMINFO_H

#include <QByteArray>
#include <QHostAddress>
#include <QMetaType>

/*!
 * \struct UdpDatagramInfo
 * \brief DTO с информацией о принятой UDP-дейтаграмме.
 *
 * \details
 * Структура используется для передачи данных из UdpServerController
 * в слой интерфейса без привязки контроллера к MainWindow.
 */
struct UdpDatagramInfo
{
    /*!
     * \brief Полезная нагрузка UDP-дейтаграммы.
     */
    QByteArray data;

    /*!
     * \brief IP-адрес отправителя дейтаграммы.
     */
    QHostAddress senderAddress;

    /*!
     * \brief UDP-порт отправителя дейтаграммы.
     */
    quint16 senderPort = 0;
};

Q_DECLARE_METATYPE(UdpDatagramInfo)

#endif // UDPDATAGRAMINFO_H
