#include "header.h"

#define     END_OF_TRANSMISSION     "[CLIENT HAS STOPPED TRANSMISSION]"
#define     FILE_NAME               "/home/alexey/Desktop/image.jpeg"
int sendFile(const char* fileName, int socket);


int main()
{
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
    bool taskLoop {true};
    char ch {'\0'};
    std::string message {};

    while (taskLoop) {
        std::cout << "\nWhat are you going to do? (Press an appropriate button)."
                  << "\n [1]   - Send message"
                  << "\n [2]   - Send file"
                  << "\n [ESC] - Exit"
                  << std::endl;
        ch = std::cin.get();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch(ch) {
        case '1':
            std::cout << "\nEnter your message. (Press 'ENTER' to send message; 'ESC' - to exit)." << std::endl;

            linuxTerminalMode(!CANONICAL);
            while(true) {
                std::cout << ">" << std::flush;
                if (0 == getString(message)) {
                    retValue = send(clientSocket, message.c_str(), message.size(), 0);
                    if (retValue < 0) {
                        std::perror("[WARNING]::[send]");
                    }
                    else {} // Nothing to do
                    message.clear();
                }
                else {
                    break;
                }
            }
            linuxTerminalMode(CANONICAL);
            break;
        case '2':
            if (0 == sendFile(FILE_NAME, clientSocket)) {
                std::cout << "\nFile " << FILE_NAME << " has been successfully sent!" << std::endl;
            }
            else {
                std::cout << "Can't sent file " << FILE_NAME << '.' << std::endl;
            }
            break;
        case 27:
            retValue = send(clientSocket, END_OF_TRANSMISSION, sizeof(END_OF_TRANSMISSION), 0);
            taskLoop = false;
            break;
        default:
            break;
        }
    }


    close(conn);
    shutdown(clientSocket, SHUT_RDWR);
    return 0;
}



int sendFile(const char* fileName, int socket)
{
    std::FILE*  image {nullptr};
    long        imageSize {0};
    long        sendSize {0};
    long        retValue {0};
    int         bufSize {2048};
    char        buffer[bufSize] {'\0'};               // Буффер для пакетной отправки изображения

    // #### Open file/image
    image = std::fopen(fileName, "rb");
    if (image == nullptr) {
        perror("[ERROR]::[fopen]");
        return -1;
    }
    else {
        // ## Calculate file size
        std::fseek(image, 0, SEEK_END);
        imageSize = std::ftell(image);
        std::fseek(image, 0, SEEK_SET);
    }

    // #### Firstly, send @imageSize to the server
    retValue = send(socket, (void*) &imageSize, sizeof(long), 0);
    if (retValue < 0) {
        std::perror("[WARNING]::[send]");
    }
    else {}

    // #### Copy the image in pieces (4096 byte) in the buffer and send
    while (!std::feof(image)) {
        retValue = std::fread((void*) buffer, 1, bufSize, image);
        if (retValue < 0) {
            std::perror("[ERROR]::[fread]");
            return -1;
        }
        else {
            retValue = send(socket, (void*) buffer, retValue, 0);
            if (retValue < 0) {
                std::perror("[ERROR]::[send]");
                return -1;
            }
            else {
                sendSize += retValue;
            } // Nothing to do
        }

        for (int ii {0}; ii < bufSize; ++ii) {
            buffer[ii] = '\0';
        }
    }

    std::cout << "\nImage original size: " << imageSize << " bytes."
              << "\nYou send:            " << sendSize << " bytes."
              << std::endl;

    return 0;

}


/*
}
else if (retValue < imageSize) {
    std::cerr << "\n[WARNING]::[fread]: read to buffer: " << retValue << " bytes,"
              << "\n                    but image size: " << imageSize << " bytes."
              << std::endl;
}
else {
    std::cout << "\nSuccessfully copy image in the buffer!" << std::endl;
}

*/






// #### Отправляем сообщение через клиентский сокет. Последний аргумент функции равен
// #### нулю, что означает запись в сокет в порядке поступления байтов.
// ####
//    retValue = send(clientSocket, strFromClient, strlen(strFromClient), 0);
//    if (retValue < 0) {
//        std::perror("[WARNING]::[send]");
//    }
//    else {
//        std::cout << "\nNext message send to server: ";
//        for (int ii {0}; ii < retValue; ++ii) {
//            std::cout.put(strFromClient[ii]);
//        }
//        std::cout << std::endl;
//    }

//    // Считываем сообщение от сервера
//    retValue = read(clientSocket, buffer, 1024);
//    if (retValue != -1) {
//        std::cout << "\nGot server response: ";
//        std::cout.write(buffer, retValue);
//        std::cout << std::endl;
//    }
//    else {
//        std::perror("[WARNING]::[read]");
//    }
