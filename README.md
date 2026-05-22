# TRMM Implementation and Performance Benchmark

Реализация функции TRMM (Triangular Matrix Multiply) из BLAS уровня 3.

## Структура проекта


trmm_project/
├── src/
│ ├── trmm.hpp # Заголовочный файл
│ ├── trmm.cpp # Реализация TRMM
│ └── benchmark.cpp # Бенчмарк производительности
├── tests/
│ └── test_trmm.cpp # Тесты корректности
├── build.bat # Скрипт сборки
├── run_tests.bat # Запуск тестов
├── run_benchmark.bat # Запуск бенчмарка
├── CMakeLists.txt # CMake конфигурация
└── README.md