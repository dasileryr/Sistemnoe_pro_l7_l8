#!/bin/bash

# Скрипт для компиляции всех заданий

echo "Компиляция консольных приложений..."

# Компиляция консольных приложений
g++ -std=c++11 -Wall -Wextra -pthread -o task1 task1.cpp
if [ $? -eq 0 ]; then
    echo "✓ task1 скомпилирован успешно"
else
    echo "✗ Ошибка компиляции task1"
fi

g++ -std=c++11 -Wall -Wextra -pthread -o task2 task2.cpp
if [ $? -eq 0 ]; then
    echo "✓ task2 скомпилирован успешно"
else
    echo "✗ Ошибка компиляции task2"
fi

g++ -std=c++11 -Wall -Wextra -pthread -o task3 task3.cpp
if [ $? -eq 0 ]; then
    echo "✓ task3 скомпилирован успешно"
else
    echo "✗ Ошибка компиляции task3"
fi

g++ -std=c++11 -Wall -Wextra -pthread -o task4 task4.cpp
if [ $? -eq 0 ]; then
    echo "✓ task4 скомпилирован успешно"
else
    echo "✗ Ошибка компиляции task4"
fi

g++ -std=c++11 -Wall -Wextra -pthread -o task5 task5.cpp
if [ $? -eq 0 ]; then
    echo "✓ task5 скомпилирован успешно"
else
    echo "✗ Ошибка компиляции task5"
fi

echo ""
echo "Попытка компиляции Qt приложений..."

# Проверяем наличие Qt5
if command -v pkg-config &> /dev/null && pkg-config --exists Qt5Widgets; then
    QT_CFLAGS=$(pkg-config --cflags Qt5Widgets)
    QT_LIBS=$(pkg-config --libs Qt5Widgets)
    
    g++ -std=c++11 -Wall -Wextra -pthread -fPIC $QT_CFLAGS -o task6 task6.cpp $QT_LIBS
    if [ $? -eq 0 ]; then
        echo "✓ task6 скомпилирован успешно"
    else
        echo "✗ Ошибка компиляции task6"
    fi
    
    g++ -std=c++11 -Wall -Wextra -pthread -fPIC $QT_CFLAGS -o task7 task7.cpp $QT_LIBS
    if [ $? -eq 0 ]; then
        echo "✓ task7 скомпилирован успешно"
    else
        echo "✗ Ошибка компиляции task7"
    fi
else
    echo "⚠ Qt5 не найден. Пропуск компиляции task6 и task7"
    echo "  Для компиляции Qt приложений установите Qt5:"
    echo "  Ubuntu/Debian: sudo apt-get install qt5-default libqt5widgets5-dev"
    echo "  macOS: brew install qt5"
fi

echo ""
echo "Готово!"

