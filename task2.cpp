#include <iostream>
#include <thread>
#include <chrono>

void printNumbers(int start, int end) {
    std::cout << "Поток начал работу. Диапазон: [" << start << ", " << end << "]" << std::endl;
    for (int i = start; i <= end; i++) {
        std::cout << i << " ";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << std::endl << "Поток завершил работу." << std::endl;
}

int main() {
    int start, end;
    
    std::cout << "Задание 2: Поток выводит числа в заданном диапазоне" << std::endl;
    std::cout << "Введите начало диапазона: ";
    std::cin >> start;
    std::cout << "Введите конец диапазона: ";
    std::cin >> end;
    
    if (start > end) {
        std::cout << "Ошибка: начало диапазона больше конца!" << std::endl;
        return 1;
    }
    
    // Создаем поток для вывода чисел
    std::thread t(printNumbers, start, end);
    
    // Ждем завершения потока
    t.join();
    
    std::cout << "Программа завершена." << std::endl;
    return 0;
}

