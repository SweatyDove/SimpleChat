#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr, socket()
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()

#include <thread>



#define     mcrIsDigit(ch)          (ch >= '0' && ch <= '9')


#define     END_OF_MESSAGE          "END_OF_MESSAGE"
#define     FILE_NAME               "/tmp/image.jpeg"



enum Code {
    NO_ERROR,
    BAD_USAGE,
    BAD_SOCKET,
    BAD_SETSOCKOPT,
    BAD_BIND,
    BAD_LISTEN,
    BAD_ACCEPT,

    MAX_RETURN_CODE
};

enum DataType {
    STOP_TRANSFER = -1,
    NO_DATA = 0,
    MESSAGE,
    IMAGE,

    MAX_MESSAGE_TYPE
};


int receiveImage(int socket);
int receiveMessage(int socket);



