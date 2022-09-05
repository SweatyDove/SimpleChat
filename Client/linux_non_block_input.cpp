#include "header.h"

// #### Linux realization of khibit() function (from Windows's <conio.h>)
// ####
int linux_kbhit()
{
    struct timeval timeValue = {0, 0};                  // Representation of time with more accuracy
    fd_set fileDescriptorSet;                           // Buffer for the file's descriptor set.
    int maxDescriptorNumber {0};                        // Max integer number, which descriptor can have.
                                                        // (if fd1 = 0, fd2 = 1, fd3 = 10, then need to use 10)
    int selectRetValue {0};

    // #1 Clear a new set of descriptors (which are living in the [fileDescriptorSet])
    FD_ZERO(&fileDescriptorSet);

    // #2 Add a [STDIN_FILENO] to the specified set: [fileDescriptorSet]
    FD_SET(STDIN_FILENO, &fileDescriptorSet);

    // #3 Wait for status changes of elements in [fileDescriptorSet].
    // ## [timeValue] is a time before get answer.
    selectRetValue = select(maxDescriptorNumber + 1, &fileDescriptorSet, NULL, NULL, &timeValue);

    return selectRetValue;
}




// #### Set (or unset) CANONICAL mode of input via linux terminal.
// #### It means, that we don't need to press "Enter" after each keystroke.
// ####
void linuxTerminalMode(bool mode)
{
    struct termios settings;
    static bool firstIn {true};
    static struct termios canonicalSettings;


    // ## Get the parameters, associated with the object [STDIN_FILENO] (standart input, fd = 0).
    // ## Then write these parameters into the [settings]. (I "suppose", that, by-default, CANONICAL
    // ## mode is active - and I have to save it's settings.)
    tcgetattr(STDIN_FILENO, &settings);

    // ## Save the canonical mode settings for STDIN_FILENO.
    // ## If [(mask ^ (a & mask)) != 0] then [mask] isn't set in [a].
    if (firstIn && (ICANON ^ (settings.c_cflag & ICANON))) {
        assert("Default state is non-CANONICAL");
    }
    // ## CANONICAL is active by default
    else if (firstIn) {
        canonicalSettings = settings;
        firstIn = false;
    }
    else {} // Nothing to do ##


    // ## Turn on/off CANONICAL mode
    if (mode == CANONICAL) {
        tcsetattr(STDIN_FILENO, TCSANOW, &canonicalSettings);
        //settings.c_lflag |= ECHO;
    }
    else {
        settings.c_lflag &= ~ICANON;
        //settings.c_lflag &= ~ECHO;          // Make input invisible
        settings.c_cc[VTIME] = 0;           // Set time for waiting non-canonical input
        settings.c_cc[VMIN] = 1;            // Set num of chars for !CANONICAL read

        // ## Set the [STDIN_FILENO] attributes, specified in [settings].
        // ## [TCSANOW] means that the changes occur immediately.
        tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    }


    return;
}
