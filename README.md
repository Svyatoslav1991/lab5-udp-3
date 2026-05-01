# Lab5 UDP Client/Server

Учебный проект по Qt/C++ для лабораторной работы по теме **UDP/IP-сокеты**.

Проект содержит два оконных приложения:

- **UDP client** — отправляет UDP-дейтаграммы на заданный адрес и порт.
- **UDP server** — привязывается к заданному адресу и порту, принимает UDP-дейтаграммы и отображает их в журнале.

Проект собран на CMake, разделён на модули `core`, `client`, `server` и покрыт unit-тестами.

## Возможности

### UDP client

- отправка одной UDP-дейтаграммы;
- периодическая отправка дейтаграмм по таймеру;
- отправка текущего времени;
- отправка пользовательского текста;
- настройка адреса, порта и интервала отправки;
- журнал отправленных дейтаграмм;
- сохранение настроек через `QSettings`.

### UDP server

- запуск приёма UDP-дейтаграмм через `QUdpSocket::bind`;
- остановка приёма;
- отображение состояния сервера;
- отображение адреса и порта отправителя;
- журнал принятых дейтаграмм;
- сохранение настроек через `QSettings`.

### Tests

Проект содержит тесты для:

- `AppSettings`;
- `UdpClientController`;
- `UdpServerController`;
- клиентского `MainWindow`;
- серверного `MainWindow`.

## Структура проекта

```text
lab5_udp_3/
├── .github/
│   └── workflows/
│       └── ci.yml
├── client/
│   ├── include/
│   │   ├── mainwindow.h
│   │   └── udpclientcontroller.h
│   ├── src/
│   │   ├── main.cpp
│   │   ├── mainwindow.cpp
│   │   ├── mainwindow.ui
│   │   └── udpclientcontroller.cpp
│   └── CMakeLists.txt
├── core/
│   ├── include/
│   │   ├── appsettings.h
│   │   └── udpdatagraminfo.h
│   ├── src/
│   │   └── appsettings.cpp
│   └── CMakeLists.txt
├── server/
│   ├── include/
│   │   ├── mainwindow.h
│   │   └── udpservercontroller.h
│   ├── src/
│   │   ├── main.cpp
│   │   ├── mainwindow.cpp
│   │   ├── mainwindow.ui
│   │   └── udpservercontroller.cpp
│   └── CMakeLists.txt
├── tests/
│   ├── test_appsettings.cpp
│   ├── test_client_mainwindow.cpp
│   ├── test_server_mainwindow.cpp
│   ├── test_udpclientcontroller.cpp
│   ├── test_udpservercontroller.cpp
│   └── CMakeLists.txt
├── CMakeLists.txt
├── LICENSE.md
└── README.md
```

## Используемые технологии

- C++17;
- Qt Widgets;
- Qt Network;
- Qt Test;
- CMake;
- GitHub Actions.

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Запуск тестов

```bash
ctest --test-dir build --output-on-failure
```

## Запуск приложений

После сборки будут доступны два исполняемых файла:

- `udp_client`;
- `udp_server`.

Типичный сценарий проверки:

1. Запустить `udp_server`.
2. Нажать кнопку **Запустить сервер**.
3. Запустить `udp_client`.
4. Нажать **Отправить один раз** или **Старт**.
5. Проверить журнал приёма на сервере.

По умолчанию используются параметры:

```text
address: 127.0.0.1
port:    50000
```

## Архитектура

Проект разделён на три основных модуля.

### `core`

Общая логика, не зависящая от конкретного приложения:

- `AppSettings` — чтение и сохранение настроек через `QSettings`;
- `UdpDatagramInfo` — DTO с информацией о принятой UDP-дейтаграмме.

### `client`

Клиентское GUI-приложение:

- `MainWindow` отвечает за интерфейс;
- `UdpClientController` инкапсулирует UDP-отправку через `QUdpSocket`.

### `server`

Серверное GUI-приложение:

- `MainWindow` отвечает за интерфейс;
- `UdpServerController` инкапсулирует привязку сокета, приём и обработку UDP-дейтаграмм.

## CI

В проект добавлен простой GitHub Actions workflow:

- установка Qt5 и CMake;
- конфигурация проекта;
- сборка;
- запуск тестов через CTest.

Файл workflow расположен по пути:

```text
.github/workflows/ci.yml
```

## Лицензия

Проект распространяется под лицензией MIT. См. файл [LICENSE.md](LICENSE.md).
