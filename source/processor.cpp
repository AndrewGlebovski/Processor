#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include "stack.hpp"


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    int count = 0; ///< Operation count
    int ip = 0; ///< Current operation
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
    CMD_JMP  = 7, ///< Jump to the specific line of code
    CMD_DUP  = 8, ///< Duplicates last number
    CMD_POP  = 9, ///< Pop into register or memory
} COMMANDS;


/// Argument type bit
typedef enum {
    BIT_CONST = 0x1000000, ///< Constant bit
    BIT_REG   = 0x2000000, ///< Register bit
    BIT_MEM   = 0x4000000, ///< Memory bit
} ARG_TYPE;


/**
 * \brief Reads binary file
 * \param [out] file Input file
 * \param [in]  program Program to read in
 * \return Non zero value means error
*/
int read_file(int file, Program *program);


/**
 * \brief Executes program
 * \param program Program to execute
 * \return Non zero value means error
*/
int execute(Program *program);


/**
 * \brief Prints all information about program
 * \param [in] program Program to print
*/
void print_program(Program *program);




int main() {
    int input = open("debug/binary.txt", O_RDONLY | O_BINARY);

    Program program = {};

    read_file(input, &program);

    close(input);

    print_program(&program);

    execute(&program);

    free(program.code);

    printf("Processor!");

    return 0;
}


int execute(Program *program) {
    Stack stack = {};
    stack_constructor(&stack, 32);

    int reg[] = {0, 0, 0, 0};

    while(program -> ip < program -> count) {
        int cmd = (program -> code)[program -> ip++], arg = 0;

        switch(cmd & 0XFFFFFF) {
            case CMD_HLT: {
                stack_dump(&stack, 0, stdout);
                stack_destructor(&stack);
                return 0;
            }
            
            case CMD_OUT: {
                int value = 0;
                stack_pop(&stack, &value);
                printf("%i\n", value);
                break;
            }

            case CMD_PUSH: {
                if (cmd & BIT_CONST) arg = (program -> code)[program -> ip++];
                if (cmd & BIT_REG) arg += reg[(program -> code)[program -> ip++]];

                stack_push(&stack, arg);
                break;
            }

            case CMD_POP: {
                if (cmd & BIT_CONST) {
                    int value = 0;
                    stack_pop(&stack, &value);
                }
                else if (cmd & BIT_REG) {
                    arg = (program -> code)[program -> ip++];
                    stack_pop(&stack, &reg[arg]);
                }

                break;
            }
            
            case CMD_DUP: {
                int value = 0;
                stack_pop(&stack, &value);
                stack_push(&stack, value);
                stack_push(&stack, value);
                break;
            }

            case CMD_ADD: {
                int val1 = 0, val2 = 0;
                stack_pop(&stack, &val1);
                stack_pop(&stack, &val2);
                stack_push(&stack, val2 + val1);
                break;
            }

            case CMD_SUB: {
                int val1 = 0, val2 = 0;
                stack_pop(&stack, &val1);
                stack_pop(&stack, &val2);
                stack_push(&stack, val2 - val1);
                break;
            }

            case CMD_MUL: {
                int val1 = 0, val2 = 0;
                stack_pop(&stack, &val1);
                stack_pop(&stack, &val2);
                stack_push(&stack, val2 * val1);
                break;
            }

            case CMD_DIV: {
                int val1 = 0, val2 = 0;
                stack_pop(&stack, &val1);
                stack_pop(&stack, &val2);
                stack_push(&stack, val2 / val1);
                break;
            }

            case CMD_JMP: {
                arg = (program -> code)[program -> ip++];

                if (arg == -1) {
                    printf("Jump to -1 in operation %i!\n", program -> ip - 1);
                    return -1;
                }

                program -> ip = arg;
                break;
            }

            default: {
                printf("Unknown command %i!\n", (program -> code)[program -> ip]);
                return 1;
            }
        }
    }

    stack_destructor(&stack);

    printf("[Warning] No hlt at end of the program!\n");
    return 1;
}


int read_file(int file, Program *program) {
    if (file == -1) {
        printf("Invalid file!\n");
        return 1;
    }

    if (!program) {
        printf("Can't work with then null pointer!\n");
        return 1;
    }

    int b = read(file, &(program -> count), sizeof(int));

    program -> code = (int *) calloc(program -> count, sizeof(int));
    
    b += read(file, program -> code, (unsigned int) program -> count * sizeof(int));

    if (b != (program -> count + 1) * (int) sizeof(int)) {
        printf("Expected bytes %i, actualy read %i", b, (program -> count + 1) * (int) sizeof(int));
        return 1;
    }

    return 0;
}


void print_program(Program *program) {
    printf("Operation count: %i\n", program -> count);

    for(int i = 0; i < program -> count; i++)
        printf("%i ", program -> code[i]);

    putchar('\n');
}
