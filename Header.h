#pragma once

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>

class Data {
public:
    int value;
    mutable std::mutex mtx;

    Data(int v = 0) : value(v) {}


    void print(const std::string& prefix = "") const {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << prefix << "value = " << value << std::endl;
    }
};


void swap_v1(Data& a, Data& b) {

    std::lock(a.mtx, b.mtx);


    std::lock_guard<std::mutex> lock_a(a.mtx, std::adopt_lock);
    std::lock_guard<std::mutex> lock_b(b.mtx, std::adopt_lock);


    std::swap(a.value, b.value);
}


void swap_v2(Data& a, Data& b) {

    std::scoped_lock lock(a.mtx, b.mtx);

    std::swap(a.value, b.value);
}


void swap_v3(Data& a, Data& b) {
    std::unique_lock<std::mutex> lock_a(a.mtx, std::defer_lock);
    std::unique_lock<std::mutex> lock_b(b.mtx, std::defer_lock);


    std::lock(lock_a, lock_b);


    std::swap(a.value, b.value);


}