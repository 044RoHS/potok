#include <iostream>
#include <vector>
#include <future>
#include <thread>

std::future<size_t> findMinIndexAsync(const std::vector<int>& arr, size_t start, size_t end) {
    std::promise<size_t> prom;
    auto fut = prom.get_future();

    std::thread([prom = std::move(prom), &arr, start, end]() mutable {
        size_t minIndex = start;
        for (size_t i = start + 1; i < end; ++i) {
            if (arr[i] < arr[minIndex]) {
                minIndex = i;
            }
        }
        prom.set_value(minIndex);
        }).detach();  

    return fut;
}
void selectionSortAsync(std::vector<int>& arr) {
    const size_t n = arr.size();
    for (size_t i = 0; i < n - 1; ++i) {
       
        auto fut = findMinIndexAsync(arr, i, n);

        size_t minIndex = fut.get();

      
        if (minIndex != i) {
            std::swap(arr[i], arr[minIndex]);
        }
    }
}
int main() {
    std::vector<int> data = { 64, 25, 12, 22, 11 };
    std::cout << "Before: ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";

    selectionSortAsync(data);

    std::cout << "After:  ";
    for (int x : data) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}