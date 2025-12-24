#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void printNumbers(int threadId, int start, int end) {
    std::cout << "Поток " << threadId << " начал работу. Диапазон: [" << start << ", " << end << "]" << std::endl;
    for (int i = start; i <= end; i++) {
        std::cout << "[Поток " << threadId << "] " << i << " ";
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    std::cout << std::endl << "Поток " << threadId << " завершил работу." << std::endl;
}

int main() {
    int numThreads, start, end;
    
    std::cout << "Задание 3: Несколько потоков выводят числа в заданном диапазоне" << std::endl;
    std::cout << "Введите количество потоков: ";
    std::cin >> numThreads;
    std::cout << "Введите начало диапазона: ";
    std::cin >> start;
    std::cout << "Введите конец диапазона: ";
    std::cin >> end;
    
    if (numThreads <= 0) {
        std::cout << "Ошибка: количество потоков должно быть больше 0!" << std::endl;
        return 1;
    }
    
    if (start > end) {
        std::cout << "Ошибка: начало диапазона больше конца!" << std::endl;
        return 1;
    }
    
    int range = end - start + 1;
    int numbersPerThread = range / numThreads;
    int remainder = range % numThreads;
    
    std::vector<std::thread> threads;
    
    // Распределяем диапазон между потоками
    int currentStart = start;
    for (int i = 0; i < numThreads; i++) {
        int currentEnd = currentStart + numbersPerThread - 1;
        if (i < remainder) {
            currentEnd++; // Распределяем остаток по первым потокам
        }
        
        threads.emplace_back(printNumbers, i + 1, currentStart, currentEnd);
        currentStart = currentEnd + 1;
    }
    
    // Ждем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Все потоки завершены." << std::endl;
    return 0;
}

