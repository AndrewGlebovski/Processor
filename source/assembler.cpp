#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "text.hpp"


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    size_t count = 0; ///< Operation count
} Program;


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

    Program program = {};
    printf("%p %llu\n", program.code, program.count);

    printf("Assembler!\n");

    return 0;
}
