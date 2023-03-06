
#include "my_tcpsocket.h"



// Штука для безопасного преобразования указателей между собой. То есть мы используем сперва
// преобразование в void, которое нельзя применить НЕ к указателям. А потом в нужный тип
template <typename result_t, typename source_t>
result_t pointer_cast(source_t* var)
{
    return static_cast<result_t>(static_cast<void*>(var));
}

template <typename result_t, typename source_t>
result_t pointer_cast(const source_t* var)
{
    return static_cast<result_t>(static_cast<const void*>(var));
}





// Что должен делать конструктор сокета? По хорошему, нам нужен только IP и порт.
// А дальше оно само должно как-то работать.
namespace my {

TCPSocket::TCPSocket(char* ip, int port) :
    mb_ip {ip},
    mb_port {port}
{
    TCPSocket::createSocket();
    TCPSocket::setSocketOptions();
    TCPSocket::bindWithAddress();
    TCPSocket::setInListeningState();

} // TCPSocket()


//==================================================================================================
// Устанавливаем размер очереди для сокета
//==================================================================================================
void TCPSocket::setSocketMaxQueueSize(int size)
{
    mb_serverSocketMaxQueue = size;
}





//==================================================================================================
// Функция создаёт сокет в области имён
//==================================================================================================
void TCPSocket::createSocket(int domain, int type, int protocol)
{
    // Тут создаём сокет, работающий с семейством протоколов сетевого уровня
    // IPv4 (@AF_INET),осуществляющий последовательную передачу данных
    // (@SOCK_STREAM; то есть не дэйтаграмм, а потока битов). На транспортном
    // уровне передача осуществляется по протоколу TCP (@IPPROTO_TCP).
    // Но нужно иметь ввиду, что тут мы задали только имя сокета, но ещё не связали его с физическим
    // адресом (будет далее).
    mb_serverSocketFD = socket(domain, type, protocol);
    if (mb_serverSocketFD < 0) {
        std::perror("\n[ERROR]");
        //return TCPSocket::errorCode::BAD_SOCKET;
    }
    else {
        std::cout << "Server's socket has been successfully created!" << std::endl;
        //return TCPSocket::errorCode::NO_ERROR;
    }
}




//==================================================================================================
// Функция-оболочка устанавливает параметры сокета
//==================================================================================================
void TCPSocket::setSocketOptions(int level, int options)
{
    // Теперь устанавливаем опции серверного сокета
    // @SOL_SOCKET - опции уровня сокета
    // @SO_REUSEADDR, @SO_REUSEPORT - непосредственно устанавливаемые флаги
    // @serverSocketOpt - переменная, которая и содержит эти опции
    mb_retValue = setsockopt(mb_serverSocketFD, level, options, &mb_serverSocketOpt, sizeof(mb_serverSocketOpt));
    if (mb_retValue != 0) {
        std::perror("\n[ERROR]");
        mb_retValue = TCPSocket::errorCode::BAD_SETSOCKOPT;
    }
    else {
        std::cout << "Server's socket flags has been successfully set!" << std::endl;
        mb_retValue = 0;
    }

}

//==================================================================================================
// Функция-оболочка устанавливает параметры сокета
//==================================================================================================
void TCPSocket::bindWithAddress()
{
    // Инициализируем структуру, содержащую информацию о адресе сокета и затем связываем её с самим
    // сокетом.
    // @AF_INET           - Семейство адресов IPv4.
    // @htons(portNumber) - Переводим номер порта в сетевой порядок следования байтов
    //                      (т.е. в Big-endian). Если же исходный порядок уже Big-endian, то не
    //                      делаем ничего.
    // @INADDR_ANY        - В данном случае мы указываем, с какими IP-адресами хотим работать (т.е.
    //                      у машины может быть несколько сетевых интерфейсов с разными IP-шниками).
    //                      И тут мы определяем, может ли программа работать с другими интерфейсами.

    mb_serverSocketAddr.sin_family      = AF_INET;
    mb_serverSocketAddr.sin_port        = htons(mb_port);
    mb_serverSocketAddr.sin_addr.s_addr = INADDR_ANY;

    mb_serverSocketAddrSize = sizeof(mb_serverSocketAddr);

    // Теперь связываем серверный сокет с адресом.
    mb_retValue = bind(mb_serverSocketFD, pointer_cast<const sockaddr*>(&mb_serverSocketAddr), mb_serverSocketAddrSize);
    if (mb_retValue < 0) {
        std::perror("\n[ERROR]");
        //return RetCode::BAD_SERVER_SOCKET;
    }
    else {
        std::cout << "Server's socket has been successfully bound with adress!" << std::endl;
    } // Nothing to do

}


//==================================================================================================
//
//==================================================================================================
void TCPSocket::setInListeningState()
{
    mb_retValue = listen(mb_serverSocketFD, mb_serverSocketMaxQueue);
    if (mb_retValue < 0) {
        std::perror("\n[ERROR]");
        //return RetCode::BAD_SERVER_SOCKET;
    }
    else {
        std::cout << "Server's socket is in the listening state now..." << std::endl;
    } // Nothing to do
}



int TCPSocket::acceptPendingConnection()
{
    Connection* currentConnection {nullptr};

    // При установке очередного соединения - данные о нём будут записываться в массив
    // Сперва ищем первый эл-т массива, который равен нулю
    if (mb_arrayOfEstablishConn.empty() == true) {
        currentConnection = new Connection {};
        mb_arrayOfEstablishConn.push_back(currentConnection);
    }
    else {
        // Ищем первый эл-т массива, значение которого равно NULL (т.е. уже свободный).
        // Если такого нет, то добавляем новый эл-т соединения
        for (auto conn : mb_arrayOfEstablishConn) {
            if (conn == nullptr) {
                conn = new Connection {};
                currentConnection = conn;
                break;
            }
            else {
                currentConnection = new Connection {};
                mb_arrayOfEstablishConn.push_back(currentConnection);
            }
        }
    }

    currentConnection->FD = accept(mb_serverSocketFD,
                                   currentConnection->clientAddr,
                                   pointer_cast<socklen_t*>(&(currentConnection->clientAddrSize)));

    struct sockaddr_in* pV4Addr = pointer_cast<sockaddr_in*>(currentConnection->clientAddr);
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN] {'\0'};
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);



   // currentConnection->FD = accept(mb_serverSocketFD, NULL, NULL);
    if (currentConnection->FD < 0) {
        std::perror("\n[ERROR]");
        return 0;
    }
    else {
        std::cout << "\nConnection established!" << std::endl;
        return currentConnection->FD;
    }

}





} // END_OF_MY_NAMESPACE
