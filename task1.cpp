#include <iostream>
#include <thread>
#include <chrono>

void printNumbers(int start, int end) {
    for (int i = start; i <= end; i++) {
        std::cout << i << " ";
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Задание 1: Поток выводит числа от 0 до 50" << std::endl;
    
    // Создаем поток для вывода чисел
    std::thread t(printNumbers, 0, 50);
    
    // Ждем завершения потока
    t.join();
    
    std::cout << "Поток завершен." << std::endl;
    return 0;
}

