#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "udpservercontroller.h"

#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serverController_(new UdpServerController(this))
{
    ui->setupUi(this);

    loadSettings_();
    setupConnections_();
    setServerUiActive_(false);
}

//--------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

//--------------------------------------------------------------------------

void MainWindow::setupConnections_()
{
    connect(ui->startServerButton, &QPushButton::clicked,
            this, &MainWindow::onStartServerClicked_);

    connect(ui->stopServerButton, &QPushButton::clicked,
            this, &MainWindow::onStopServerClicked_);

    connect(serverController_, &UdpServerController::receivingStarted,
            this, &MainWindow::onReceivingStarted_);

    connect(serverController_, &UdpServerController::receivingStopped,
            this, &MainWindow::onReceivingStopped_);

    connect(serverController_, &UdpServerController::datagramReceived,
            this, &MainWindow::onDatagramReceived_);

    connect(serverController_, &UdpServerController::errorOccurred,
            this, &MainWindow::onServerError_);
}

//--------------------------------------------------------------------------

void MainWindow::onStartServerClicked_()
{
    serverController_->startReceiving(bindAddress_(), bindPort_());
}

//--------------------------------------------------------------------------

void MainWindow::onStopServerClicked_()
{
    serverController_->stopReceiving();
}

//--------------------------------------------------------------------------

void MainWindow::onReceivingStarted_(const QHostAddress& address, quint16 port)
{
    saveSettings_();
    setServerUiActive_(true);

    appendLog_(QStringLiteral("UDP-сервер запущен на %1:%2.")
               .arg(address.toString())
               .arg(port));
}

//--------------------------------------------------------------------------

void MainWindow::onReceivingStopped_()
{
    setServerUiActive_(false);
    appendLog_(QStringLiteral("UDP-сервер остановлен."));
}

//--------------------------------------------------------------------------

void MainWindow::onDatagramReceived_(const UdpDatagramInfo& datagramInfo)
{
    const QString text = QString::fromUtf8(datagramInfo.data);

    const QString message = QStringLiteral("Получено %1 байт от %2:%3 — \"%4\"")
            .arg(datagramInfo.data.size())
            .arg(datagramInfo.senderAddress.toString())
            .arg(datagramInfo.senderPort)
            .arg(text);

    appendLog_(message);
}

//--------------------------------------------------------------------------

void MainWindow::onServerError_(const QString& message)
{
    appendLog_(QStringLiteral("Ошибка: %1").arg(message));
}

//--------------------------------------------------------------------------

QHostAddress MainWindow::bindAddress_() const
{
    return QHostAddress(ui->addressLineEdit->text().trimmed());
}

//--------------------------------------------------------------------------

quint16 MainWindow::bindPort_() const
{
    return static_cast<quint16>(ui->portSpinBox->value());
}

//--------------------------------------------------------------------------

void MainWindow::setServerUiActive_(bool active)
{
    ui->startServerButton->setEnabled(!active);
    ui->stopServerButton->setEnabled(active);

    ui->addressLineEdit->setEnabled(!active);
    ui->portSpinBox->setEnabled(!active);

    ui->statusValueLabel->setText(active
                                  ? QStringLiteral("Запущен")
                                  : QStringLiteral("Остановлен"));
}

//--------------------------------------------------------------------------

void MainWindow::appendLog_(const QString& message)
{
    const QString timestamp = QDateTime::currentDateTime()
            .toString(QStringLiteral("hh:mm:ss"));

    ui->logTextEdit->append(QStringLiteral("[%1] %2").arg(timestamp, message));
}

//--------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveSettings_();
    QMainWindow::closeEvent(event);
}

//--------------------------------------------------------------------------

void MainWindow::loadSettings_()
{
    const UdpServerSettings settings = appSettings_.loadServerSettings();

    ui->addressLineEdit->setText(settings.bindAddress);
    ui->portSpinBox->setValue(settings.bindPort);
}

//--------------------------------------------------------------------------

void MainWindow::saveSettings_() const
{
    appSettings_.saveServerSettings(collectSettings_());
}

//--------------------------------------------------------------------------

UdpServerSettings MainWindow::collectSettings_() const
{
    UdpServerSettings settings;
    settings.bindAddress = ui->addressLineEdit->text().trimmed();
    settings.bindPort = static_cast<quint16>(ui->portSpinBox->value());

    return settings;
}
