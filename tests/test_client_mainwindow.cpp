#include <QtTest>

#include "mainwindow.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QNetworkDatagram>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTextEdit>
#include <QUdpSocket>

/*!
 * \class TestClientMainWindow
 * \brief Тесты главного окна UDP-клиента.
 *
 * \details
 * Проверяет UI-логику клиентского окна:
 * - загрузку настроек по умолчанию;
 * - переключение режима ручного текста;
 * - отправку одной UDP-дейтаграммы через кнопку;
 * - логирование успешной отправки и ошибки;
 * - запуск и остановку периодической отправки;
 * - сохранение настроек при закрытии окна.
 */
class TestClientMainWindow : public QObject
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
     * \brief Проверяет начальное состояние окна при пустом хранилище настроек.
     */
    void constructor_loadsDefaultSettingsAndInitialUiState();

    /*!
     * \brief Проверяет включение и отключение поля ручного текста.
     */
    void currentTimeCheckBox_togglesMessageLineEdit();

    /*!
     * \brief Проверяет отправку ручного сообщения через кнопку "Отправить один раз".
     */
    void sendOnceButton_withManualMessage_sendsDatagramAndWritesLog();

    /*!
     * \brief Проверяет логирование ошибки при попытке отправить пустое ручное сообщение.
     */
    void sendOnceButton_withEmptyManualMessage_writesErrorLog();

    /*!
     * \brief Проверяет запуск и остановку периодической отправки.
     */
    void startAndStopButtons_controlPeriodicSendingAndUiState();

    /*!
     * \brief Проверяет сохранение настроек при закрытии окна.
     */
    void closeEvent_savesClientSettings();

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
     * \brief Привязывает UDP-сокет-приёмник к localhost и свободному порту.
     * \param receiver Сокет-приёмник.
     * \param port Сюда будет записан выбранный порт.
     * \return true, если привязка прошла успешно.
     */
    bool bindLocalReceiver_(QUdpSocket& receiver, quint16* port) const;

    /*!
     * \brief Возвращает объект QSettings приложения.
     * \return Объект QSettings.
     */
    QSettings createSettings_() const;
};

//--------------------------------------------------------------------------

void TestClientMainWindow::initTestCase()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestClientMainWindow::init()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestClientMainWindow::cleanup()
{
    clearSettings_();
}

//--------------------------------------------------------------------------

void TestClientMainWindow::constructor_loadsDefaultSettingsAndInitialUiState()
{
    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* intervalSpinBox = findRequiredWidget_<QSpinBox>(&window, "intervalSpinBox");
    auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
    auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");
    auto* sendOnceButton = findRequiredWidget_<QPushButton>(&window, "sendOnceButton");
    auto* startButton = findRequiredWidget_<QPushButton>(&window, "startButton");
    auto* stopButton = findRequiredWidget_<QPushButton>(&window, "stopButton");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    QCOMPARE(addressLineEdit->text(), QStringLiteral("127.0.0.1"));
    QCOMPARE(portSpinBox->value(), 50000);
    QCOMPARE(intervalSpinBox->value(), 1000);
    QCOMPARE(currentTimeCheckBox->isChecked(), true);

    QVERIFY(!messageLineEdit->isEnabled());
    QVERIFY(sendOnceButton->isEnabled());
    QVERIFY(startButton->isEnabled());
    QVERIFY(!stopButton->isEnabled());
    QVERIFY(logTextEdit->toPlainText().isEmpty());
}

//--------------------------------------------------------------------------

void TestClientMainWindow::currentTimeCheckBox_togglesMessageLineEdit()
{
    MainWindow window;

    auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
    auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");

    QVERIFY(currentTimeCheckBox->isChecked());
    QVERIFY(!messageLineEdit->isEnabled());

    currentTimeCheckBox->setChecked(false);

    QVERIFY(messageLineEdit->isEnabled());

    currentTimeCheckBox->setChecked(true);

    QVERIFY(!messageLineEdit->isEnabled());
}

//--------------------------------------------------------------------------

void TestClientMainWindow::sendOnceButton_withManualMessage_sendsDatagramAndWritesLog()
{
    QUdpSocket receiver;
    quint16 receiverPort = 0;

    QVERIFY2(bindLocalReceiver_(receiver, &receiverPort),
             qPrintable(receiver.errorString()));

    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
    auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");
    auto* sendOnceButton = findRequiredWidget_<QPushButton>(&window, "sendOnceButton");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("127.0.0.1"));
    portSpinBox->setValue(receiverPort);
    currentTimeCheckBox->setChecked(false);
    messageLineEdit->setText(QStringLiteral("Hello from MainWindow"));

    sendOnceButton->click();

    QTRY_VERIFY_WITH_TIMEOUT(receiver.hasPendingDatagrams(), 1000);

    const QNetworkDatagram datagram = receiver.receiveDatagram();

    QVERIFY(datagram.isValid());
    QCOMPARE(datagram.data(), QByteArrayLiteral("Hello from MainWindow"));

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("Отправлено")));
    QVERIFY(logText.contains(QStringLiteral("Hello from MainWindow")));
    QVERIFY(logText.contains(QString::number(receiverPort)));

    const QSettings settings = createSettings_();

    QCOMPARE(settings.value(QStringLiteral("client/address")).toString(),
             QStringLiteral("127.0.0.1"));
    QCOMPARE(settings.value(QStringLiteral("client/port")).toInt(),
             static_cast<int>(receiverPort));
    QCOMPARE(settings.value(QStringLiteral("client/useCurrentTime")).toBool(),
             false);
    QCOMPARE(settings.value(QStringLiteral("client/message")).toString(),
             QStringLiteral("Hello from MainWindow"));
}

//--------------------------------------------------------------------------

void TestClientMainWindow::sendOnceButton_withEmptyManualMessage_writesErrorLog()
{
    MainWindow window;

    auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
    auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");
    auto* sendOnceButton = findRequiredWidget_<QPushButton>(&window, "sendOnceButton");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    currentTimeCheckBox->setChecked(false);
    messageLineEdit->clear();

    sendOnceButton->click();

    const QString logText = logTextEdit->toPlainText();

    QVERIFY(logText.contains(QStringLiteral("Ошибка")));
    QVERIFY(logText.contains(QStringLiteral("пустую")));
}

//--------------------------------------------------------------------------

void TestClientMainWindow::startAndStopButtons_controlPeriodicSendingAndUiState()
{
    QUdpSocket receiver;
    quint16 receiverPort = 0;

    QVERIFY2(bindLocalReceiver_(receiver, &receiverPort),
             qPrintable(receiver.errorString()));

    MainWindow window;

    auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
    auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
    auto* intervalSpinBox = findRequiredWidget_<QSpinBox>(&window, "intervalSpinBox");
    auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
    auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");
    auto* sendOnceButton = findRequiredWidget_<QPushButton>(&window, "sendOnceButton");
    auto* startButton = findRequiredWidget_<QPushButton>(&window, "startButton");
    auto* stopButton = findRequiredWidget_<QPushButton>(&window, "stopButton");
    auto* logTextEdit = findRequiredWidget_<QTextEdit>(&window, "logTextEdit");

    addressLineEdit->setText(QStringLiteral("127.0.0.1"));
    portSpinBox->setValue(receiverPort);
    intervalSpinBox->setValue(50);
    currentTimeCheckBox->setChecked(false);
    messageLineEdit->setText(QStringLiteral("periodic"));

    startButton->click();

    QVERIFY(!startButton->isEnabled());
    QVERIFY(stopButton->isEnabled());
    QVERIFY(!sendOnceButton->isEnabled());
    QVERIFY(!addressLineEdit->isEnabled());
    QVERIFY(!portSpinBox->isEnabled());
    QVERIFY(!intervalSpinBox->isEnabled());
    QVERIFY(!currentTimeCheckBox->isEnabled());
    QVERIFY(!messageLineEdit->isEnabled());

    QTRY_VERIFY_WITH_TIMEOUT(receiver.hasPendingDatagrams(), 1000);

    const QNetworkDatagram firstDatagram = receiver.receiveDatagram();

    QVERIFY(firstDatagram.isValid());
    QCOMPARE(firstDatagram.data(), QByteArrayLiteral("periodic"));

    QVERIFY(logTextEdit->toPlainText().contains(QStringLiteral("Периодическая отправка запущена")));

    stopButton->click();

    QVERIFY(startButton->isEnabled());
    QVERIFY(!stopButton->isEnabled());
    QVERIFY(sendOnceButton->isEnabled());
    QVERIFY(addressLineEdit->isEnabled());
    QVERIFY(portSpinBox->isEnabled());
    QVERIFY(intervalSpinBox->isEnabled());
    QVERIFY(currentTimeCheckBox->isEnabled());
    QVERIFY(messageLineEdit->isEnabled());

    QVERIFY(logTextEdit->toPlainText().contains(QStringLiteral("Периодическая отправка остановлена")));
}

//--------------------------------------------------------------------------

void TestClientMainWindow::closeEvent_savesClientSettings()
{
    {
        MainWindow window;

        auto* addressLineEdit = findRequiredWidget_<QLineEdit>(&window, "addressLineEdit");
        auto* portSpinBox = findRequiredWidget_<QSpinBox>(&window, "portSpinBox");
        auto* intervalSpinBox = findRequiredWidget_<QSpinBox>(&window, "intervalSpinBox");
        auto* currentTimeCheckBox = findRequiredWidget_<QCheckBox>(&window, "currentTimeCheckBox");
        auto* messageLineEdit = findRequiredWidget_<QLineEdit>(&window, "messageLineEdit");

        addressLineEdit->setText(QStringLiteral("192.168.1.25"));
        portSpinBox->setValue(53000);
        intervalSpinBox->setValue(1500);
        currentTimeCheckBox->setChecked(false);
        messageLineEdit->setText(QStringLiteral("saved message"));

        window.close();
    }

    const QSettings settings = createSettings_();

    QCOMPARE(settings.value(QStringLiteral("client/address")).toString(),
             QStringLiteral("192.168.1.25"));
    QCOMPARE(settings.value(QStringLiteral("client/port")).toInt(), 53000);
    QCOMPARE(settings.value(QStringLiteral("client/intervalMs")).toInt(), 1500);
    QCOMPARE(settings.value(QStringLiteral("client/useCurrentTime")).toBool(), false);
    QCOMPARE(settings.value(QStringLiteral("client/message")).toString(),
             QStringLiteral("saved message"));
}

//--------------------------------------------------------------------------

template <typename Widget>
Widget* TestClientMainWindow::findRequiredWidget_(QObject* parent,
                                                  const char* objectName) const
{
    Widget* widget = parent->findChild<Widget*>(QString::fromLatin1(objectName));

    if (widget == nullptr) {
        qFatal("Required widget was not found: %s", objectName);
    }

    return widget;
}

//--------------------------------------------------------------------------

void TestClientMainWindow::clearSettings_() const
{
    QSettings settings = createSettings_();
    settings.clear();
    settings.sync();
}

//--------------------------------------------------------------------------

bool TestClientMainWindow::bindLocalReceiver_(QUdpSocket& receiver,
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

QSettings TestClientMainWindow::createSettings_() const
{
    return QSettings(QStringLiteral("Myshkovskiy Svyatoslav"),
                     QStringLiteral("Lab5Udp"));
}

//--------------------------------------------------------------------------

QTEST_MAIN(TestClientMainWindow)

#include "test_client_mainwindow.moc"
