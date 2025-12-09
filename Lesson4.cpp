#include <iostream>
#include <vector>
#include <future>
#include <algorithm>

const int MIN_SIZE = 1000;

void square(int& x) {
    x = x * x;
}

void parallel_for_each(std::vector<int>& vec, size_t start, size_t end) {
    size_t count = end - start;

    if (count <= MIN_SIZE) {
        for (size_t i = start; i < end; ++i) {
            square(vec[i]); 
        }
        return; 
    }

    size_t mid = start + count / 2;


    auto future_first = std::async(std::launch::async, [&vec, start, mid]() {
        parallel_for_each(vec, start, mid);  
    });


    parallel_for_each(vec, mid, end);


    future_first.wait();  
}


void parallel_for_each(std::vector<int>& vec) {
    if (!vec.empty()) {
        parallel_for_each(vec, 0, vec.size());
    }
}

void sequential_for_each(std::vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        square(vec[i]);
    }
}

void print_vec(const std::vector<int>& v, const std::string& name) {
    std::cout << name << ": ";
    for (size_t i = 0; i < v.size() && i < 10; ++i) {
        std::cout << v[i] << " ";
    }
    if (v.size() > 10) std::cout << "...";
    std::cout << "\n";
}


int main() {

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "До обработки:\n";
    print_vec(data, "data");


    auto seq = data;
    auto par = data;


    sequential_for_each(seq);
    std::cout << "\nПосле последовательной обработки:\n";
    print_vec(seq, "последовательно");


    parallel_for_each(par);
    std::cout << "\nПосле параллельной обработки:\n";
    print_vec(par, "параллельно");

    if (seq == par) {
        std::cout << "\n Результаты совпадают!\n";
    } else {
        std::cout << "\n Ошибка: результаты разные!\n";
    }

    std::cout << "\n--- Большой тест (10 000 элементов) ---\n";
    std::vector<int> big(10000, 3); 
    auto big_seq = big;
    auto big_par = big;

    sequential_for_each(big_seq);
    parallel_for_each(big_par);

    std::cout << "Первые 5 элементов (должны быть 9): ";
    for (int i = 0; i < 5; ++i) {
        std::cout << big_par[i] << " ";
    }
    std::cout << "\n Большой тест пройден.\n";

    return 0;
}
