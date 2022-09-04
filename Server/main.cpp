#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr, socket()
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()



#define PORT_NUMBER     8080        // portNum > 1024 && portNum <= 65535

enum Code {
    NO_ERROR,
    BAD_SOCKET,
    BAD_SETSOCKOPT,
    BAD_BIND,
    BAD_LISTEN,
    BAD_ACCEPT,

    MAX_RETURN_CODE
};

int main()
{
    char buffer[1024] = {'\0'};

    const char* helloString = "Hello from SERVER";
    int retValue {0};


    // #### int socket(int DOMAIN, int TYPE, int PROTOCOL);
    // ####
    // #### Функция создаёт сокет (Или оконечную точку для коммуникации?? Ведь СОКЕТ это: SERVER_IP:SERVER_PORT + CLIENT_IP:CLIENT_PORT).
    // ####         Тогда правильнее будет сказать, что функция создаёт СЕРВЕРНУЮ часть соединения (пока без адреса).
    // ####         В случае успеха возвращает дескриптор, по которому можно обращаться к СЕРВЕРНОЙ части сокета (соединения).
    // ####
    // ####   DOMAIN   - коммуникационный домен, описывающий форматы адресов и правила их интерпретации
    // ####              (AF_INET - адрес состоит из имени хоста и номера порта; AF_UNIX - адресс это допустимое имя файла).
    // ####   TYPE     - тип соединения (SOCK_STREAM - создание виртуального канала для потоковой передачи байтов;
    // ####              SOCK_DGRAM - передача датаграмм, отдельных пакетов с данными, т.е. порциями).
    // ####   PROTOCOL - тип протокола (TCP, UDP и т.д.)
    // ####
    int serverSocketPart {socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    if (serverSocketPart < 0) {
        std::perror("\n[ERROR]::[socket]");
        return Code::BAD_SOCKET;
    }
    else {
        std::cout << "\nServer end-point of a socket has been created." << std::endl;
    }


    // #### int setsockopt(int SOCKET, int LEVEL, int FLAG, const void *BUFFER, socklen_t BUFFER_LENGTH);
    // ####
    // #### Функция устанавливает флаги (опции) серверной части сокета.
    // ####
    // ####   LEVEL   - Определяет уровень, на котором будет работать флаг
    // ####             (SOL_SOCKET - непосредственно уровень сокета).
    // ####   FLAG    - Флаг(и) для установки
    // ####
    // ####   BUFFER  - буффер для хранения флагов (ниже используется просто целое число)
    // ####
    int serverSocketPartOptions {1};
    retValue = setsockopt(serverSocketPart, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &serverSocketPartOptions, sizeof(serverSocketPartOptions));
    if (retValue != 0) {
        std::perror("[ERROR]::[setsockopt]");
        return Code::BAD_SETSOCKOPT;
    }
    else {} // Nothing to do


    // #### Итак, сокет для сервера создали. Теперь нужно присвоить ему адрес, по которому с этим сокетом смогут
    // #### связываться другие процессы. То есть адрес - это не часть сокета, а просто способ обращения к нему.
    // #### Инициализируем структуру, по которой будет осуществляться доступ к нашему серверному сокету.
    // ####
    struct sockaddr_in serverSocketAddress;                       //   для локального адреса, для широковещательного адрес и т.п.)

    serverSocketAddress.sin_family      = AF_INET;                // Коммуникационный домен AF_INET (адрес = имя хоста + номер порта)
    serverSocketAddress.sin_port        = htons(PORT_NUMBER);     // Номер порта (переведённый в сетевой порядок следования байтов)
    serverSocketAddress.sin_addr.s_addr = INADDR_ANY;             // Присваиваем IP-адрес (INADDR_ANY - хотим работать со всеми IP-адресами машины).
                                                                    // Как я понимаю, есть разные адреса (для WiFi, для подключения через Ethernet,
                                                                    //   для локального адреса, для широковещательного адрес и т.п.)
    int servEndPointAddrLen {sizeof(serverSocketAddress)};


    // #### Связываем наш сокет с адресом
    // ####
    retValue = bind(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), servEndPointAddrLen);
    if (retValue < 0) {
        std::perror("[ERROR]::[bind]");
        return Code::BAD_BIND;
    }
    else {} // Nothing to do


    // #### Устанавливаем сокет в пассивное состояние, в котором он будет "слушать"
    // #### входящие соединения, использующие ф-цию accept().
    // #### [socketMaxQueue] - максимальная длина очереди из желающих присоединиться к сокету.
    // ####
    int socketMaxQueue {3};
    retValue = listen(serverSocketPart, socketMaxQueue);
    if (retValue < 0) {
        std::perror("[ERROR]::[listen]");
        return Code::BAD_LISTEN;
    }
    else {} // Nothing to do



    // #### Принимаем первый запрос из очереди желающих присоединиться к серверному сокету.
    // #### Формируем полноценный сокет (пару клиент-сервер).
    // ####
    int socket_1 {accept(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), (socklen_t*)(&servEndPointAddrLen))};
    if (socket_1 < 0) {
        std::perror("[ERROR]::[accept]");
        return Code::BAD_ACCEPT;
    }
    else {} // Nothing to do

    // #### Считываем данные, приходящие через сокет [socketClientServer]
    // ####
    retValue = read(socket_1, buffer, 1024);
    if (retValue != -1) {
        std::cout << "Data, recived from client_1: ";
        std::cout.write(buffer, retValue);

    }
    else {
        std::perror("[WARNING]::[read]");
    }

    // #### Отправляем данные через сокет @socketClientServer клиенту
    // ####
    retValue = send(socket_1, helloString, strlen(helloString), 0);
    if (retValue != -1) {
        std::cout << "\nServer send message to client_1: ";
        for (int ii {0}; ii < retValue; ++ii) {
            std::cout.put(helloString[ii]);
        }
        std::cout << std::endl;
    }
    else {
        std::perror("[WARNING]::[send]");
    }

    // #### Закрываем соединение с клиентом
    close(socket_1);

    // #### Отключаем как чтение, так и запись связанную с socketServer
    shutdown(serverSocketPart, SHUT_RDWR);
    return 0;

}







