#include "fileio.h"
#include "svec.h"
#include <stdio.h>

FILE* masm_fileio_get_file(char* inputFileName) {
    FILE* file = fopen(inputFileName, "r");
    if (file == NULL) {
        return NULL;
    }
    return file;
}


char* masm_fileio_read_file_to_buffer(char* fileName)  {
    FILE* file = masm_fileio_get_file(fileName);
    // Checks if the file is found
    if (file == NULL) {
        fprintf(stderr, "masm: error: file not found \"%s\"\n", fileName) ;
        return NULL;
    }
    // Moves the file internal position to the end
    fseek(file, 0, SEEK_END);
    // Gets the size of the file
    int fileLength = ftell(file);
    // Rewinds the file position to the beginning 
    rewind(file);

    // Create a file buffer
    char* buffer = xmalloc(fileLength + 1);
    
    // Reads all the file contents to the buffer
    fread(buffer, 1, fileLength, file);

    // Appends a null term to print out 
    buffer[fileLength] = '\0';
    return buffer;
}


