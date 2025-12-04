#include "Header.h"

int main() {
    Data d1(10), d2(20), d3(30), d4(40);

    std::cout << "Before swap:\n";
    d1.print("d1: ");
    d2.print("d2: ");
    d3.print("d3: ");
    d4.print("d4: ");

   
    std::vector<std::thread> threads;

  
    threads.emplace_back([&d1, &d2]() {
        for (int i = 0; i < 1000; ++i) {
            swap_v1(d1, d2);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        });

 
    threads.emplace_back([&d3, &d4]() {
        for (int i = 0; i < 1000; ++i) {
            swap_v2(d3, d4);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        });

   
    threads.emplace_back([&d2, &d3]() {
        for (int i = 0; i < 1000; ++i) {
            swap_v3(d2, d3);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
        });

    for (auto& t : threads) t.join();

    std::cout << "\nAfter concurrent swaps:\n";
    d1.print("d1: ");
    d2.print("d2: ");
    d3.print("d3: ");
    d4.print("d4: ");

    return 0;
}