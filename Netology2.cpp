#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>

class VectorSumCalculator {
private:
    std::vector<int> vector1;
    std::vector<int> vector2;
    std::vector<int> result;
    size_t size;

public:
    VectorSumCalculator(size_t n) : size(n) {
        // Инициализируем векторы случайными числами
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1, 100);

        vector1.resize(size);
        vector2.resize(size);
        result.resize(size);

        for (size_t i = 0; i < size; ++i) {
            vector1[i] = dist(gen);
            vector2[i] = dist(gen);
        }
    }

    // Последовательное вычисление (1 поток)
    void sequentialSum() {
        for (size_t i = 0; i < size; ++i) {
            result[i] = vector1[i] + vector2[i];
        }
    }

    // Параллельное вычисление
    void parallelSum(int numThreads) {
        std::vector<std::thread> threads;

        size_t chunkSize = size / numThreads;
        size_t remainder = size % numThreads;

        size_t start = 0;

        for (int i = 0; i < numThreads; ++i) {
            size_t end = start + chunkSize + (i < remainder ? 1 : 0);

            threads.emplace_back([this, start, end]() {
                for (size_t j = start; j < end; ++j) {
                    result[j] = vector1[j] + vector2[j];
                }
                });

            start = end;
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    // Проверка корректности результатов
    bool verifyResults() {
        std::vector<int> sequentialResult(size);
        for (size_t i = 0; i < size; ++i) {
            sequentialResult[i] = vector1[i] + vector2[i];
        }
        return sequentialResult == result;
    }
};

// Функция для измерения времени выполнения
template<typename Func>
double measureTime(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    return duration.count();
}

int main() {
    // Устанавливаем локаль для вывода кириллицы
    setlocale(LC_ALL, "Russian");

    // Выводим количество аппаратных ядер
    unsigned int numCores = std::thread::hardware_concurrency();
    std::cout << "Доступное количество аппаратных ядер: " << numCores << std::endl;
    std::cout << std::endl;

    // Размеры массивов для тестирования
    std::vector<size_t> sizes = { 1000, 10000, 100000, 1000000 };

    // Количество потоков для тестирования
    std::vector<int> threadCounts = { 1, 2, 4, 8, 16 };

    // Создаем таблицу для результатов
    std::vector<std::vector<double>> results(
        threadCounts.size(),
        std::vector<double>(sizes.size(), 0.0)
    );

    // Запускаем тесты
    for (size_t sizeIdx = 0; sizeIdx < sizes.size(); ++sizeIdx) {
        size_t size = sizes[sizeIdx];
        std::cout << "Тестирование для размера: " << size << " элементов" << std::endl;

        VectorSumCalculator calculator(size);

        for (size_t threadIdx = 0; threadIdx < threadCounts.size(); ++threadIdx) {
            int numThreads = threadCounts[threadIdx];

            double time = measureTime([&]() {
                if (numThreads == 1) {
                    calculator.sequentialSum();
                }
                else {
                    calculator.parallelSum(numThreads);
                }
                });

            results[threadIdx][sizeIdx] = time;

            // Проверяем корректность результатов
            if (!calculator.verifyResults()) {
                std::cout << "Ошибка: некорректные результаты для "
                    << numThreads << " потоков!" << std::endl;
            }
            std::cout << "  " << numThreads << " потоков: "
                << std::fixed << std::setprecision(6) << time << "s" << std::endl;
        }
        std::cout << std::endl;
    }

    // Выводим итоговую таблицу
    std::cout << "ИТОГОВАЯ ТАБЛИЦА ПРОИЗВОДИТЕЛЬНОСТИ" << std::endl;
    std::cout << "====================================" << std::endl;

    // Заголовок таблицы
    std::cout << std::setw(8) << "Потоки" << " |";
    for (size_t size : sizes) {
        std::cout << std::setw(12) << size << " |";
    }
    std::cout << std::endl;

    // Разделитель
    std::cout << std::string(8 + (sizes.size() * 15), '-') << std::endl;

    // Данные таблицы
    for (size_t threadIdx = 0; threadIdx < threadCounts.size(); ++threadIdx) {
        std::cout << std::setw(8) << threadCounts[threadIdx] << " |";
        for (size_t sizeIdx = 0; sizeIdx < sizes.size(); ++sizeIdx) {
            std::cout << std::setw(12) << std::fixed << std::setprecision(6)
                << results[threadIdx][sizeIdx] << " |";
        }
        std::cout << std::endl;
    }

    // Анализ оптимального количества потоков
    std::cout << std::endl << "АНАЛИЗ ОПТИМАЛЬНОГО КОЛИЧЕСТВА ПОТОКОВ:" << std::endl;
    std::cout << "====================================" << std::endl;

    for (size_t sizeIdx = 0; sizeIdx < sizes.size(); ++sizeIdx) {
        double minTime = results[0][sizeIdx];
        int bestThreads = 1;

        for (size_t threadIdx = 1; threadIdx < threadCounts.size(); ++threadIdx) {
            if (results[threadIdx][sizeIdx] < minTime) {
                minTime = results[threadIdx][sizeIdx];
                bestThreads = threadCounts[threadIdx];
            }
        }

        std::cout << "Размер " << std::setw(8) << sizes[sizeIdx]
            << ": оптимально " << bestThreads << " потоков ("
            << std::fixed << std::setprecision(6) << minTime << "s)" << std::endl;
    }

    return 0;
}
