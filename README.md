# cserver — C++ EtherCAT сервер (Windows)

Минимальный каркас сервиса: цикл EtherCAT (заглушка) и HTTP-сервер (заглушка). Постепенно заменим на SOEM и REST API.

## Сборка

Требуется CMake 3.20+, компилятор с C++17 (MSVC, MinGW).

```bash
cmake -S . -B build -G "Ninja"
cmake --build build --config Release
```

Исполняемый файл: `build/cserver`.

## Запуск

```bash
build/cserver
```

По умолчанию HTTP-заглушка на порту 8080.

## План интеграции SOEM на Windows

1. Установить Npcap (совместим c WinPcap API) — нужен сырой доступ к NIC.
2. Склонировать [SOEM](https://github.com/OpenEtherCATsociety/SOEM), собрать как static lib для MSVC.
3. Включить флаг `-DUSE_SOEM=ON` и подключить include/lib SOEM в CMake.
4. Реализовать EtherCAT-поток реального времени с приоритетом и таймером (1 кГц/2 кГц).

## REST API (план)

- POST `/motors/{id}/position` body: `{ "pos_rad": number }`
- POST `/motors/{id}/velocity` body: `{ "vel_rads": number }`
- GET `/state` — агрегированное состояние узлов

На первом этапе используем `cpp-httplib` (header-only), затем при необходимости заменить на Asio/Boost.Beast.


