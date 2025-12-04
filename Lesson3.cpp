#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>     
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>       

int main() {
 
    const char* server_ip = "127.0.0.1";
    int server_port = 8080;                
    const char* message = "Привет, сервер! Это тестовое сообщение от клиента.\n";

  
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Ошибка: не удалось создать сокет\n";
        return 1;
    }
    std::cout << " Сокет создан.\n";


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);              
    server_addr.sin_addr.s_addr = inet_addr(server_ip);    

   
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        struct hostent* host = gethostbyname(server_ip);
        if (host == nullptr) {
            std::cerr << "Ошибка: неизвестный хост '" << server_ip << "'\n";
            close(client_socket);
            return 1;
        }
        memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    }

    std::cout << " Подключаюсь к " << server_ip << ":" << server_port << "...\n";
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << " Ошибка подключения к серверу. Убедитесь, что сервер запущен!\n";
        close(client_socket);
        return 1;
    }
    std::cout << " Успешно подключено!\n";


    std::cout << " Отправляю: \"" << message << "\"\n";
    if (send(client_socket, message, strlen(message), 0) == -1) {
        std::cerr << " Ошибка отправки данных\n";
        close(client_socket);
        return 1;
    }

    char buffer[1024] = { 0 };  
    std::cout << " Жду ответ от сервера...\n";
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        std::cerr << " Ошибка приёма данных\n";
        close(client_socket);
        return 1;
    }
    else if (bytes_received == 0) {
        std::cout << " Сервер закрыл соединение (получено 0 байт)\n";
    }
    else {
        buffer[bytes_received] = '\0';  
        std::cout << " Получен ответ (" << bytes_received << " байт):\n";
        std::cout << "\"" << buffer << "\"\n";
    }


    close(client_socket);
    std::cout << "Соединение закрыто.\n";

    return 0;
}

