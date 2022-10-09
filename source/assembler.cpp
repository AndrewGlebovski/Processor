#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "text.hpp"


int main() {
    int input = open("debug/source.txt", O_RDONLY);

    if (input == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    Text text = {};

    read_file(input, &text);

    for(int i = 0; i < text.size; i++)
        printf("[%.3i] %s\n", text.lines[i].len, text.lines[i].str);

    printf("Assembler!\n");

    return 0;
}
