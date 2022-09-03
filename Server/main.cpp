#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()
#define MY_PORT     8080

enum ReturnCode {
    NO_ERROR,
    BAD_SOCKET,

    MAX_RETURN_CODE
};

int main()
{
    //int portNum {MY_PORT};              // Какой-то порт (главное, portNum > 1024 && portNum <= 65535)
    int socketMaxQueue {3};             // Максимальная длина очереди из желающих присоединиться к сокету
    int newSocketFD {};                 // Дескриптор сокета для очередного клиента
    char buffer[1024] = {'\0'};

    const char* helloString = "Hello from SERVER";


    int valRead {0};
    int valSend {0};


    struct sockaddr_in serverAddress;   // Структура, описывающая сокет (IP:порт). Стоит отметить, что для IPv6 используется
                                        // аналогичная структура "sockaddr_in6", а "sockaddr" - базовая часть всех этих структур.
    int addrLen {sizeof(serverAddress)};


    // Создаём новый сокет в домене AF_INET (что означает соединение по IPv4);
    // Тип сокета SOCK_STREAM (означает, что это TCP-сокет);
    // Значение IP (0 - айпишник выбирается автоматически).
    // Сама функция возращает дескриптор сокета

    // #### Сперва создаём новый TCP-сокет (SOCK_STREAM) в домене AF_INET, что означает
    // #### соединение по
    // ####
    int serverFD {socket(AF_INET, SOCK_STREAM, 0)};
    if (serverFD < 0) {
        std::perror("\n[ERROR]::[socket]");
        exit(EXIT_FAILURE);
    }
    else {
        std::cout << "\nServer socket has been created." << std::endl;
    }

    // Функция для манипуляции флагами сокета [serverFD]; SOL_SOCKET - означает манипуляции
    // флагами на уровне сокета. Далее указанные флаги закидываем в [socketOption]
    int socketOption {1};
    if (0 != setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socketOption, sizeof(socketOption))) {
        std::perror("[ERROR]::[setsockopt]");
        exit(EXIT_FAILURE);
    }
    else {} // Nothing to do



    serverAddress.sin_family = AF_INET;                     // Семья адресов IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;             // Привязываем сокет к адресу для приема ВСЕХ входящих сообщений
    serverAddress.sin_port = htons(MY_PORT);                // Номер порта

    // Присоединяем сокет к нашему порту
    if ((bind(serverFD, (struct sockaddr*)(&serverAddress), sizeof(serverAddress))) < 0) {
        std::perror("[ERROR]::[bind]");
        exit(EXIT_FAILURE);
    }
    else {} // Nothing to do

    // Устанавливаем сокет в пассивное состояние, в котором он будет "слушать"
    // входящие соединения, использующие ф-цию accept().
    if (listen(serverFD, socketMaxQueue) < 0) {
        std::perror("[ERROR]::[listen]");
        exit(EXIT_FAILURE);
    }
    else {} // Nothing to do

    // Принимаем первый запрос из очереди на соединение с [serverFD]
    newSocketFD = accept(serverFD, (struct sockaddr*)(&serverAddress), (socklen_t*)(&addrLen));
    if (newSocketFD < 0) {
        std::perror("[ERROR]::[accept]");
        exit (EXIT_FAILURE);
    }
    else {} // Nothing to do;

    // Read the input data from newSocket
    valRead = read(newSocketFD, buffer, 1024);
    if (valRead != -1) {
        std::cout << "Data, recived from client: ";
        std::cout.write(buffer, valRead);
    }
    else {
        std::perror("[WARNING]::[read]");
        close(newSocketFD);
    }

    // Send message to the newSocket.
    valSend = send(newSocketFD, helloString, strlen(helloString), 0);
    if (valSend != -1) {
        std::cout << "\nServer send message: ";
        for (int ii {0}; ii < valSend; ++ii) {
            std::cout.put(helloString[ii]);
        }
        std::cout << std::endl;
    }
    else {
        std::perror("[ERROR]::[send]");
        close(newSocketFD);
    }

    shutdown(serverFD, SHUT_RDWR);
    return 0;

}







