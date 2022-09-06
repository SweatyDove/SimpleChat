
#include "header.h"


//int main()
int main(int argc, const char* argv[])
{


    //int argc = 2;
    //const char* argv[3] = {"Server", "8080", ""};


    char        ch {'\0'};
    int         retValue {0};                       // Variable for the storing different return values
    int         portNumber {0};


    // #### Обработка аргументов командной строки
    if (argc != 2) {
        std::cout << "\nUsage:  Server  Port" << std::endl;
        return BAD_USAGE;
    }
    else {
        // Определяем номер порта
        while (true) {
            ch = *argv[1];
            if (mcrIsDigit(ch)) {
                portNumber = portNumber * 10 + (ch - '0');
                ++argv[1];
            }
            else {
                break;
            }
        }
    }
    std::cout << "\nPort number: " << portNumber << std::endl;



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
        std::cout << "\nServer part of a socket has been created." << std::endl;
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


    // #### Работа с клиентом
    // ####
    int dataType {0};                   // Тип получаемых данных (сообщение, картинка, файл и т.д.)

    int attempt {0};                    // Номер текущей попытки получить данные от клиента
    int maxAttempt {10};                // Максимальное число таких попыток


    ssize_t bytesRead {0};


    bool inLoop {true};
    while (inLoop) {

        // Определяем тип данных, которые поступят далее
        bytesRead = read(socket_1, &dataType, sizeof(int));
        if (bytesRead < 0) {
             std::perror("[WARNING]::[read]");
        }
        else {
            switch (dataType) {

            // Клиент закончил передачу (в явном виде)
            case STOP_TRANSFER:
                std::cout << "\nConnection closed due to the STOP_TRANSFER signal from the client." << std::endl;
                inLoop = false;
                break;

            // От клиента нет данных
            case NO_DATA:
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                attempt++;
                if (attempt >= maxAttempt) {
                    inLoop = false;
                }
                else {} // Nothing to do
                break;

            // От клиента придёт сообщение
            case MESSAGE:
                receiveMessage(socket_1);
                dataType = NO_DATA;
                attempt = 0;
                break;

            // От клиента придёт изображение
            case IMAGE:
                receiveImage(socket_1);
                dataType = NO_DATA;
                attempt = 0;
                break;

            // Некорректный тип данных
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

