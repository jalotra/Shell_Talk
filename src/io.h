
#ifndef IO_H
#define IO_H

#include <string.h>
#include <unistd.h>

// Reads into buff from stdin assuming that the maxSize is the allocated size of buff
int read_from_screen(char* buff, int maxSize);

// Writes from buff to stdout,
int write_to_screen(char* buff);

#endif


