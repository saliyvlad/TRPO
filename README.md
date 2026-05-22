```markdown
# TRMM Implementation & Benchmark (BLAS Level 3)

Реализация функции TRMM (Triangular Matrix Multiply) из стандарта BLAS Level 3 с поддержкой многопоточности через OpenMP и сравнением производительности с библиотекой OpenBLAS.

## Структура проекта

```
trmm_project/
├── src/
│   ├── trmm.hpp          # Заголовочный файл с интерфейсами
│   ├── trmm.cpp          # Реализация TRMM (последовательная + OpenMP)
│   └── benchmark.cpp     # Бенчмарк производительности
├── tests/
│   └── test_trmm.cpp     # Тесты корректности (сравнение с OpenBLAS)
├── build.bat             # Скрипт сборки для Windows
├── run_tests.bat         # Запуск тестов
├── run_benchmark.bat     # Запуск бенчмарка
├── upload_to_git.bat     # Скрипт загрузки на GitHub
├── CMakeLists.txt        # Конфигурация CMake
├── .gitignore            # Исключения для Git
└── README.md             # Этот файл
```

## Быстрый старт (Windows)

### Требования
- ОС: Windows 10/11
- Компилятор: MSVC (Visual Studio 2022 Build Tools)
- CMake: версии 3.20 или новее
- vcpkg: установлен в C:/vcpkg
- Git: для работы с репозиторием

### Установка зависимостей

```powershell
# Установка vcpkg (если не установлен)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Установка OpenBLAS
.\vcpkg install openblas:x64-windows
```

### Сборка и запуск

```batch
:: Сборка проекта
build.bat

:: Запуск тестов
run_tests.bat

:: Запуск бенчмарка
run_benchmark.bat
```

Примечание: Бенчмарк для матриц 2000×2000 выполняется 1–3 минуты. Это нормально.

## Описание функции TRMM

TRMM (Triangular Matrix Multiply) выполняет операцию:

```
B := α × op(A) × B   (при SIDE = Left)
B := α × B × op(A)   (при SIDE = Right)
```

Где:
- A — треугольная матрица размера m×m
- B — общая матрица размера m×n (результат записывается в неё)
- α — скалярный множитель
- op(A) — A, Aᵀ или Aᴴ (в зависимости от параметра TRANS)

### Параметры функции

| Параметр | Возможные значения | Описание |
|----------|-------------------|----------|
| SIDE | Left, Right | Положение треугольной матрицы |
| UPLO | Upper, Lower | Какая часть матрицы треугольная |
| TRANS | None, Transpose, ConjugateTranspose | Операция над матрицей A |
| DIAG | NonUnit, Unit | Является ли диагональ единичной |
| α | float / double | Скалярный множитель |

## Тестирование

Файл `tests/test_trmm.cpp` сравнивает результаты реализации с эталонной библиотекой OpenBLAS.

Пример успешного вывода:
```
Testing TRMM implementation...
Matrix size: 100 x 100

Running custom TRMM...
Running OpenBLAS TRMM...

Comparing results...
✓ TEST PASSED: Results match OpenBLAS
```

Точность сравнения: 1×10⁻¹⁰ (для типа double).

## Бенчмарк производительности

Бенчмарк измеряет время выполнения для:
- Количества потоков: 1, 2, 4, 8, 16
- Типов данных: float (одинарная точность) и double (двойная точность)
- Сравнения с библиотекой OpenBLAS

Пример вывода:
```
SINGLE PRECISION (float)
------------------------
   Threads       Time (s)   OpenBLAS (s)  Performance %
-------------------------------------------------------
         1         45.123         1.234          2.74%
         2         22.891         1.234          5.39%
         4         11.567         1.234         10.67%
         8          6.234         1.234         19.80%
        16          4.123         1.234         29.93%
```

Цель: достичь не менее 70% производительности OpenBLAS.

## Технические особенности

### Алгоритм
- Реализован основной вариант: Left / Upper / NoTrans
- Оптимизированный порядок вычислений: сначала обновление элементов, затем масштабирование
- Скаляр α применяется один раз в конце обработки каждого столбца

### Параллелизация (OpenMP)
```cpp
#pragma omp parallel for
for (int j = 0; j < (int)n; ++j) {
    // Обработка столбца j матрицы B
    // Каждый поток работает с независимым столбцом
}
```
- Распараллеливание по столбцам матрицы B
- Отсутствие конфликтов доступа к памяти (race conditions)
- Линейное ускорение до числа физических ядер процессора

### Совместимость
- Windows: MSVC + vcpkg + OpenBLAS
- Linux: GCC/Clang + pkg-config + OpenBLAS
- Кроссплатформенная сборка через CMake

## Устранение неполадок

| Проблема | Решение |
|----------|---------|
| Не найдён cblas.h | Выполните: vcpkg install openblas:x64-windows |
| OpenBLAS target not found | Удалите папку build/ и запустите build.bat заново |
| Ошибка C3016 в OpenMP | В параллельных циклах используется int вместо size_t |
| libopenblas.dll not found | Скопируйте DLL в папку с exe-файлом |
| Тесты не проходят | Проверьте, что α применяется один раз, а не дважды |

## Автор

Салий Владислав Павлович  
Группа: ИКС-432  
Практика: Б2.О.01(У)  
2024

## Лицензия

Проект распространяется под лицензией MIT.
```
