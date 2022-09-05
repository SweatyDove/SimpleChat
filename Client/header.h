#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr
//#include <netinet/in.h>       // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()
#include <arpa/inet.h>
#include <string>
#include <limits>
#include <thread>



#include <cassert>
#include <sys/time.h>
#include <sys/select.h>
#include <termios.h>                // Describe a general terminal interface that
                                    // is provided to control asynchronous communications ports.

#define     CANONICAL   true
#define     ESC         027


#define     LINUX_CRLF(c)   (c == '\n')
#define     WINDOWS_CRLF(c)


void    linuxTerminalMode(bool mode);
int     linux_kbhit(void);

void clearWorkScreen(int lines, int columns);
void moveCursor(int relative_line, unsigned int column);
int getString(std::string& message);



#define PORT_NUMBER 8080
#define TARGET_IP "127.0.0.1"               // Сетевой адрес самой машины

enum RetCode {
    NO_ERROR,
    BAD_SOCKET,
    BAD_SETSOCKOPT,
    BAD_INET_PTON,
    BAD_CONNECT,
    BAD_BIND,
    BAD_LISTEN,
    BAD_ACCEPT,

    MAX_RETURN_RetCode
};

