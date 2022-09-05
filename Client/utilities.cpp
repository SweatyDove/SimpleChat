
#include "header.h"


// #### Функция записывает строку в буфер
int getString(std::string& message)
{
    int size {};
    char ch {'\0'};
    //int (*keystroke)(void) = linux_kbhit;

    // #### This loop is responsible for getting number
    while (true) {

        size = message.size();
        // Если нажали какую-то клавишу
        if (linux_kbhit()) {
            // Считываем её
            ch = std::cin.get();

            // Если ESC - то выход из режима ввода
            if (ch == 27) {
                for (int ii {0}; ii < message.size(); ++ii) {
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




void clearWorkScreen(int lines, int columns)
{
    // ## Clear the work screen
    for (int ii {0}; ii < lines; ++ii) {
        for (int jj {0}; jj < columns; ++jj) {
           std::cout << ' ';
        }
        std::cout << std::endl;
    }

    // ## Return cursor to the start position
    for (int ii {0}; ii < lines; ++ii) {
        std::cout << MOVE_CURSOR_ONE_LINE_UP;
    }

    return;
}

// Move cursor in the specified position (@@relative_line > 0, then move forward; @@relative_line < 0 - move backward; @@relative_line == 0 - stay at the same line)
void moveCursor(int relative_line, unsigned int column)
{
    //int mask = 0;
    //unsigned char* forward  = "\n";                                     // New line
    //unsigned char* backward = "\033[F";                                 // Previous line
    const char* shift    = nullptr;                                  // Set the direction of the cursor movement

    shift = (relative_line > 0) ? NEW_LINE : MOVE_CURSOR_ONE_LINE_UP;
    //mask = relative_line >> 31;
    //relative_line = (relative_line + mask) ^ mask;				// Take modulo (if <int> has 32 bits!)

    relative_line = (relative_line >= 0) ? relative_line : -relative_line;       // Take modulo

    // Set line position
    while (relative_line-- > 0) {
        std::cout << shift;
    }

    // Set column position
    std::cout.put('\r');
    while (column-- > 0) {
        std::cout.put(' ');
    }

    return;
}




