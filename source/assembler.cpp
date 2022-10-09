#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include "text.hpp"


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    size_t count = 0; ///< Operation count
    size_t ip = 0; ///< Current operation
} Program;


/// List of commands
typedef enum {
    CMD_HLT  = 0, ///< Program end
    CMD_PUSH = 1, ///< Push number to stack
    CMD_OUT  = 2, ///< Print number
    CMD_ADD  = 3, ///< Add two numbers
    CMD_SUB  = 4, ///< Subtract two numbers
    CMD_MUL  = 5, ///< Multiply two numbers
    CMD_DIV  = 6, ///< Divide two numbers
} COMMANDS;


/**
 * \brief Translate text from file to actual code
 * \param [out] program This struct will be filled with information
 * \param [in]  text Text to analyze
 * \return Non zero value means error
 * \note Don't forget to free code before and after using translate
*/
int translate(Program *program, Text *text);


/**
 * \brief Free program memory
 * \param [in] program This program will be reinitialize
 * \return Non zero value means error
*/
int free_program(Program *program);


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
    
    translate(&program, &text);

    printf("%llu\n", program.count);
    for(size_t i = 0; i < program.count; i++)
        printf("%i ", program.code[i]);
    putchar('\n');

    free_program(&program);
    free_text(&text);

    printf("Assembler!\n");

    return 0;
}


int translate(Program *program, Text *text) {
    program -> code = (int *) calloc(text -> size * 2, sizeof(int));

    if (!program -> code) {
        printf("Failed to allocate memory!\n");
        return 1;
    }

    for(int i = 0; (text -> lines)[i].str != nullptr && (text -> lines)[i].len != -1; i++) {
        int n = 0;
        char cmd[10] = "";
        sscanf((text -> lines)[i].str, "%s%n", cmd, &n);

        if (strcmp(cmd, "hlt") == 0) {
            (program -> code)[program -> ip++] = CMD_HLT;
            break;
        }
        else if (strcmp(cmd, "push") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to push at line %i!\n", i);
                return 1;
            }

            (program -> code)[program -> ip++] = CMD_PUSH;
            (program -> code)[program -> ip++] = value;
        }
        else if (strcmp(cmd, "out") == 0) {
            (program -> code)[program -> ip++] = CMD_OUT;
        }
        else if (strcmp(cmd, "add") == 0) {
            (program -> code)[program -> ip++] = CMD_ADD;
        }
        else if (strcmp(cmd, "sub") == 0) {
            (program -> code)[program -> ip++] = CMD_SUB;
        }
        else if (strcmp(cmd, "mul") == 0) {
            (program -> code)[program -> ip++] = CMD_MUL;
        }
        else if (strcmp(cmd, "div") == 0) {
            (program -> code)[program -> ip++] = CMD_DIV;
        }
        else {
            printf("Unknown command %s\n", cmd);
            return 1;
        }
    }

    program -> count = program -> ip;
    program -> ip = 0;

    program -> code = (int *) realloc(program -> code, program -> count * sizeof(int));

    if (!program -> code) {
        printf("Failed to reallocate memory!\n");
        return 1;
    }

    return 0;
}


int free_program(Program *program) {
    if (!program -> code) {
        printf("Failed to free memory due to null pointer!\n");
        return 1;
    }

    free(program -> code);
    program -> code = nullptr;

    program -> count = 0;
    program -> ip = 0;

    return  0;
}
