#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 * \brief Главное окно UDP-приложения.
 *
 * \details
 * На текущем этапе класс содержит только базовую инициализацию UI.
 * Сетевая логика UDP будет добавлена позже отдельным контроллером.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief Создаёт главное окно приложения.
     * \param parent Родительский QWidget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /*!
     * \brief Уничтожает главное окно и освобождает UI.
     */
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;
};

#endif // MAINWINDOW_H
