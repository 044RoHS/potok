#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>


#define CSI "\033["  

void worker(int threadNum, int totalThreads, int steps) {
    using namespace std::chrono;

    auto start = steady_clock::now();

 
    {
        std::cout << "Thread " << std::setw(2) << threadNum
            << " | ID: " << std::this_thread::get_id()
            << " | [" << std::string(steps, ' ') << "]"
            << std::endl; 
    }

    
    std::this_thread::sleep_for(std::chrono::milliseconds(50 * threadNum));

    for (int i = 1; i <= steps; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

        
        int linesUp = totalThreads - threadNum + 1;
        std::cout << CSI << linesUp << "A"; 

        std::cout << "\r";

        std::string bar(i, '#');
        bar.resize(steps, ' '); 

        std::cout << "Thread " << std::setw(2) << threadNum
            << " | ID: " << std::this_thread::get_id()
            << " | [" << bar << "]"
            << CSI << "K" 
            << std::flush;

        
        std::cout << CSI << linesUp << "B"; 
    }

    auto end = steady_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(end - start);

 
    {
        int linesUp = totalThreads - threadNum + 1;
        std::cout << CSI << linesUp << "A";
        std::cout << "\r";
        std::cout << "Thread " << std::setw(2) << threadNum
            << " | ID: " << std::this_thread::get_id()
            << " | [" << std::string(steps, '#') << "]"
            << " Done in " << duration.count() << " ms"
            << CSI << "K"
            << std::flush;
        std::cout << CSI << linesUp << "B";
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    const int numThreads = 5;
    const int workSteps = 20;

    std::cout << "Starting " << numThreads << " threads, each with " << workSteps << " steps...\n";
    std::cout << std::string(60, '-') << "\n";

    
    for (int i = 1; i <= numThreads; ++i) {
        std::cout << "Thread " << std::setw(2) << i
            << " | ID: " << std::this_thread::get_id() 
            << " | [" << std::string(workSteps, ' ') << "]"
            << std::endl;
    }

 
    std::cout << CSI << numThreads << "A" << std::flush;

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (int i = 1; i <= numThreads; ++i) {
        threads.emplace_back(worker, i, numThreads, workSteps);
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    std::cout << "\n\nAll threads completed.\n";
    return 0;
}