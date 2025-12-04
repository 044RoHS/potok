#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <cmath>

void apply_to_range(std::vector<int>& vec, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        vec[i] = vec[i] * vec[i];  
    }
}

void parallel_for_each(std::vector<int>& vec, int num_threads = 4) {
    size_t n = vec.size();
    if (n == 0) return;

    if (num_threads > static_cast<int>(n)) {
        num_threads = static_cast<int>(n);
    }

    size_t chunk_size = n / num_threads;
    size_t remainder = n % num_threads;  

    std::vector<std::future<void>> futures;

    size_t start = 0;

    
    for (int t = 0; t < num_threads - 1; ++t) {
        size_t current_chunk = chunk_size;
        if (t < static_cast<int>(remainder)) {
            current_chunk++;  
        }
        size_t end = start + current_chunk;
        futures.push_back(std::async(std::launch::async, [&vec, start, end]() {
            apply_to_range(vec, start, end);
            }));

        start = end;  
    }
    size_t end = n;
    apply_to_range(vec, start, end);
    for (auto& fut : futures) {
        fut.wait();
    }
}
void sequential_for_each(std::vector<int>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        vec[i] = vec[i] * vec[i];
    }
}


void print_vector(const std::vector<int>& vec, const std::string& label) {
    std::cout << label << ": ";
    size_t count = std::min<size_t>(10, vec.size());
    for (size_t i = 0; i < count; ++i) {
        std::cout << vec[i] << " ";
    }
    if (vec.size() > 10) std::cout << "...";
    std::cout << "\n";
}


int main() {
   
    std::vector<int> data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    std::cout << "До обработки:\n";
    print_vector(data, "data");


    std::vector<int> data_seq = data;
    std::vector<int> data_par = data;


    sequential_for_each(data_seq);
    std::cout << "\nПосле последовательной обработки:\n";
    print_vector(data_seq, "data_seq");

   
    parallel_for_each(data_par, 4);
    std::cout << "\nПосле параллельной обработки (4 потока):\n";
    print_vector(data_par, "data_par");

    
    bool same = (data_seq == data_par);
    std::cout << "\nРезультаты совпадают? " << (same ? "ДА " : "НЕТ ") << "\n";

    return 0;
}