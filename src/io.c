#include "io.h"
#include <assert.h>

int read_from_screen(char* buff, int maxSize){
    int readBytes = read(STDIN_FILENO, buff, maxSize - 1);
    assert(readBytes != -1);
    buff[readBytes] = '\0';
    return readBytes;
}

int write_to_screen(char* buff){
    int printBytes = write(STDOUT_FILENO, buff, strlen(buff));
    assert(printBytes != -1);
    return printBytes;
}