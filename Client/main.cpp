#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr
//#include <netinet/in.h>       // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()
#include <arpa/inet.h>

#define PORT_NUMBER 8080
#define TARGET_IP "127.0.0.1"               // Сетевой адрес самой машины

enum RetCode {
    NO_ERROR,
    BAD_SOCKET,
    BAD_SETSOCKOPT,
    BAD_INET_PTON,
    BAD_CONNECT,
    BAD_BIND,
    BAD_LISTEN,
    BAD_ACCEPT,

    MAX_RETURN_RetCode
};

int main()
{
    int retValue {0};
    char buffer[1024] = {'\0'};
    const char* strFromClient = "Hello from CLIENT!";


    // #### int socket(int DOMAIN, int TYPE, int PROTOCOL);
    // ####
    // #### Функция создаёт клиентскую часть сокета (пока без локального адреса).
    // #### Сам же сокет состоит из 3(5) частей: SERVER_IP:SERVER_PORT + CLIENT_IP:CLIENT_PORT (+ протокол взаимодействия).
    // #### В случае успеха возвращает дескриптор, по которому можно обращаться к клиентской части сокета (соединения).
    // ####
    // ####   DOMAIN   - коммуникационный домен, описывающий форматы адресов и правила их интерпретации
    // ####              (AF_INET - адрес состоит из имени хоста и номера порта; AF_UNIX - адрес это допустимое имя файла).
    // ####   TYPE     - тип соединения (SOCK_STREAM - создание виртуального канала для потоковой передачи байтов;
    // ####              SOCK_DGRAM - передача датаграмм, отдельных пакетов с данными, т.е. порциями).
    // ####   PROTOCOL - тип протокола (TCP, UDP и т.д.)
    // ####
    int clientSocket {socket(AF_INET, SOCK_STREAM, 0)};
    if (clientSocket < 0) {
        std::perror("[ERROR]::[socket]");
        return RetCode::BAD_SOCKET;
    }
    else {
        std::cout << "\nClient end-point of socket has been created." << std::endl;
    }


    // #### Инициализируем структуру, которая будет содержать адрес СЕРВЕРНОЙ части нашего
    // #### соединения (сокета). По этому адресу мы будем пытаться связать нашу КЛИЕНТСКУЮ часть
    // #### сокета с серверной частью. И в случае успеха получим полноценный СОКЕТ.
    // ####

    struct sockaddr_in serverSocketAddress;

    serverSocketAddress.sin_family = AF_INET;               // Коммуникационный домен AF_INET (где адрес = имя хоста + номер порта)
    serverSocketAddress.sin_port   = htons(PORT_NUMBER);    // Номер порта (переведённый в сетевой порядок следования байтов)

    // Конвертируем IP-адрес из текстовой формы в бинарную (сетевой порядок байт) в нужном
    // нам исходном формате (так как задан параметр AF_INET, то в формат адресов семейства IPv4)
    if (inet_pton(AF_INET, TARGET_IP, &serverSocketAddress.sin_addr) <= 0) {
       std::perror("[ERROR]::[inet_pton]");
       return RetCode::BAD_INET_PTON;
    }
    else {} // Nothing to do


    // #### Установка соединения через клиентскую часть сокета с сервером
    // #### (который задан через адрес для подключения).
    // #### Но откуда тогда берём клиентский адрес? Нужен ли он нам? То есть с какого IP::порта исходит подключение?
    // ####
    int conn {connect(clientSocket, (struct sockaddr*) &serverSocketAddress, sizeof(serverSocketAddress))};
    if (conn < 0) {
        std::perror("[ERROR]::[connect]");
        return RetCode::BAD_CONNECT;
    }
    else {
        std::cout << "\nConnection to the server established." << std::endl;
    }

    // #### Отправляем сообщение через клиентский сокет. Последний аргумент функции равен
    // #### нулю, что означает запись в сокет в порядке поступления байтов.
    // ####
    retValue = send(clientSocket, strFromClient, strlen(strFromClient), 0);
    if (retValue < 0) {
        std::perror("[WARNING]::[send]");
    }
    else {
        std::cout << "\nNext message send to server: ";
        for (int ii {0}; ii < retValue; ++ii) {
            std::cout.put(strFromClient[ii]);
        }
        std::cout << std::endl;
    }

    // Считываем сообщение от сервера
    retValue = read(clientSocket, buffer, 1024);
    if (retValue != -1) {
        std::cout << "\nGot server response: ";
        std::cout.write(buffer, retValue);
        std::cout << std::endl;
    }
    else {
        std::perror("[WARNING]::[read]");
    }

    close(conn);
    shutdown(clientSocket, SHUT_RDWR);
    return 0;
}
