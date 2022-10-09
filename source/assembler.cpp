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

    printf("Assembler!\n");

    return 0;
}
