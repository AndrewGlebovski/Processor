/**
 * \file
 * \brief Text module source
*/

#if defined(_WIN32) || defined(_WIN64)
    #include <io.h>
#elif __linux__
    #include <unistd.h>
#else
    #error "Your system case is not defined!"
#endif

#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include "text.hpp"




int read_text(Text *text, int file) {
    size_t size = get_file_size(file);

    char *buffer = nullptr;

    size = read_in_buffer(file, &buffer, (unsigned int) size);

    int lines = 1 + replace_in_buffer(buffer, '\n', '\0');

    text -> lines = (String *) calloc(lines + 1, sizeof(String));

    text -> buffer = buffer;

    for(int l = 0; l < lines; l++) {
        text -> lines[l] = {buffer, 0};

        while(*buffer != '\0')
            buffer++;

        text -> lines[l].len = (int)(buffer - (text -> lines)[l].str);

        buffer++;
    }

    text -> lines[lines] = {nullptr, -1};
    text -> size = lines;

    return 0;
}


size_t read_in_buffer(int file, char **buffer, size_t size) {
    *buffer = (char *) calloc(size + 1, sizeof(char));
    
    size = read(file, *buffer, (unsigned int) size * sizeof(char));

    *buffer = (char *) realloc(*buffer, size + 1);

    (*buffer)[size] = '\0';

    return size;
}


int replace_in_buffer(char *buffer, const char old_char, const char new_char) {
    int count = 0;

    for(size_t i = 0; buffer[i]; i++) {
        if (buffer[i] == old_char) {
            buffer[i] = new_char;
            count++;
        }
    }

    return count;
}


size_t get_file_size(int file) {
    struct stat info = {};
    fstat(file, &info);
    return info.st_size;
}


int print_lines(String lines[], int file) {
    for(int i = 0; lines[i].str != nullptr && lines[i].len != -1; i++) {
        write(file, lines[i].str, lines[i].len * sizeof(char));
        write(file, "\n", sizeof(char));
    }

    return 0;
}


int free_text(Text *text) {
    free(text -> lines);
    text -> lines = nullptr;

    free(text -> buffer);
    text -> buffer = nullptr;

    text -> size = 0;

    return 0;
}
