

// Comments:
//
// #1 Добавить несколько потоков-клиентов, чтобы можно было получать сообщения сразу от нескольких чловек






#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr, socket()
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()

#include <thread>
#include <atomic>


#define     macroPaste(front, back)     front ## _ ## back

int receiveImage(std::FILE*, int socket);

const int bufSize {16384};
char buffer[bufSize];             // Temporary make it global 'cause I don't know the size of input image

#define     FILE_NAME               "/home/alexey/Desktop/new_image.jpeg"


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

enum Dataype {
    MESSAGE = 1,
    IMAGE,

    MAX_MESSAGE_TYPE
};

std::atomic<bool>   terminalIsFree {true};

int main()
{

    //const char* helloString = "Hello from SERVER";
    int retValue {0};
    std::FILE* image {nullptr};



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
    int serverSocketAddressLen {sizeof(serverSocketAddress)};


    // #### Связываем наш сокет с адресом
    // ####
    retValue = bind(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), serverSocketAddressLen);
    if (retValue < 0) {
        std::perror("[ERROR]::[bind]");
        return Code::BAD_BIND;
    }
    else {} // Nothing to do


    // #### Устанавливаем сокет в пассивное состояние, в котором он будет "слушать"
    // #### входящие соединения, использующие ф-цию accept().
    // #### [socketMaxQueue] - максимальная длина очереди из желающих присоединиться к сокету.
    // ####
    int socketMaxQueue {1};
    retValue = listen(serverSocketPart, socketMaxQueue);
    if (retValue < 0) {
        std::perror("[ERROR]::[listen]");
        return Code::BAD_LISTEN;
    }
    else {} // Nothing to do


    // #### Принимаем первый запрос из очереди желающих присоединиться к серверному сокету.
    // #### Формируем полноценный сокет (пару клиент-сервер).
    // ####
    int socket_1 {accept(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), (socklen_t*)(&serverSocketAddressLen))};
    if (socket_1 < 0) {
        std::perror("[ERROR]::[accept]");
        return Code::BAD_ACCEPT;
    }
    int dataType {0};
    retValue = read(socket_1, &dataType, sizeof(int));
    if (retValue < 0) {
         std::perror("[WARNING]::[read]");
    }
    else {
        switch (dataType) {
        case MESSAGE:
            // ReciveMessage
            break;
        // Случай приема файла/изображения
        case IMAGE:
            receiveImage(image, socket_1);
            break;
        default:
            std::cout << "\n[WARNING]: incorrect type of input data" << std::endl;
        }
    }
    // Случай приёма сообщений
    /*
    else {
        int attempt {0};
        const int maxAttempt {10};
        const int bufSize {2048};
        char buffer[bufSize] = {'\0'};

        while (attempt < maxAttempt) {
            retValue = read(socket_1, buffer, bufSize);
            // Если есть сообщение - выводим
            if (retValue > 0) {
                std::cout << "CLIENT_1: ";
                std::cout.write(buffer, retValue);
                std::cout << "\n";
                attempt = 0;
            }
            else if (retValue == 0) {
                // Если нет - увеличиваем счётчик попыток
                attempt++;
            }
            else {
                std::perror("[WARNING]::[read]");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    */

    // #### Закрываем соединение с клиентом
    close(socket_1);

    // #### Отключаем как чтение, так и запись связанную с socketServer
    shutdown(serverSocketPart, SHUT_RDWR);
    return 0;

}


int receiveImage(std::FILE* image, int socket)
{
    long imageSize {};
    long retValue {0};
    int receiveSize {0};

    // #### Get the size of image
    do {
        retValue = read(socket, &imageSize, sizeof(long));
    } while (retValue < 0);

    image = std::fopen(FILE_NAME, "wr");
    if (image == nullptr) {
        std::perror("[ERROR]::[fopen]");
        return -1;
    }
    else {
        // #### Send the verification signal, that we are ready to accept the image
    }


    // ####

    struct timeval timeValue = {10, 0};
    fd_set fileDescriptorSet;

    while(receiveSize < imageSize) {

        // #1 Clear a new set of descriptors (which are living in the [fileDescriptorSet])
        FD_ZERO(&fileDescriptorSet);

        // #2 Add our socket descriptor into the specified set: [fileDescriptorSet]
        FD_SET(socket, &fileDescriptorSet);

        // #3 Wait for status changes of elements in [fileDescriptorSet].
        // ## [timeValue] is a time before get answer.
        retValue = select(FD_SETSIZE, &fileDescriptorSet, NULL, NULL, &timeValue);
        if (retValue < 0) {
            std::perror("[ERROR]::[select]");
            return -1;
        }
        else if (retValue == 0) {
            std::cout << "\n[ERROR]::[select]: buffer read timeout has expired.";
            return -1;
        }
        else {
            retValue = read(socket, buffer, bufSize);
            if (retValue < 0) {
                std::perror("[ERROR]::[select]");
                return -1;
            }
            else {
                // Пишем в файл изображения данные из буфера
                std::fwrite(buffer, 1, retValue, image);
                receiveSize += retValue;
            }
        }
    }

    std::cout << "Get image size:      " << imageSize << " bytes."
              << "Received image size: " << receiveSize << " bytes."
              << std::endl;

    std::fclose(image);
    std::string displayCommand {"\n eog "};
    std::string fileName {FILE_NAME};
    std::string commandLine = displayCommand + fileName;
    std::system(commandLine.c_str());

    return 0;

}




    // #### Принимаем первый запрос из очереди желающих присоединиться к серверному сокету.
    // #### Формируем полноценный сокет (пару клиент-сервер).
    // ####
//    int socket_1 {accept(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), (socklen_t*)(&serverSocketAddressLen))};
//    if (socket_1 < 0) {
//        std::perror("[ERROR]::[accept]");
//        return Code::BAD_ACCEPT;
//    }
//    else {} // Nothing to do

    // #### Считываем данные, приходящие через сокет [socketClientServer]
    // ####
//    retValue = read(socket_1, buffer, 1024);
//    if (retValue != -1) {
//        std::cout << "Data, recived from client_1: ";
//        std::cout.write(buffer, retValue);

//    }
//    else {
//        std::perror("[WARNING]::[read]");
//    }

    // #### Отправляем данные через сокет @socketClientServer клиенту
    // ####
    /*
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
    */

//    // #### Закрываем соединение с клиентом
//    close(socket_1);

//    // #### Отключаем как чтение, так и запись связанную с socketServer
//    shutdown(serverSocketPart, SHUT_RDWR);
//    return 0;






//int tmp;

//int number {0};
//while (number < socketMaxQueue) {
//    tmp = accept(serverSocketPart, (struct sockaddr*)(&serverSocketAddress), (socklen_t*)(&serverSocketAddressLen));
//    if (tmp < 0) {
//        std::perror("[ERROR]::[accept]");
//        return Code::BAD_ACCEPT;
//    }
//    else if (number == 0) {
//        std::thread thread_0 (foo, tmp, number);
//        ++number;
//    }
//    else if (number == 1) {
//        std::thread thread_1 (foo, tmp, number);
//        ++number;
//    }
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//}

//void foo(int socket, int num)
//{
//    int socket {tmp};
//    int retValue {};
//    const int bufSize {1024};
//    char buffer[bufSize] = {'\0'};

//    while (terminalIsFree) {
//        retValue = read(socket, buffer, bufSize);
//        if (retValue != -1) {
//            std::cout << "CLIENT_1:\n";
//            std::cout.write(buffer, retValue);
//        }
//        else {
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }
//    }

//});







