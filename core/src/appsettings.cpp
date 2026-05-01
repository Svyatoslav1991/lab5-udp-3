#include "appsettings.h"

#include <QSettings>

namespace
{
constexpr quint16 DEFAULT_PORT = 50000;
constexpr int DEFAULT_INTERVAL_MS = 1000;

QString defaultAddress()
{
    return QStringLiteral("127.0.0.1");
}

//--------------------------------------------------------------------------

quint16 readPort(const QSettings& settings,
                 const QString& key,
                 quint16 defaultValue)
{
    const int value = settings.value(key, defaultValue).toInt();

    if (value < 1 || value > 65535) {
        return defaultValue;
    }

    return static_cast<quint16>(value);
}

//--------------------------------------------------------------------------

int readPositiveInt(const QSettings& settings,
                    const QString& key,
                    int defaultValue)
{
    const int value = settings.value(key, defaultValue).toInt();

    if (value <= 0) {
        return defaultValue;
    }

    return value;
}

} // namespace

//--------------------------------------------------------------------------

UdpClientSettings::UdpClientSettings()
    : address(defaultAddress())
{
}

//--------------------------------------------------------------------------

UdpServerSettings::UdpServerSettings()
    : bindAddress(defaultAddress())
{
}

//--------------------------------------------------------------------------

AppSettings::AppSettings(const QString& organizationName,
                         const QString& applicationName)
    : organizationName_(organizationName)
    , applicationName_(applicationName)
{
}

//--------------------------------------------------------------------------

UdpClientSettings AppSettings::loadClientSettings() const
{
    const QSettings settings = createSettings_();

    UdpClientSettings result;
    result.address = settings.value(QStringLiteral("client/address"),
                                    defaultAddress()).toString();

    result.port = readPort(settings,
                           QStringLiteral("client/port"),
                           DEFAULT_PORT);

    result.intervalMs = readPositiveInt(settings,
                                        QStringLiteral("client/intervalMs"),
                                        DEFAULT_INTERVAL_MS);

    result.useCurrentTime = settings.value(QStringLiteral("client/useCurrentTime"),
                                           true).toBool();

    result.message = settings.value(QStringLiteral("client/message"),
                                    QString()).toString();

    return result;
}

//--------------------------------------------------------------------------

void AppSettings::saveClientSettings(const UdpClientSettings& settings) const
{
    QSettings storage = createSettings_();

    storage.setValue(QStringLiteral("client/address"), settings.address);
    storage.setValue(QStringLiteral("client/port"), settings.port);
    storage.setValue(QStringLiteral("client/intervalMs"), settings.intervalMs);
    storage.setValue(QStringLiteral("client/useCurrentTime"), settings.useCurrentTime);
    storage.setValue(QStringLiteral("client/message"), settings.message);

    storage.sync();
}

//--------------------------------------------------------------------------

UdpServerSettings AppSettings::loadServerSettings() const
{
    const QSettings settings = createSettings_();

    UdpServerSettings result;
    result.bindAddress = settings.value(QStringLiteral("server/bindAddress"),
                                        defaultAddress()).toString();

    result.bindPort = readPort(settings,
                               QStringLiteral("server/bindPort"),
                               DEFAULT_PORT);

    return result;
}

//--------------------------------------------------------------------------

void AppSettings::saveServerSettings(const UdpServerSettings& settings) const
{
    QSettings storage = createSettings_();

    storage.setValue(QStringLiteral("server/bindAddress"), settings.bindAddress);
    storage.setValue(QStringLiteral("server/bindPort"), settings.bindPort);

    storage.sync();
}

//--------------------------------------------------------------------------

QSettings AppSettings::createSettings_() const
{
    return QSettings(organizationName_, applicationName_);
}
