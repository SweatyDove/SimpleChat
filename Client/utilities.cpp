
#include "header.h"


// #### Функция записывает строку в буфер
int getString(std::string& message)
{
    char ch {'\0'};
    //int (*keystroke)(void) = linux_kbhit;

    // #### This loop is responsible for getting number
    while (true) {

        // Если нажали какую-то клавишу
        if (linux_kbhit()) {
            // Считываем её
            ch = std::cin.get();

            // Если ESC - то выход из режима ввода
            if (ch == 27) {
                for (unsigned int ii {0}; ii < message.size(); ++ii) {
                    std::cout << "\b\b\b   \b\b\b";
                }
                std::cout << "\b\b\b   \b\b\b" << std::flush;
                message.clear();
                return -1;
            }
            // Если BACKSPACE - то затираем один символ на экране (и в message)
            else if (ch == 127) {
                if (message.size() > 0) {
                    message.pop_back();
                    std::cout << "\b\b\b   \b\b\b" << std::flush;
                }
                else {
                    std::cout << "\b\b  \b\b" << std::flush;
                }
            }
            // Если ENTER - закончили ввод строки сообщения
            else if (ch == '\n') {
                return 0;

            }
            // Любой другой символ пишем на экран и в буффер сообщения
            else {
                message.push_back(ch);
            }
        }
        // Ждём следующий символ
        else {
            // Предполагаю, что максимальная скорость набора текста 6000 символов/мин.
            // Чтобы не перегружать процессор.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    return 0;
}



// #### Отправка файла/изображения.
// ####
int sendFile(int socket)
{
    std::FILE*  image {nullptr};
    long        imageSize {0};
    long        sendSize {0};
    long        retValue {0};
    int         bufSize {2048};
    char        buffer[bufSize] {'\0'};               // Буффер для пакетной отправки изображения

    std::string filePath {};

    while (true) {

        std::cout << "\nEnter path to the image to send (or enter \"exit\" to exit):"
                  << "\nPath: " << std::flush;
        std::cin >> filePath;
        //std::strcpy(buffer, filePath.c_str());

        if (0 == std::strcmp(filePath.c_str(), "exit")) {
            return 0;
        }

        //std::cout << "\nTrying to open:" << filePath.c_str();
        // #### Open file/image
        image = std::fopen(filePath.c_str(), "rb");
        if (image == nullptr) {
            filePath.clear();
            perror("[ERROR]::[fopen]");
        }
        else {
            break;
        }
    }
    // ## Calculate file size
    std::fseek(image, 0, SEEK_END);
    imageSize = std::ftell(image);
    std::fseek(image, 0, SEEK_SET);


    // #### Firstly, send @imageSize to the server
    retValue = send(socket, (void*) &imageSize, sizeof(long), 0);
    if (retValue < 0) {
        std::perror("[WARNING]::[send]");
    }
    else {}

    // #### Copy the image in pieces (2048 byte-pieces) in the buffer and send
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

//        for (int ii {0}; ii < bufSize; ++ii) {
//            buffer[ii] = '\0';
//        }
    }
    std::cout << "\nImage original size: " << imageSize << " bytes."
              << "\nYou send:            " << sendSize << " bytes."
              << std::endl;

    if (sendSize == imageSize) {
        std::cout << "\nFile " << filePath << " has been successfully sent!" << std::endl;
    }
    else {
        std::cout << "Can't completely sent file " << filePath << '.' << std::endl;
    }



    return 0;

}

