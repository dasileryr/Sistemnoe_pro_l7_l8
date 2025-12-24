#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <mutex>
#include <fstream>
#include <iomanip>

const int ARRAY_SIZE = 10000;

// Глобальные переменные для результатов
std::mutex resultMutex;
std::mutex fileMutex;
int maxValue = INT_MIN;
int minValue = INT_MAX;
long long sum = 0;
std::vector<int> numbers;

void findMax(const std::vector<int>& data, int start, int end) {
    int localMax = INT_MIN;
    for (int i = start; i < end; i++) {
        if (data[i] > localMax) {
            localMax = data[i];
        }
    }
    
    std::lock_guard<std::mutex> lock(resultMutex);
    if (localMax > maxValue) {
        maxValue = localMax;
    }
    std::cout << "Поток поиска максимума завершил работу. Локальный максимум: " << localMax << std::endl;
}

void findMin(const std::vector<int>& data, int start, int end) {
    int localMin = INT_MAX;
    for (int i = start; i < end; i++) {
        if (data[i] < localMin) {
            localMin = data[i];
        }
    }
    
    std::lock_guard<std::mutex> lock(resultMutex);
    if (localMin < minValue) {
        minValue = localMin;
    }
    std::cout << "Поток поиска минимума завершил работу. Локальный минимум: " << localMin << std::endl;
}

void findSum(const std::vector<int>& data, int start, int end) {
    long long localSum = 0;
    for (int i = start; i < end; i++) {
        localSum += data[i];
    }
    
    std::lock_guard<std::mutex> lock(resultMutex);
    sum += localSum;
    std::cout << "Поток вычисления суммы завершил работу. Локальная сумма: " << localSum << std::endl;
}

void writeToFile(const std::string& filename, double average) {
    std::lock_guard<std::mutex> lock(fileMutex);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }
    
    file << "=== РЕЗУЛЬТАТЫ ВЫЧИСЛЕНИЙ ===" << std::endl;
    file << "Размер массива: " << ARRAY_SIZE << std::endl;
    file << "\n=== МАССИВ ЧИСЕЛ ===" << std::endl;
    
    // Записываем числа в файл (по 10 чисел в строке для читаемости)
    for (int i = 0; i < ARRAY_SIZE; i++) {
        file << std::setw(8) << numbers[i];
        if ((i + 1) % 10 == 0) {
            file << std::endl;
        } else {
            file << " ";
        }
    }
    if (ARRAY_SIZE % 10 != 0) {
        file << std::endl;
    }
    
    file << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;
    file << "Максимум: " << maxValue << std::endl;
    file << "Минимум: " << minValue << std::endl;
    file << "Среднее: " << std::fixed << std::setprecision(2) << average << std::endl;
    file << "Сумма: " << sum << std::endl;
    
    file.close();
    std::cout << "Поток записи в файл завершил работу. Данные записаны в " << filename << std::endl;
}

int main() {
    std::cout << "Задание 5: Поиск максимума, минимума и среднего с записью в файл" << std::endl;
    
    // Генерируем массив случайных чисел
    numbers.resize(ARRAY_SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100000);
    
    std::cout << "Генерация массива из " << ARRAY_SIZE << " элементов..." << std::endl;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        numbers[i] = dis(gen);
    }
    std::cout << "Массив сгенерирован." << std::endl;
    
    // Разделяем работу между тремя потоками
    int chunkSize = ARRAY_SIZE / 3;
    
    // Создаем три потока для поиска максимума, минимума и суммы
    std::thread maxThread1(findMax, std::ref(numbers), 0, chunkSize);
    std::thread maxThread2(findMax, std::ref(numbers), chunkSize, 2 * chunkSize);
    std::thread maxThread3(findMax, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE);
    
    std::thread minThread1(findMin, std::ref(numbers), 0, chunkSize);
    std::thread minThread2(findMin, std::ref(numbers), chunkSize, 2 * chunkSize);
    std::thread minThread3(findMin, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE);
    
    std::thread sumThread1(findSum, std::ref(numbers), 0, chunkSize);
    std::thread sumThread2(findSum, std::ref(numbers), chunkSize, 2 * chunkSize);
    std::thread sumThread3(findSum, std::ref(numbers), 2 * chunkSize, ARRAY_SIZE);
    
    // Ждем завершения всех потоков вычислений
    maxThread1.join();
    maxThread2.join();
    maxThread3.join();
    minThread1.join();
    minThread2.join();
    minThread3.join();
    sumThread1.join();
    sumThread2.join();
    sumThread3.join();
    
    // Вычисляем среднее
    double average = static_cast<double>(sum) / ARRAY_SIZE;
    
    // Выводим результаты в консоль
    std::cout << "\n=== РЕЗУЛЬТАТЫ ===" << std::endl;
    std::cout << "Максимум: " << maxValue << std::endl;
    std::cout << "Минимум: " << minValue << std::endl;
    std::cout << "Среднее: " << average << std::endl;
    std::cout << "Сумма: " << sum << std::endl;
    
    // Создаем поток для записи в файл
    std::string filename = "results.txt";
    std::thread fileThread(writeToFile, filename, average);
    
    // Ждем завершения потока записи
    fileThread.join();
    
    std::cout << "\nВсе потоки завершены. Результаты сохранены в файл " << filename << std::endl;
    
    return 0;
}

