#include <QtTest>

#include "appsettings.h"

#include <QSettings>
#include <QUuid>

/*!
 * \class TestAppSettings
 * \brief Unit-тесты сервиса AppSettings.
 *
 * \details
 * Тесты используют уникальные имена организации и приложения,
 * чтобы не затрагивать реальные настройки UDP-приложения.
 */
class TestAppSettings : public QObject
{
    Q_OBJECT

private slots:
    /*!
     * \brief Очищает тестовое хранилище перед каждым тестом.
     */
    void init();

    /*!
     * \brief Очищает тестовое хранилище после каждого теста.
     */
    void cleanup();

    /*!
     * \brief Проверяет значения UDP-клиента по умолчанию.
     */
    void loadClientSettings_returnsDefaultValuesWhenStorageIsEmpty();

    /*!
     * \brief Проверяет сохранение и загрузку настроек UDP-клиента.
     */
    void saveAndLoadClientSettings_returnsSavedValues();

    /*!
     * \brief Проверяет значения UDP-сервера по умолчанию.
     */
    void loadServerSettings_returnsDefaultValuesWhenStorageIsEmpty();

    /*!
     * \brief Проверяет сохранение и загрузку настроек UDP-сервера.
     */
    void saveAndLoadServerSettings_returnsSavedValues();

    /*!
     * \brief Проверяет восстановление порта клиента по умолчанию при некорректном сохранённом значении.
     */
    void loadClientSettings_replacesInvalidPortWithDefaultValue();

    /*!
     * \brief Проверяет восстановление интервала клиента по умолчанию при некорректном сохранённом значении.
     */
    void loadClientSettings_replacesInvalidIntervalWithDefaultValue();

private:
    /*!
     * \brief Создаёт AppSettings для текущего тестового хранилища.
     * \return Сервис настроек.
     */
    AppSettings createAppSettings_() const;

    /*!
     * \brief Очищает текущее тестовое хранилище QSettings.
     */
    void clearStorage_() const;

private:
    QString organizationName_;
    QString applicationName_;
};

//--------------------------------------------------------------------------

void TestAppSettings::init()
{
    organizationName_ = QStringLiteral("Lab5UdpTests");
    applicationName_ = QStringLiteral("AppSettings_%1")
            .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    clearStorage_();
}

//--------------------------------------------------------------------------

void TestAppSettings::cleanup()
{
    clearStorage_();
}

//--------------------------------------------------------------------------

void TestAppSettings::loadClientSettings_returnsDefaultValuesWhenStorageIsEmpty()
{
    const AppSettings appSettings = createAppSettings_();

    const UdpClientSettings settings = appSettings.loadClientSettings();

    QCOMPARE(settings.address, QStringLiteral("127.0.0.1"));
    QCOMPARE(settings.port, static_cast<quint16>(50000));
    QCOMPARE(settings.intervalMs, 1000);
    QCOMPARE(settings.useCurrentTime, true);
    QCOMPARE(settings.message, QString());
}

//--------------------------------------------------------------------------

void TestAppSettings::saveAndLoadClientSettings_returnsSavedValues()
{
    const AppSettings appSettings = createAppSettings_();

    UdpClientSettings savedSettings;
    savedSettings.address = QStringLiteral("192.168.0.10");
    savedSettings.port = 51000;
    savedSettings.intervalMs = 2500;
    savedSettings.useCurrentTime = false;
    savedSettings.message = QStringLiteral("Hello UDP");

    appSettings.saveClientSettings(savedSettings);

    const UdpClientSettings loadedSettings = appSettings.loadClientSettings();

    QCOMPARE(loadedSettings.address, savedSettings.address);
    QCOMPARE(loadedSettings.port, savedSettings.port);
    QCOMPARE(loadedSettings.intervalMs, savedSettings.intervalMs);
    QCOMPARE(loadedSettings.useCurrentTime, savedSettings.useCurrentTime);
    QCOMPARE(loadedSettings.message, savedSettings.message);
}

//--------------------------------------------------------------------------

void TestAppSettings::loadServerSettings_returnsDefaultValuesWhenStorageIsEmpty()
{
    const AppSettings appSettings = createAppSettings_();

    const UdpServerSettings settings = appSettings.loadServerSettings();

    QCOMPARE(settings.bindAddress, QStringLiteral("127.0.0.1"));
    QCOMPARE(settings.bindPort, static_cast<quint16>(50000));
}

//--------------------------------------------------------------------------

void TestAppSettings::saveAndLoadServerSettings_returnsSavedValues()
{
    const AppSettings appSettings = createAppSettings_();

    UdpServerSettings savedSettings;
    savedSettings.bindAddress = QStringLiteral("0.0.0.0");
    savedSettings.bindPort = 52000;

    appSettings.saveServerSettings(savedSettings);

    const UdpServerSettings loadedSettings = appSettings.loadServerSettings();

    QCOMPARE(loadedSettings.bindAddress, savedSettings.bindAddress);
    QCOMPARE(loadedSettings.bindPort, savedSettings.bindPort);
}

//--------------------------------------------------------------------------

void TestAppSettings::loadClientSettings_replacesInvalidPortWithDefaultValue()
{
    QSettings storage(organizationName_, applicationName_);
    storage.setValue(QStringLiteral("client/port"), 70000);
    storage.sync();

    const AppSettings appSettings = createAppSettings_();

    const UdpClientSettings settings = appSettings.loadClientSettings();

    QCOMPARE(settings.port, static_cast<quint16>(50000));
}

//--------------------------------------------------------------------------

void TestAppSettings::loadClientSettings_replacesInvalidIntervalWithDefaultValue()
{
    QSettings storage(organizationName_, applicationName_);
    storage.setValue(QStringLiteral("client/intervalMs"), -100);
    storage.sync();

    const AppSettings appSettings = createAppSettings_();

    const UdpClientSettings settings = appSettings.loadClientSettings();

    QCOMPARE(settings.intervalMs, 1000);
}

//--------------------------------------------------------------------------

AppSettings TestAppSettings::createAppSettings_() const
{
    return AppSettings(organizationName_, applicationName_);
}

//--------------------------------------------------------------------------

void TestAppSettings::clearStorage_() const
{
    QSettings storage(organizationName_, applicationName_);
    storage.clear();
    storage.sync();
}

//--------------------------------------------------------------------------

QTEST_MAIN(TestAppSettings)

#include "test_appsettings.moc"
