#ifndef FILEIO_H
#define FILEIO_H

#include <stdbool.h>
#include <stdio.h>

FILE* masm_fileio_get_file(char* inputFileName);

char* masm_fiolio_read_file_to_buffer(char* fileName);

#endif
