#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QByteArray>
#include <QString>

#include "appsettings.h"

class UdpClientController;
class QCloseEvent;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 * \brief Главное окно UDP-клиента.
 *
 * \details
 * Класс отвечает только за взаимодействие с интерфейсом:
 * - считывает адрес, порт, интервал и текст дейтаграммы;
 * - передаёт данные в UdpClientController;
 * - отображает результат отправки в журнале.
 *
 * Сетевая логика инкапсулирована в UdpClientController.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Создаёт главное окно UDP-клиента.
     * \param parent Родительский QWidget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /*!
     * \brief Уничтожает главное окно UDP-клиента.
     */
    ~MainWindow() override;

protected:
    /*!
     * \brief Сохраняет настройки клиента при закрытии окна.
     * \param event Событие закрытия окна.
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    /*!
     * \brief Обрабатывает нажатие кнопки отправки одной дейтаграммы.
     */
    void onSendOnceClicked_();

    /*!
     * \brief Обрабатывает запуск периодической отправки дейтаграмм.
     */
    void onStartClicked_();

    /*!
     * \brief Обрабатывает остановку периодической отправки дейтаграмм.
     */
    void onStopClicked_();

    /*!
     * \brief Включает или отключает поле ручного ввода текста.
     * \param checked true, если нужно отправлять текущее время.
     */
    void onCurrentTimeModeChanged_(bool checked);

    /*!
     * \brief Обрабатывает успешную отправку дейтаграммы.
     * \param datagram Отправленные данные.
     * \param address Адрес получателя.
     * \param port Порт получателя.
     * \param bytesWritten Количество отправленных байт.
     */
    void onDatagramSent_(const QByteArray& datagram,
                         const QHostAddress& address,
                         quint16 port,
                         qint64 bytesWritten);

    /*!
     * \brief Обрабатывает ошибку UDP-клиента.
     * \param message Текст ошибки.
     */
    void onClientError_(const QString& message);

    /*!
     * \brief Загружает сохранённые настройки клиента в интерфейс.
     */
    void loadSettings_();

    /*!
     * \brief Сохраняет текущие настройки клиента из интерфейса.
     */
    void saveSettings_() const;

    /*!
     * \brief Собирает настройки клиента из текущего состояния интерфейса.
     * \return Настройки клиента.
     */
    UdpClientSettings collectSettings_() const;

private:
    /*!
     * \brief Подключает сигналы интерфейса и контроллера.
     */
    void setupConnections_();

    /*!
     * \brief Формирует текущую дейтаграмму на основе состояния интерфейса.
     * \return Данные дейтаграммы.
     */
    QByteArray createDatagram_() const;

    /*!
     * \brief Отправляет текущую дейтаграмму по параметрам из интерфейса.
     * \return true, если отправка была успешно инициирована.
     */
    bool sendCurrentDatagram_();

    /*!
     * \brief Считывает адрес получателя из интерфейса.
     * \return Адрес получателя.
     */
    QHostAddress destinationAddress_() const;

    /*!
     * \brief Считывает порт получателя из интерфейса.
     * \return Порт получателя.
     */
    quint16 destinationPort_() const;

    /*!
     * \brief Считывает интервал периодической отправки.
     * \return Интервал в миллисекундах.
     */
    int sendingIntervalMs_() const;

    /*!
     * \brief Переключает состояние интерфейса при периодической отправке.
     * \param active true, если периодическая отправка активна.
     */
    void setPeriodicSendingUiActive_(bool active);

    /*!
     * \brief Добавляет сообщение в журнал клиента.
     * \param message Сообщение журнала.
     */
    void appendLog_(const QString& message);

private:
    Ui::MainWindow* ui;
    UdpClientController* clientController_;
    AppSettings appSettings_;
};

#endif // MAINWINDOW_H
