#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "udpdatagraminfo.h"
#include "appsettings.h"

#include <QHostAddress>
#include <QMainWindow>
#include <QString>

class UdpServerController;
class QCloseEvent;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 * \brief Главное окно UDP-сервера.
 *
 * \details
 * Класс отвечает за взаимодействие с интерфейсом:
 * - считывает адрес и порт привязки;
 * - запускает и останавливает UdpServerController;
 * - отображает состояние сервера;
 * - выводит принятые UDP-дейтаграммы в журнал.
 *
 * Сетевая логика инкапсулирована в UdpServerController.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Создаёт главное окно UDP-сервера.
     * \param parent Родительский QWidget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /*!
     * \brief Уничтожает главное окно UDP-сервера.
     */
    ~MainWindow() override;

protected:
    /*!
     * \brief Сохраняет настройки сервера при закрытии окна.
     * \param event Событие закрытия окна.
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    /*!
     * \brief Обрабатывает запуск UDP-сервера.
     */
    void onStartServerClicked_();

    /*!
     * \brief Обрабатывает остановку UDP-сервера.
     */
    void onStopServerClicked_();

    /*!
     * \brief Обрабатывает успешный запуск приёма UDP-дейтаграмм.
     * \param address Фактический адрес привязки.
     * \param port Фактический порт привязки.
     */
    void onReceivingStarted_(const QHostAddress& address, quint16 port);

    /*!
     * \brief Обрабатывает остановку приёма UDP-дейтаграмм.
     */
    void onReceivingStopped_();

    /*!
     * \brief Обрабатывает принятую UDP-дейтаграмму.
     * \param datagramInfo Информация о принятой дейтаграмме.
     */
    void onDatagramReceived_(const UdpDatagramInfo& datagramInfo);

    /*!
     * \brief Обрабатывает ошибку UDP-сервера.
     * \param message Текст ошибки.
     */
    void onServerError_(const QString& message);

private:
    /*!
     * \brief Подключает сигналы интерфейса и контроллера.
     */
    void setupConnections_();

    /*!
     * \brief Считывает адрес привязки из интерфейса.
     * \return Адрес привязки.
     */
    QHostAddress bindAddress_() const;

    /*!
     * \brief Считывает порт привязки из интерфейса.
     * \return Порт привязки.
     */
    quint16 bindPort_() const;

    /*!
     * \brief Переключает состояние интерфейса при запуске/остановке сервера.
     * \param active true, если сервер запущен.
     */
    void setServerUiActive_(bool active);

    /*!
     * \brief Добавляет сообщение в журнал сервера.
     * \param message Сообщение журнала.
     */
    void appendLog_(const QString& message);

    /*!
     * \brief Загружает сохранённые настройки сервера в интерфейс.
     */
    void loadSettings_();

    /*!
     * \brief Сохраняет текущие настройки сервера из интерфейса.
     */
    void saveSettings_() const;

    /*!
     * \brief Собирает настройки сервера из текущего состояния интерфейса.
     * \return Настройки сервера.
     */
    UdpServerSettings collectSettings_() const;

private:
    Ui::MainWindow* ui;
    UdpServerController* serverController_;
    AppSettings appSettings_;
};

#endif // MAINWINDOW_H
