
#include "header.h"

#define     mcrIsDigit(ch)          (ch >= '0' && ch <= '9')


int main(int argc, const char* argv[])
//int main()
{
//    int argc = 2;
//    const char* argv[3] = {"Server", "8080", ""};


    int retValue {0};                       // Variable for the storing different return values
    std::FILE* image {nullptr};             //

    int portNumber {0};

    char c {'\0'};

    if (argc != 2) {
        std::cout << "\nUsage:  Server  Port" << std::endl;
        return 0;
    }
    else {
        // Определяем номер порта
        while (true) {
            c = *argv[1];
            if (mcrIsDigit(c)) {
                portNumber = portNumber * 10 + (c - '0');
                ++argv[1];
            }
            else {
                break;
            }
        }
    }

    std::cout << "\nportNumber: " << portNumber << std::endl;



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
    serverSocketAddress.sin_port        = htons(portNumber);     // Номер порта (переведённый в сетевой порядок следования байтов)
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
    int socketMaxQueue {2};
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
    else {
        std::cout << "\nConnection established!" << std::endl;
    }

    // #### Определяем тип получаемых данных
    int dataType {0};
    int attempt {0};
    int maxAttempt {10};

    bool inLoop {true};
    while (inLoop) {
        retValue = read(socket_1, &dataType, sizeof(int));
        if (retValue < 0) {
             std::perror("[WARNING]::[read]");
        }
        else {
            switch (dataType) {
            case STOP_TRANSFER:
                std::cout << "\nConnection closed due to the STOP_TRANSFER signal from the client." << std::endl;
                inLoop = false;
                break;
            case NO_DATA:
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                attempt++;
                if (attempt == maxAttempt) {
                    inLoop = false;
                }
                else {}
                break;
            case MESSAGE:
                receiveMessage(socket_1);
                dataType = NO_DATA;
                break;
            case IMAGE:
                receiveImage(image, socket_1);
                dataType = NO_DATA;
                break;
            default:
                std::cout << "\n[WARNING]: incorrect type of input data: " << dataType << "."
                          << std::endl;
                if (attempt < maxAttempt) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    attempt++;
                }
                else {
                    std::cout << "\nConnection closed due to the inability to receive correct data-type-signal from client";
                    inLoop = false;
                }
                break;
            } // End of switch()
        } // End of if-else()
    } // End of while()


    // #### Закрываем соединение с клиентом
    close(socket_1);

    // #### Отключаем как чтение, так и запись связанную с socketServer
    shutdown(serverSocketPart, SHUT_RDWR);
    return 0;

}


// #### Функция обрабатывает случай получения сообщения (набора сообщений) от клиента
// #### Возвращает кол-во символов, отправленных клиентом
int receiveMessage(int socket)
{
        int attempt {0};
        const int maxAttempt {10};
        const int bufSize {1024};
        char buffer[bufSize] = {'\0'};
        int retValue {0};

        bool inLoop {true};

        while (inLoop) {
            retValue = read(socket, buffer, bufSize);
            // Если есть сообщение - выводим
            if (retValue > 0) {

                if (0 == std::strcmp(buffer, END_OF_MESSAGE)) {
                    std::cout << "\nCLIENT_1: [END_OF_MESSAGE]" << std::endl;
                    return 0;
                }
                else {} // Nothing to do


                std::cout << "CLIENT_1: ";
                std::cout.write(buffer, retValue);
                std::cout << "\n";

                // Очищаем буфер для нового сообщения
                for (int ii {0}; ii < retValue; ++ii) {
                    buffer[ii] = '\0';
                }
                attempt = 0;
            }
            // Если нет - увеличиваем счётчик попыток
            // (Обдумать ситуацию, когда read() возвращает 0 в случае работы с сокетом)
            else if (retValue == 0) {
                inLoop = false;
                attempt++;
                std::cout <<"\n attempt " << attempt;
                if (attempt >= maxAttempt) {
                    inLoop = false;
                }
                else {} // Nothing to do
            }
            // Ошибка чтения сообщения - выходим.
            else {
                std::perror("[WARNING]::[read]");
                inLoop = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }


        return -1;
}



// #### Функция обрабатывает случай получения сервером картинки от клиента
// ####
int receiveImage(std::FILE* image, int socket)
{
    long imageSize {};
    long retValue {0};
    int receiveSize {0};
    const int bufSize {1024};
    char buffer[bufSize] = {'\0'};

    // #### Get the size of image
    do {
        retValue = read(socket, &imageSize, sizeof(long));
    } while (retValue < 0);

    if (retValue == 0) {
        std::cout << "\nDidn't get any images.";
        return 0;
    }

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
                // Пишем в файл изображения данные из буфера (в кол-ве retValue)
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







