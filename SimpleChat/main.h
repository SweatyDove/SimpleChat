#ifndef MAIN_H
#define MAIN_H


#include <iostream>
#include <sys/socket.h>         // For: socket()
#include <netinet/in.h>         // For: IPPROTO_TCP
#include <cstdio>               // For: perror()


// For tests
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()
#include <thread>
//



enum RetCode {
    NO_ERROR,
    BAD_SERVER_SOCKET,
    BAD_CLIENT_SOCKET,
    BAD_SOCKET,

    MAX_RET_CODE
};

enum ControlMessage {
    STOP_TRANSFER = -1,
    NO_DATA = 0,
    MESSAGE,
    //FILE,

    MAX_MESSAGE_TYPE
};





/*
 * TEMPORARY snippet of the code
 */

#define     mcrIsDigit(ch)          (ch >= '0' && ch <= '9')


#define     END_OF_MESSAGE          "END_OF_MESSAGE"
#define     FILE_NAME               "/tmp/image.jpeg"

enum Code {
    //NO_ERROR,
    BAD_USAGE,
    //BAD_SOCKET,
    BAD_SETSOCKOPT,
    BAD_BIND,
    BAD_LISTEN,
    BAD_ACCEPT,

    MAX_RETURN_CODE
};

//enum DataType {
//    STOP_TRANSFER = -1,
//    NO_DATA = 0,
//    MESSAGE,
//    IMAGE,

//    MAX_MESSAGE_TYPE
//};


int receiveImage(int socket);
int receiveMessage(int socket);

/*******************************/


#endif // MAIN_H
