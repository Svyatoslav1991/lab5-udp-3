#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QtGlobal>

/*!
 * \struct UdpClientSettings
 * \brief Настройки UDP-клиента.
 */
struct UdpClientSettings
{
    /*!
     * \brief Адрес получателя UDP-дейтаграмм.
     */
    QString address;

    /*!
     * \brief Порт получателя UDP-дейтаграмм.
     */
    quint16 port = 50000;

    /*!
     * \brief Интервал периодической отправки в миллисекундах.
     */
    int intervalMs = 1000;

    /*!
     * \brief Флаг отправки текущего времени вместо ручного текста.
     */
    bool useCurrentTime = true;

    /*!
     * \brief Пользовательский текст дейтаграммы.
     */
    QString message;

    /*!
     * \brief Создаёт настройки UDP-клиента со значениями по умолчанию.
     */
    UdpClientSettings();
};

/*!
 * \struct UdpServerSettings
 * \brief Настройки UDP-сервера.
 */
struct UdpServerSettings
{
    /*!
     * \brief Локальный адрес привязки UDP-сервера.
     */
    QString bindAddress;

    /*!
     * \brief Локальный порт привязки UDP-сервера.
     */
    quint16 bindPort = 50000;

    /*!
     * \brief Создаёт настройки UDP-сервера со значениями по умолчанию.
     */
    UdpServerSettings();
};

class QSettings;

/*!
 * \class AppSettings
 * \brief Сервис чтения и сохранения настроек UDP-приложений.
 *
 * \details
 * Класс инкапсулирует работу с QSettings и предоставляет типизированный
 * интерфейс для клиентского и серверного окон.
 */
class AppSettings
{
public:
    /*!
     * \brief Создаёт сервис настроек.
     *
     * \details
     * По умолчанию используются реальные настройки лабораторного приложения.
     * Для unit-тестов можно передать отдельные имена организации и приложения,
     * чтобы не затрагивать пользовательские настройки.
     *
     * \param organizationName Имя организации для QSettings.
     * \param applicationName Имя приложения для QSettings.
     */
    explicit AppSettings(
            const QString& organizationName = QStringLiteral("Myshkovskiy Svyatoslav"),
            const QString& applicationName = QStringLiteral("Lab5Udp"));

    /*!
     * \brief Загружает настройки UDP-клиента.
     * \return Настройки клиента.
     */
    UdpClientSettings loadClientSettings() const;

    /*!
     * \brief Сохраняет настройки UDP-клиента.
     * \param settings Настройки клиента.
     */
    void saveClientSettings(const UdpClientSettings& settings) const;

    /*!
     * \brief Загружает настройки UDP-сервера.
     * \return Настройки сервера.
     */
    UdpServerSettings loadServerSettings() const;

    /*!
     * \brief Сохраняет настройки UDP-сервера.
     * \param settings Настройки сервера.
     */
    void saveServerSettings(const UdpServerSettings& settings) const;

private:
    /*!
     * \brief Создаёт объект QSettings с текущими именами организации и приложения.
     * \return Объект QSettings.
     */
    QSettings createSettings_() const;

private:
    QString organizationName_;
    QString applicationName_;
};

#endif // APPSETTINGS_H
