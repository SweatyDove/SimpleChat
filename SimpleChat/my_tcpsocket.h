#ifndef MY_TCPSOCKET_H
#define MY_TCPSOCKET_H

#include <iostream>
#include <vector>
#include <sys/socket.h>         // For: socket()
#include <netinet/in.h>         // For: IPPROTO_TCP
#include <cstdio>               // For: perror()
#include <arpa/inet.h>          // For: inet_ntop()


namespace my {

class TCPSocket {
public:
    enum errorCode {
        NO_ERROR,
        BAD_SOCKET,
        BAD_SETSOCKOPT,

        MAX_ERROR_CODE
    };

    // Структура, описывающая клиентскую сторону соединения
    struct Connection {
        int FD {};
        sockaddr* clientAddr {};
        int clientAddrSize {};
        // timeOfConnection - добавить время, когда соединения было установлено
        // Время окончания соединения будет добавлено в файл лога конкретного соединения
    };

    // Тут, наверное, должен быть динамически создаваемый массив указателей, каждый эл-т которого
    // соответствует соединению данного сокета с клиентом. Номера массива соответствуют файловым
    // дескрипторам.
    std::vector<Connection*> mb_arrayOfEstablishConn {};

public:
    int mb_serverSocketFD {};              // Файловый дескриптор серверного сокета
    int mb_serverSocketOpt {1};             // Флаги серверного сокета
    sockaddr_in mb_serverSocketAddr {};    // Структура данных, ссодержащая адрес серверного сокета
                                        // Есть также тип <sockaddr> - он общий для адресов разных
                                        // типов (sockaddr_in - для IPv4, sockaddr_in6 - для IPv6 и т.д.)
                                        // Поэтому и используется размер структуры <sockaddr_in>, так
                                        // как он разный для разных протоколов, а <sockaddr> - один.
    int mb_serverSocketAddrSize {};     // Размер структуры адреса - нужен для кастинга в <sockaddr>

    int mb_serverSocketMaxQueue {};        // Максимальный размер очереди из входящих подключений
    char* mb_ip {};
    int mb_port {};

    int mb_retValue {};                 // Вспомогательная переменная, содержащая код, возвращаемый
                                        // из методов класса





TCPSocket(char* ip, int port);
//~TCPSocket();


void setSocketMaxQueueSize(int size);

//==================================================================================================
//
//==================================================================================================
void createSocket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP);


//==================================================================================================
//
//==================================================================================================
void setSocketOptions(int level = SOL_SOCKET, int options = SO_REUSEADDR | SO_REUSEPORT);

//==================================================================================================
//
//==================================================================================================
//void bindWithAddress(int portNumber = 0, int address = INADDR_ANY);
void bindWithAddress();

//==================================================================================================
//
//==================================================================================================
void setInListeningState();


//==================================================================================================
//
//==================================================================================================
int acceptPendingConnection();


}; // END-OF-TCPSocket-Class




//    // Устанавливаем сокет в слушающее состояние
//    int serverSocketMaxQueue {10};
//    retValue = listen(serverSocketFD, serverSocketMaxQueue);
//    if (retValue < 0) {
//        std::perror("\n[ERROR]");
//        return RetCode::BAD_SERVER_SOCKET;
//    }
//    else {} // Nothing to do


//    int socketFD {accept(serverSocketFD, pointer_cast<sockaddr*>(&serverSocketAddr), &serverSocketAddrSize)};
//    if (socketFD < 0) {
//        std::perror("\n[ERROR]");
//        return RetCode::BAD_SOCKET;
//    }
//    else {
//        std::cout << "\nConnection established!" << std::endl;
//    }


} // END OF my NAMESPACE

#endif // MY_TCPSOCKET_H
