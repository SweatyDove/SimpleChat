#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr
//#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()
#include <arpa/inet.h>

#define MY_PORT 8080
#define IPv4_LOCAL_HOST "127.0.0.1"               // Сетевой адрес самой машины
int main()
{
    //int clientFD {};v4
    int conn {0};
    int targetFD {0};
    int valSend {0};
    int valRead {0};
    char buffer[1024] = {'\0'};
    const char* strFromClient = "Hello from CLIENT!";
    struct sockaddr_in serverAddress;       // Описывает адрес интернет сокета (IPv4)

    // Создаём TCP-сокет (SOCK_STREAM) по протоколу IPv4 (AF_INET)
    // и присваиваем ему IP-адрес автоматически (последний аргумент равен 0)
    targetFD = socket(AF_INET, SOCK_STREAM, 0);
    if(targetFD < 0) {
        std::cout << "\n[ERROR]::[socket]: Socket creation error" << std::endl;
        return -1;
    }
    else {
        std::cout << "\nCreate a socket" << std::endl;
    }

    // #### Заполняем структуру, описывающую адрес интернет сокета
    serverAddress.sin_family = AF_INET;             // Семья адресов формата IPv4
    serverAddress.sin_port = htons(MY_PORT);
    // ## Преобразуем наш локальный адрес (IPv4_LOCAL_HOST) в сетевой адрес типа IPv4 (семья AF_INET)
    // ## и записываем его в структуру адреса интернет сокета. (Конвертируем IPV4 или IPv6 из текстовой
    // ## формы в бинарную).
    if (inet_pton(AF_INET, IPv4_LOCAL_HOST, &serverAddress.sin_addr) <= 0) {
        std::cout << "\n[ERROR]::[inet_pton]: Invalid address/address not supported" << std::endl;
        return -1;
    }
    else {} // Nothing to do

    // #### Установка соединения между сокетом [targetFD] и целевым сокетом (задан его адрес,
    // #### т.е. адрес сервера, куда хотим подключиться)
    conn = connect(targetFD, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
    if (conn < 0) {
        std::cout << "\n[ERROR]::[connect]: connection failed." << std::endl;
        return -1;
    }
    else {
        std::cout << "\nConnection to the server established." << std::endl;
    }

    // Отправляем сообщение на сокет [sock]. Последний аргумент функции равен нулю
    // (что означает запись в сокет в порядке поступления байтов)
    valSend = send(targetFD, strFromClient, strlen(strFromClient), 0);
    if (valSend < 0) {
        std::perror("[ERROR]::[send]");
        return -1;
    }
    else {
        std::cout << "\nNext message send to server: ";
        for (int ii {0}; ii < valSend; ++ii) {
            std::cout.put(strFromClient[ii]);
        }
        std::cout << std::endl;
    }

    // Считываем сообщение от сервера
    valRead = read(targetFD, buffer, 1024);
    if (valRead != -1) {
        std::cout << "Got server response: ";
        std::cout.write(buffer, valRead);
        std::cout << std::endl;
    }
    else {
        std::perror("[WARNING]::[read]");
    }

    close(conn);
    return 0;
}
