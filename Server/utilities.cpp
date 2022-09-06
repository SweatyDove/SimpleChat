#include "header.h"


// #### Функция обрабатывает случай получения сообщения (набора сообщений) от клиента
// #### Возвращает кол-во символов, отправленных клиентом
// ####
int receiveMessage(int socket)
{
        const int   bufSize {1024};
        char        buffer[bufSize] = {'\0'};

        ssize_t     bytesRead {0};



        bool inLoop {true};
        while (inLoop) {
            bytesRead = read(socket, buffer, bufSize);
            // Если есть сообщение - выводим
            if (bytesRead > 0) {

                if (0 == std::strcmp(buffer, END_OF_MESSAGE)) {
                    std::cout << "\nCLIENT_1: [END_OF_MESSAGE]" << std::endl;
                    return NO_ERROR;
                }
                else {} // Nothing to do


                std::cout << "CLIENT_1: ";
                std::cout.write(buffer, bytesRead);
                std::cout << "\n";

                // Очищаем буфер для нового сообщения
                for (int ii {0}; ii < bytesRead; ++ii) {
                    buffer[ii] = '\0';
                }
            }
            // Если нет - увеличиваем счётчик попыток
            // (Обдумать ситуацию, когда read() возвращает 0 в случае работы с сокетом)
            else if (bytesRead == 0) {
                inLoop = false;
            }
            // Ошибка чтения сообщения - выходим.
            else {
                std::perror("[WARNING]::[read]");
                inLoop = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }


        return -1;
}




// #### Функция обрабатывает случай получения сервером картинки от клиента
// ####
int receiveImage(int socket)
{
    std::FILE* imageFD {nullptr};
    long imageSize {};
    long retValue {0};
    int receiveSize {0};
    const int bufSize {1024};
    char buffer[bufSize] = {'\0'};

    std::string filePathName {"/tmp/image.jpeg"};

    ssize_t bytesRead {0};

    // #### Get the size of the image
    do {
        bytesRead = read(socket, &imageSize, sizeof(long));
    } while (bytesRead < 0);

    if (bytesRead == 0) {
        std::cout << "\nDidn't get any images.";
        return NO_ERROR;
    }

    // #### Open file/image
    imageFD = std::fopen(filePathName.c_str(), "wb");
    if (imageFD == nullptr) {
        filePathName.clear();
        perror("[ERROR]::[fopen]");
        return 0;
    }
    else {} // Nothing to do

    // Вводим путь/имя файла, куда хотим сохранить получаемое изображение
//    while (true) {
//        std::cout << "\nEnter the path/name, where you want to save image:"
//                  << "\nPath: " << std::flush;
//        std::cin >> filePathName;

//        // #### Open file/image
//        imageFD = std::fopen(filePathName.c_str(), "wb");
//        if (imageFD == nullptr) {
//            filePathName.clear();
//            perror("[ERROR]::[fopen]");
//        }
//        else {
//            // Need to send the verification signal, that we are ready to accept the image
//            break;
//        }
//    }


    struct timeval  timeValue = {10, 0};
    fd_set          fileDescriptorSet;

    while(receiveSize < imageSize) {

        // #1 Clear a new set of descriptors (which are living in the [fileDescriptorSet])
        FD_ZERO(&fileDescriptorSet);

        // #2 Add our socket descriptor into the specified set: [fileDescriptorSet]
        FD_SET(socket, &fileDescriptorSet);

        // #3 Wait for status changes of elements in [fileDescriptorSet].
        // ## [timeValue] is a time before get answer.
        retValue = select(FD_SETSIZE, &fileDescriptorSet, NULL, NULL, &timeValue);

        if (retValue < 0) {
            std::perror("[ERROR]::[select]");
            return -1;
        }
        else if (retValue == 0) {
            std::cout << "\n[ERROR]::[select]: buffer read timeout has expired.";
            return -1;
        }
        else {
            bytesRead = read(socket, buffer, bufSize);
            if (bytesRead < 0) {
                std::perror("[ERROR]::[select]");
                return -1;
            }
            else {
                // Пишем в файл изображения данные из буфера (в кол-ве retValue)
                std::fwrite(buffer, 1, bytesRead, imageFD);
                receiveSize += bytesRead;
            }
        }
    }

    std::cout << "Get image size:      " << imageSize << " bytes."
              << "Received image size: " << receiveSize << " bytes."
              << std::endl;
    std::fclose(imageFD);

    // #### Вывод изображения на экран (нужно добавить возможность использования разных утилит
    // #### для разных типов файлов
    std::string displayCommand {"\n eog "};
    std::string commandLine = displayCommand + filePathName;
    std::system(commandLine.c_str());

    return 0;

}
