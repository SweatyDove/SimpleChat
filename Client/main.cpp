#include "header.h"


//#define portNumber 8080
//#define targetIP "127.0.0.1"               // Сетевой адрес самой машины
//#define     FILE_NAME               "/home/alexey/Desktop/image.jpeg"
//#define     FILE_NAME

//#define     FILE_NAME               /home/alexey/Desktop/Other/unnamed.jpg



//int main()
int main(int argc, const char* argv[])
{
    int         targetPort {0};
    const char* targetIP {nullptr};

    //int argc = 3;
    //const char* argv[4] = {"Client", "127.0.0.1", "8080", ""};


    char ch {'\0'};

    if (argc != 3) {
        std::cout << "\nUsage: Client  IPv4  Port" << std::endl;
        return 0;
    }
    else {
        targetIP = argv[1];
        while (true) {
            ch = *argv[2];
            if (mcrIsDigit(ch)) {
                targetPort = targetPort * 10 + (ch - '0');
                ++argv[2];
            }
            else {
                break;
            }
        }
    }

    std::cout << "\nTarget IPv4: " << targetIP
              << "\nTarget Port: " << targetPort
              << std::endl;
    int retValue {0};


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
    serverSocketAddress.sin_port   = htons(targetPort);    // Номер порта (переведённый в сетевой порядок следования байтов)

    // Конвертируем IP-адрес из текстовой формы в бинарную (сетевой порядок байт) в нужном
    // нам исходном формате (так как задан параметр AF_INET, то в формат адресов семейства IPv4)
    if (inet_pton(AF_INET, targetIP, &serverSocketAddress.sin_addr) <= 0) {
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
    std::string message {};
    int         dataType {0};

    bool taskLoop {true};
    while (taskLoop) {
        std::cout << "\nWhat are you going to do? (Enter an appropriate button)."
                  << "\n [1]   - Send message"
                  << "\n [2]   - Send file"
                  << "\n [Q]   - Exit"
                  << std::endl;
        ch = std::cin.get();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(ch) {

        // #### Отправка сообщения
        // ####
        case '1':
            dataType = MESSAGE;
            retValue = send(clientSocket, (void*) &dataType, sizeof(int), 0);
            if (retValue < 0) {
                 std::perror("[WARNING]::[read]");
            }
            else {} // Nothing to do

            std::cout << "\nEnter your message and press 'ENTER' to send it. (Press 'ESC' - to exit)." << std::endl;

            linuxTerminalMode(!CANONICAL);
            while(true) {
                std::cout << ">" << std::flush;

                // Если сообщение - отправляем сообщение
                if (0 == getString(message)) {
                    retValue = send(clientSocket, message.c_str(), message.size(), 0);
                    if (retValue < 0) {
                        std::perror("[WARNING]::[send]");
                    }
                    else {} // Nothing to do
                    message.clear();
                }
                // Если конец ввода сообщений - отправляем сигнал
                else {
                    retValue = send(clientSocket, END_OF_MESSAGE, sizeof(END_OF_MESSAGE), 0);
                    break;
                }
            }
            linuxTerminalMode(CANONICAL);
            break;

        // #### Отправка изображения
        // ####
        case '2':
            dataType = IMAGE;
            retValue = send(clientSocket, (void*) &dataType, sizeof(int), 0);
            if (retValue < 0) {
                 std::perror("[WARNING]::[read]");
            }
            else {} // Nothing to do

            sendFile(clientSocket);

            break;

        // #### Выход
        // ####
        case 'Q': case 'q':
            dataType = STOP_TRANSFER;
            retValue = send(clientSocket, (void*) &dataType, sizeof(int), 0);
            std::cout << "\b\b  \b\b" << std::flush;
            taskLoop = false;
            break;
        default:
            break;
        }
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max());
    }


    close(conn);
    shutdown(clientSocket, SHUT_RDWR);
    return 0;
}
