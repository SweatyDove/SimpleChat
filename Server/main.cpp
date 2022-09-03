#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr, socket()
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()



#define PORT_NUMBER     8080        // portNum > 1024 && portNum <= 65535

enum ReturnCode {
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
    //int newSocketFD {};                 // Дескриптор сокета для очередного клиента
    char buffer[1024] = {'\0'};

    const char* helloString = "Hello from SERVER";
    int retValue {0};


    int valRead {0};
    int valSend {0};





    // #### int socket(int DOMAIN, int TYPE, int PROTOCOL);
    // ####
    // #### Функция создаёт сокет (оконечную точку для коммуникации). В случае успеха возвращает
    // ####         дескриптор, по которому можно обращаться к сокету.
    // ####
    // ####   DOMAIN   - коммуникационный домен, описывающий форматы адресов и правила их интерпретации
    // ####              (AF_INET - адрес состоит из имени хоста и номера порта; AF_UNIX - адресс это допустимое имя файла).
    // ####   TYPE     - тип соединения (SOCK_STREAM - создание виртуального канала для потоковой передачи байтов;
    // ####              SOCK_DGRAM - передача датаграмм, отдельных пакетов с данными, т.е. порциями).
    // ####   PROTOCOL - тип протокола (TCP, UDP и т.д.)
    // ####
    int serverSocket {socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)};
    if (serverSocket < 0) {
        std::perror("\n[ERROR]::[socket]");
        return BAD_SOCKET;
    }
    else {
        std::cout << "\n[MESSAGE]::[socket]: server socket has been created." << std::endl;
    }


    // #### int setsockopt(int SOCKET, int LEVEL, int FLAG, const void *BUFFER, socklen_t BUFFER_LENGTH);
    // ####
    // #### Функция устанавливает флаги (опции) сокета.
    // ####
    // ####   LEVEL   - Определяет уровень, на котором будет работать флаг
    // ####             (SOL_SOCKET - непосредственно уровень сокета).
    // ####   FLAG    - непосредственно флаг(и) для установки
    // ####
    // ####   BUFFER  - буффер для хранения флагов (ниже используется просто целое число)
    // ####
    int socketOption {1};
    retValue = setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socketOption, sizeof(socketOption));
    if (retValue != 0) {
        std::perror("[ERROR]::[setsockopt]");
        return BAD_SETSOCKOPT;
    }
    else {} // Nothing to do


    // #### Итак, сокет для сервера создали. Теперь нужно присвоить ему адрес, по которому с этим сокетом смогут
    // #### связываться другие процессы.
    // #### Инициализируем структуру, которая будет содержать адрес нашего серверного сокета:
    // ####
    struct sockaddr_in serverAddress =
    {
        .sin_family      = AF_INET,             // Коммуникационный домен AF_INET (адрес = имя хоста + номер порта)
        .sin_port        = htons(PORT_NUMBER)   // Номер порта (переведённый в сетевой порядок следования байтов)
        .sin_addr.s_addr = INADDR_ANY,          // Присваиваем IP-адрес (INADDR_ANY - хотим работать со всеми IP-адресами машины).
                                                // Как я понимаю, есть разные адреса (для WiFi, для подключения через Ethernet,
                                                //   для локального адреса, для широковещательного адрес и т.п.)
    };
    int addrLen {sizeof(serverAddress)};


    // #### Связываем наш сокет с адресом
    // ####
    retValue = bind(serverSocket, (struct sockaddr*)(&serverAddress), sizeof(serverAddress));
    if (retValue < 0) {
        std::perror("[ERROR]::[bind]");
        return ReturnCode::BAD_BIND;
    }
    else {} // Nothing to do


    // #### Устанавливаем сокет в пассивное состояние, в котором он будет "слушать"
    // #### входящие соединения, использующие ф-цию accept().
    // #### [socketMaxQueue] - максимальная длина очереди из желающих присоединиться к сокету.
    // ####
    int socketMaxQueue {3};
    retValue = listen(serverSocket, socketMaxQueue);
    if (retValue < 0) {
        std::perror("[ERROR]::[listen]");
        return ReturnCode::BAD_LISTEN;
    }
    else {} // Nothing to do



    // #### Принимаем первый запрос из очереди на соединение с серверным сокетом
    // ####
    int clientSocket {accept(serverSocket, (struct sockaddr*)(&serverAddress), (socklen_t*)(&addrLen))};
    if (clientSocket < 0) {
        std::perror("[ERROR]::[accept]");
        return ReturnCode::BAD_ACCEPT;
    }
    else {} // Nothing to do

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







