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
#include <cctype>



#include <cassert>
#include <sys/time.h>
#include <sys/select.h>
#include <termios.h>                // Describe a general terminal interface that
                                    // is provided to control asynchronous communications ports.

#define     CANONICAL   true
#define     ESC         027
#define     MOVE_CURSOR_ONE_LINE_UP     "\033[F"
#define     NEW_LINE                    "\n"


#define     LINUX_CRLF(c)   (c == '\n')
#define     WINDOWS_CRLF(c)


void    linuxTerminalMode(bool mode);
int     linux_kbhit(void);

void clearWorkScreen(int lines, int columns);
void moveCursor(int relative_line, unsigned int column);
int getString(std::string& message);




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


