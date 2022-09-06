


#include <iostream>
#include <sys/socket.h>         // For: struct sockaddr, socket()
#include <netinet/in.h>         // For: htons()
#include <cstdio>               // For: perror()
#include <cstring>              // For: strlen()
#include <unistd.h>             // For: close()

#include <thread>
#include <atomic>


#define     macroPaste(front, back)     front ## _ ## back

int receiveImage(std::FILE*, int socket);
int receiveMessage(int socket);

//const int bufSize {16384};
//char buffer[bufSize];             // Temporary make it global 'cause I don't know the size of input image

#define     FILE_NAME               "/home/alexey/Desktop/new_image.jpeg"


#define PORT_NUMBER     8080        // portNum > 1024 && portNum <= 65535

enum Code {
    NO_ERROR,
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

#define END_OF_MESSAGE "END_OF_MESSAGE"

std::atomic<bool>   terminalIsFree {true};
