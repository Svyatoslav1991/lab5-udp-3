#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "udpclientcontroller.h"

#include <QDateTime>
#include <QHostAddress>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , clientController_(new UdpClientController(this))
{
    ui->setupUi(this);

    setupConnections_();
    onCurrentTimeModeChanged_(ui->currentTimeCheckBox->isChecked());
}

//--------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

//--------------------------------------------------------------------------

void MainWindow::setupConnections_()
{
    connect(ui->sendOnceButton, &QPushButton::clicked,
            this, &MainWindow::onSendOnceClicked_);

    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::onStartClicked_);

    connect(ui->stopButton, &QPushButton::clicked,
            this, &MainWindow::onStopClicked_);

    connect(ui->currentTimeCheckBox, &QCheckBox::toggled,
            this, &MainWindow::onCurrentTimeModeChanged_);

    connect(clientController_, &UdpClientController::datagramSent,
            this, &MainWindow::onDatagramSent_);

    connect(clientController_, &UdpClientController::errorOccurred,
            this, &MainWindow::onClientError_);
}

//--------------------------------------------------------------------------

void MainWindow::onSendOnceClicked_()
{
    clientController_->sendDatagram(createDatagram_(),
                                    destinationAddress_(),
                                    destinationPort_());
}

//--------------------------------------------------------------------------

void MainWindow::onStartClicked_()
{
    clientController_->startPeriodicSending(createDatagram_(),
                                            destinationAddress_(),
                                            destinationPort_(),
                                            sendingIntervalMs_());

    if (clientController_->isPeriodicSendingActive()) {
        setPeriodicSendingUiActive_(true);
        appendLog_(QStringLiteral("Периодическая отправка запущена."));
    }
}

//--------------------------------------------------------------------------

void MainWindow::onStopClicked_()
{
    clientController_->stopPeriodicSending();
    setPeriodicSendingUiActive_(false);

    appendLog_(QStringLiteral("Периодическая отправка остановлена."));
}

//--------------------------------------------------------------------------

void MainWindow::onCurrentTimeModeChanged_(bool checked)
{
    ui->messageLineEdit->setEnabled(!checked);
}

//--------------------------------------------------------------------------

void MainWindow::onDatagramSent_(const QByteArray& datagram,
                                 const QHostAddress& address,
                                 quint16 port,
                                 qint64 bytesWritten)
{
    const QString message = QStringLiteral("Отправлено %1 байт на %2:%3 — \"%4\"")
            .arg(bytesWritten)
            .arg(address.toString())
            .arg(port)
            .arg(QString::fromUtf8(datagram));

    appendLog_(message);
}

//--------------------------------------------------------------------------

void MainWindow::onClientError_(const QString& message)
{
    appendLog_(QStringLiteral("Ошибка: %1").arg(message));
}

//--------------------------------------------------------------------------

QByteArray MainWindow::createDatagram_() const
{
    if (ui->currentTimeCheckBox->isChecked()) {
        return QDateTime::currentDateTime()
                .toString(QStringLiteral("hh:mm:ss.zzz"))
                .toUtf8();
    }

    return ui->messageLineEdit->text().toUtf8();
}

//--------------------------------------------------------------------------

QHostAddress MainWindow::destinationAddress_() const
{
    return QHostAddress(ui->addressLineEdit->text().trimmed());
}

//--------------------------------------------------------------------------

quint16 MainWindow::destinationPort_() const
{
    return static_cast<quint16>(ui->portSpinBox->value());
}

//--------------------------------------------------------------------------

int MainWindow::sendingIntervalMs_() const
{
    return ui->intervalSpinBox->value();
}

//--------------------------------------------------------------------------

void MainWindow::setPeriodicSendingUiActive_(bool active)
{
    ui->startButton->setEnabled(!active);
    ui->stopButton->setEnabled(active);

    ui->sendOnceButton->setEnabled(!active);
    ui->addressLineEdit->setEnabled(!active);
    ui->portSpinBox->setEnabled(!active);
    ui->intervalSpinBox->setEnabled(!active);
    ui->currentTimeCheckBox->setEnabled(!active);

    ui->messageLineEdit->setEnabled(!active && !ui->currentTimeCheckBox->isChecked());
}

//--------------------------------------------------------------------------

void MainWindow::appendLog_(const QString& message)
{
    const QString timestamp = QDateTime::currentDateTime()
            .toString(QStringLiteral("hh:mm:ss"));

    ui->logTextEdit->append(QStringLiteral("[%1] %2").arg(timestamp, message));
}
