#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>     
#include <sys/socket.h>  
#include <netinet/in.h> 


void handle_client(int client_socket) {
    std::cout << " Запущен поток для обработки клиента (сокет = " << client_socket << ")\n";

    char buffer[1024] = { 0 };

   
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  
        std::cout << " Получено от клиента: \"" << buffer << "\"\n";

       
        std::string reply = "Сервер получил: \"" + std::string(buffer) + "\"\n";
        
        send(client_socket, reply.c_str(), reply.length(), 0);
        std::cout << " Ответ отправлен.\n";
    }
    else if (bytes_received == 0) {
        std::cout << " Клиент закрыл соединение.\n";
    }
    else {
        std::cerr << " Ошибка приёма данных от клиента.\n";
    }

   
    close(client_socket);
    std::cout << " Поток завершил работу с клиентом.\n";
}

int main() {
    const int PORT = 8080;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

   
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << " Ошибка: не удалось создать сокет.\n";
        return 1;
    }
    std::cout << " Серверный сокет создан.\n";

   
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(PORT);

   
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << " Ошибка bind(): порт " << PORT << " уже занят?\n";
        close(server_socket);
        return 1;
    }
    std::cout << " Сокет привязан к порту " << PORT << ".\n";

    
    if (listen(server_socket, 5) == -1) {
        std::cerr << " Ошибка listen().\n";
        close(server_socket);
        return 1;
    }
    std::cout << " Сервер запущен и ждёт подключений...\n";
    std::cout << " Подключайтесь через: telnet 127.0.0.1 " << PORT
        << "  или через ваш TCP-клиент.\n\n";

    
    while (true) {
        
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            std::cerr << " Ошибка accept().\n";
            continue;
        }

        std::cout << "\n Принято новое подключение! (сокет = " << client_socket << ")\n";

        
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();  
    }

   
    close(server_socket);
    return 0;
}
