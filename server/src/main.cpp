#include "mainwindow.h"

#include <QApplication>

/*!
 * \brief Точка входа в GUI-приложение.
 * \param argc Количество аргументов командной строки.
 * \param argv Аргументы командной строки.
 * \return Код завершения приложения.
 */
int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    MainWindow window;
    window.show();

    return application.exec();
}