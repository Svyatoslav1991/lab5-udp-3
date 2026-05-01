#include <QtTest>

#include "mainwindow.h"

#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkDatagram>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTextEdit>
#include <QUdpSocket>

/*!
 * \class TestServerMainWindow
 * \brief Тесты главного окна UDP-сервера.
 *
 * \details
 * Проверяет UI-логику серверного окна:
 * - загрузку настроек по умолчанию;
 * - запуск UDP-сервера через кнопку;
 * - остановку UDP-сервера через кнопку;
 * - отображение ошибки при некорректном адресе;
 * - приём UDP-дейтаграммы и запись в журнал;
 * - сохранение настроек при закрытии окна.
 */
class TestServerMainWindow : public QObject
{
    Q_OBJECT

private slots:
    /*!
     * \brief Очищает QSettings перед всеми тестами.
     */
    void initTestCase();

    /*!
     * \brief Очищает QSettings перед каждым тестом.
     */
    void init();

    /*!
     * \brief Очищает QSettings после каждого теста.
     */
    void cleanup();

    /*!
     * \brief Проверяет начальное состояние серверного окна.
     */
    void constructor_loadsDefaultSettingsAndInitialUiState();

    /*!
     * \brief Проверяет запуск сервера через кнопку.
     */
    void startServerButton_withValidParams_startsServerUpdatesUiAndSavesSettings();

    /*!
     * \brief Проверяет остановку сервера через кнопку.
     */
    void stopServerButton_whenServerIsRunning_stopsServerAndUpdatesUi();

    /*!
     * \brief Проверяет логирование ошибки при некорректном адресе привязки.
     */
    void startServerButton_withInvalidAddress_writesErrorLog();

    /*!
     * \brief Проверяет приём UDP-дейтаграммы и запись в журнал.
     */
    void runningServer_whenDatagramSent_writesReceivedDatagramToLog();

    /*!
     * \brief Проверяет сохранение настроек при закрытии окна.
     */
    void closeEvent_savesServerSettings();

private:
    /*!
     * \brief Ищет обязательный дочерний виджет по objectName.
     * \param parent Родительский объект.
     * \param objectName Имя объекта.
     * \return Указатель на найденный виджет.
     */
    template <typename Widget>
    Widget* findRequiredWidget_(QObject* parent, const char* objectName) const;

    /*!
     * \brief Очищает хранилище настроек тестируемого приложения.
     */
    void clearSettings_() const;

    /*!
     * \brief Возвращает объект QSettings приложения.
     * \return Объект QSettings.
     */
    QSettings createSettings_() const;

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

void TestServerMainWindow::initTestCase()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestServerMainWindow::init()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestServerMainWindow::cleanup()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestServerMainWindow::constructor_loadsDefaultSettingsAndInitialUiState()
{
    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* startServerButton = findRequiredWidget_<QPushButton>(&window, "startServerButton");
    auto* stopServerButton = findRequiredWidget_<QPushButton>(&window, "stopServerButton");
    auto* statusValueLabel = findRequiredWidget_<QLabel>(&window, "statusValueLabel");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    QCOMPARE(addressLineEdit->text(), QStringLiteral("127.0.0.1"));
    QCOMPARE(portSpinBox->value(), 50000);

    QVERIFY(startServerButton->isEnabled());
    QVERIFY(!stopServerButton->isEnabled());
    QVERIFY(addressLineEdit->isEnabled());
    QVERIFY(portSpinBox->isEnabled());

    QCOMPARE(statusValueLabel->text(), QStringLiteral("Остановлен"));
    QVERIFY(logTextEdit->toPlainText().isEmpty());
}

//--------------------------------------------------------------------------

void TestServerMainWindow::startServerButton_withValidParams_startsServerUpdatesUiAndSavesSettings()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* startServerButton = findRequiredWidget_<QPushButton>(&window, "startServerButton");
    auto* stopServerButton = findRequiredWidget_<QPushButton>(&window, "stopServerButton");
    auto* statusValueLabel = findRequiredWidget_<QLabel>(&window, "statusValueLabel");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("127.0.0.1"));
    portSpinBox->setValue(port);

    startServerButton->click();

    QVERIFY(!startServerButton->isEnabled());
    QVERIFY(stopServerButton->isEnabled());
    QVERIFY(!addressLineEdit->isEnabled());
    QVERIFY(!portSpinBox->isEnabled());

    QCOMPARE(statusValueLabel->text(), QStringLiteral("Запущен"));

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("UDP-сервер запущен")));
    QVERIFY(logText.contains(QStringLiteral("127.0.0.1")));
    QVERIFY(logText.contains(QString::number(port)));

    const QSettings settings = createSettings_();

    QCOMPARE(settings.value(QStringLiteral("server/bindAddress")).toString(),
             QStringLiteral("127.0.0.1"));
    QCOMPARE(settings.value(QStringLiteral("server/bindPort")).toInt(),
             static_cast<int>(port));
}

//--------------------------------------------------------------------------

void TestServerMainWindow::stopServerButton_whenServerIsRunning_stopsServerAndUpdatesUi()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* startServerButton = findRequiredWidget_<QPushButton>(&window, "startServerButton");
    auto* stopServerButton = findRequiredWidget_<QPushButton>(&window, "stopServerButton");
    auto* statusValueLabel = findRequiredWidget_<QLabel>(&window, "statusValueLabel");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("127.0.0.1"));
    portSpinBox->setValue(port);

    startServerButton->click();

    QVERIFY(stopServerButton->isEnabled());
    QCOMPARE(statusValueLabel->text(), QStringLiteral("Запущен"));

    stopServerButton->click();

    QVERIFY(startServerButton->isEnabled());
    QVERIFY(!stopServerButton->isEnabled());
    QVERIFY(addressLineEdit->isEnabled());
    QVERIFY(portSpinBox->isEnabled());

    QCOMPARE(statusValueLabel->text(), QStringLiteral("Остановлен"));

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("UDP-сервер запущен")));
    QVERIFY(logText.contains(QStringLiteral("UDP-сервер остановлен")));
}

//--------------------------------------------------------------------------

void TestServerMainWindow::startServerButton_withInvalidAddress_writesErrorLog()
{
    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* startServerButton = findRequiredWidget_<QPushButton>(&window, "startServerButton");
    auto* stopServerButton = findRequiredWidget_<QPushButton>(&window, "stopServerButton");
    auto* statusValueLabel = findRequiredWidget_<QLabel>(&window, "statusValueLabel");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("invalid-address"));
    portSpinBox->setValue(50000);

    startServerButton->click();

    QVERIFY(startServerButton->isEnabled());
    QVERIFY(!stopServerButton->isEnabled());
    QVERIFY(addressLineEdit->isEnabled());
    QVERIFY(portSpinBox->isEnabled());

    QCOMPARE(statusValueLabel->text(), QStringLiteral("Остановлен"));

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("Ошибка")));
    QVERIFY(logText.contains(QStringLiteral("адрес")));
}

//--------------------------------------------------------------------------

void TestServerMainWindow::runningServer_whenDatagramSent_writesReceivedDatagramToLog()
{
    quint16 port = 0;
    QVERIFY(findFreeLocalUdpPort_(&port));

    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* startServerButton = findRequiredWidget_<QPushButton>(&window, "startServerButton");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("127.0.0.1"));
    portSpinBox->setValue(port);

    startServerButton->click();

    const QByteArray payload = QByteArrayLiteral("Hello server window");

    QVERIFY(sendLocalDatagram_(port, payload));

    QTRY_VERIFY_WITH_TIMEOUT(logTextEdit->toPlainText()
                             .contains(QStringLiteral("Hello server window")),
                             1000);

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("Получено")));
    QVERIFY(logText.contains(QStringLiteral("байт")));
    QVERIFY(logText.contains(QStringLiteral("127.0.0.1")));
    QVERIFY(logText.contains(QStringLiteral("Hello server window")));
}

//--------------------------------------------------------------------------

void TestServerMainWindow::closeEvent_savesServerSettings()
{
    {
        MainWindow window;

        auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
        auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");

        addressLineEdit->setText(QStringLiteral("127.0.0.1"));
        portSpinBox->setValue(54000);

        window.close();
    }

    const QSettings settings = createSettings_();

    QCOMPARE(settings.value(QStringLiteral("server/bindAddress")).toString(),
             QStringLiteral("127.0.0.1"));
    QCOMPARE(settings.value(QStringLiteral("server/bindPort")).toInt(),
             54000);
}

//--------------------------------------------------------------------------

template <typename Widget>
Widget* TestServerMainWindow::findRequiredWidget_(QObject* parent,
                                                  const char* objectName) const
{
    Widget* widget = parent->findChild<Widget*>(QString::fromLatin1(objectName));

    if (widget == nullptr) {
        qFatal("Required widget was not found: %s", objectName);
    }

    return widget;
}

//--------------------------------------------------------------------------

void TestServerMainWindow::clearSettings_() const
{
    QSettings settings = createSettings_();
    settings.clear();
    settings.sync();
}

//--------------------------------------------------------------------------

QSettings TestServerMainWindow::createSettings_() const
{
    return QSettings(QStringLiteral("Myshkovskiy Svyatoslav"),
                     QStringLiteral("Lab5Udp"));
}

//--------------------------------------------------------------------------

bool TestServerMainWindow::findFreeLocalUdpPort_(quint16* port) const
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

bool TestServerMainWindow::sendLocalDatagram_(quint16 port,
                                              const QByteArray& payload) const
{
    QUdpSocket sender;

    const qint64 bytesWritten = sender.writeDatagram(payload,
                                                     QHostAddress(QHostAddress::LocalHost),
                                                     port);

    return bytesWritten == payload.size();
}

//--------------------------------------------------------------------------

QTEST_MAIN(TestServerMainWindow)

#include "test_server_mainwindow.moc"
