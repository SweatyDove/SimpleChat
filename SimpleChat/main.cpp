


#include "main.h"
#include "my_tcpsocket.h"


/* Идея:
 *
 * 1) При запуске программы должна открываться её серверная часть. И если кто-то
 * хочет написать - можно будет сразу получить данные. То есть это поток
 * номер 1.
 *
 * 2) Наверное, нужен ещё поток 0, который будет обрабатывать команды,
 * отправляемые пользователем.
 *
 * 3) Далее, если я кто-то хочет подключиться ко мне, кого я не знаю (его нет
 * в списке моих контактов) - тогда я должен дать своё согласие. В результате
 * я автоматически буду получать от него сообщения.
 *
 * 4) Если же я кому-то хочу написать - то выполняю комманду (напр.
 * connect IP port). Хотя на счёт порта я не уверен, так как мой чат должен
 * работать в фоновом режиме, чтобы общаться через него.
 *
 * 5) Если всё хорошо, то нужно ли открывать новое окно для общения? Или же
 * будет достаточно основного? Наверное, должен быть один терминал для команд
 * и основной информации. И далее, если кто-то соединяется (или я с кем-то
 * соединяюсь), то открывать отдельный терминал для общения через него.
 * То есть один чат - одно окно.
 *
 */







//=============================================================================
//
//=============================================================================
int main()
{
    my::TCPSocket serverSocket("127.0.0.1", 8888);



    int socket = serverSocket.acceptPendingConnection();



    // #### Работа с клиентом
    // ####
    int dataType {0};                   // Тип получаемых данных (сообщение, картинка, файл и т.д.)

    int attempt {0};                    // Номер текущей попытки получить данные от клиента
    int maxAttempt {10};                // Максимальное число таких попыток


    ssize_t bytesRead {0};


    bool inLoop {true};
    while (inLoop) {

        // Определяем тип данных, которые поступят далее
        bytesRead = read(socket, &dataType, sizeof(int));
        if (bytesRead < 0) {
             std::perror("[WARNING]::[read]");
        }
        else {
            switch (dataType) {

            // Клиент закончил передачу (в явном виде)
            case STOP_TRANSFER:
                std::cout << "\nConnection closed due to the STOP_TRANSFER signal from the client." << std::endl;
                inLoop = false;
                break;

            // От клиента нет данных
            case NO_DATA:
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                attempt++;
                if (attempt >= maxAttempt) {
                    inLoop = false;
                }
                else {} // Nothing to do
                break;

            // От клиента придёт сообщение
            case MESSAGE:
                receiveMessage(socket);
                dataType = NO_DATA;
                attempt = 0;
                break;

            // От клиента придёт изображение
//            case IMAGE:
//                receiveImage(socket);
//                dataType = NO_DATA;
//                attempt = 0;
//                break;

            // Некорректный тип данных
            default:
                std::cout << "\n[WARNING]: incorrect type of input data: " << dataType << "."
                          << std::endl;
                if (attempt < maxAttempt) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    attempt++;
                }
                else {
                    std::cout << "\nConnection closed due to the inability to receive correct data-type-signal from client";
                    inLoop = false;
                }
                break;
            } // End of switch()
        } // End of if-else()
    } // End of while()


    // #### Закрываем соединение с клиентом
    close(socket);

    // #### Отключаем как чтение, так и запись связанную с socketServer
    //shutdown(serverSocketPart, SHUT_RDWR);
    return 0;

}














