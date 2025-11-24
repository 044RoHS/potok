#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <clocale>

class SimpleClientQueue {
private:
    std::atomic<int> clientCounter;
    int maxClients;

public:
    SimpleClientQueue(int max) : clientCounter(0), maxClients(max) {}

    void clientThread() {
        while (clientCounter < maxClients) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            int current = ++clientCounter;
            std::cout << "Клиент пришёл. В очереди: " << current << " клиентов" << std::endl;
        }
        std::cout << "Достигнуто максимальное количество клиентов (" << maxClients << "). Клиенты больше не приходят." << std::endl;
    }

    void operatorThread() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(2));

            if (clientCounter > 0) {
                int current = --clientCounter;
                std::cout << "Операционист обслужил клиента. В очереди: " << current << " клиентов" << std::endl;
            }

            
            if (clientCounter == 0 && clientCounter >= maxClients) {
                std::cout << "Все клиенты обслужены. Операционист завершает работу." << std::endl;
                break;
            }
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    const int MAX_CLIENTS = 10;

    SimpleClientQueue queue(MAX_CLIENTS);

    std::cout << "Начало работы. Максимальное количество клиентов: " << MAX_CLIENTS << std::endl;

    std::thread clientThread(&SimpleClientQueue::clientThread, &queue);
    std::thread operatorThread(&SimpleClientQueue::operatorThread, &queue);

    clientThread.join();
    operatorThread.join();

    std::cout << "Программа завершена." << std::endl;

    return 0;
}
