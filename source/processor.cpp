#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include "stack.hpp"
#include "command.hpp"


#define STACK_POP(stack_ptr, value_ptr, ip) \
do { \
    if (stack_pop(stack_ptr, value_ptr)) { \
        printf("Empty stack pop in operation %i!\n", ip); \
        return 1; \
    } \
} while(0);


#define STACK_PUSH(stack_ptr, value, ip) \
do { \
    if (stack_push(stack_ptr, value)) { \
        printf("Stack push in operation %i!\n", ip); \
        return 1; \
    } \
} while(0);


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    int count = 0; ///< Operation count
    int ip = 0; ///< Current operation
} Program;


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

    int reg[4] = {};
    int ram[100] = {};

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
                STACK_POP(&stack, &value, program -> ip - 1);
                printf("%i\n", value);
                break;
            }

            case CMD_PUSH: {
                if (cmd & BIT_CONST) arg = (program -> code)[program -> ip++];
                if (cmd & BIT_REG) arg += reg[(program -> code)[program -> ip++]];
                if (cmd & BIT_MEM) arg = ram[arg];

                STACK_PUSH(&stack, arg, program -> ip - 1);
                break;
            }

            case CMD_POP: {
                if (cmd & BIT_MEM) {
                    if (cmd & BIT_CONST) arg = (program -> code)[program -> ip++];
                    if (cmd & BIT_REG) arg += reg[(program -> code)[program -> ip++]];

                    if (arg < 0 || arg > 99) {
                        printf("Segmentation fault! Wrong RAM index in operation %i!\n", program -> ip - 1);
                        return 1;
                    }

                    STACK_POP(&stack, &ram[arg], program -> ip - 1);
                }
                else if (cmd & BIT_CONST) {
                    int value = 0;
                    STACK_POP(&stack, &value, program -> ip - 1);
                }
                else if (cmd & BIT_REG) {
                    arg = (program -> code)[program -> ip++];

                    if (arg < 1 || arg > 4) {
                        printf("Segmentation fault! Wrong register index in operation %i!\n", program -> ip - 1);
                        return 1;
                    }

                    STACK_POP(&stack, &reg[arg - 1], program -> ip - 1);
                }

                break;
            }
            
            case CMD_DUP: {
                int value = 0;
                STACK_POP(&stack, &value, program -> ip - 1);
                STACK_PUSH(&stack, value, program -> ip - 1);
                STACK_PUSH(&stack, value, program -> ip - 1);
                break;
            }

            case CMD_ADD: {
                int val1 = 0, val2 = 0;
                STACK_POP(&stack, &val1, program -> ip - 1);
                STACK_POP(&stack, &val2, program -> ip - 1);
                STACK_PUSH(&stack, val2 + val1, program -> ip - 1);
                break;
            }

            case CMD_SUB: {
                int val1 = 0, val2 = 0;
                STACK_POP(&stack, &val1, program -> ip - 1);
                STACK_POP(&stack, &val2, program -> ip - 1);
                STACK_PUSH(&stack, val2 - val1, program -> ip - 1);
                break;
            }

            case CMD_MUL: {
                int val1 = 0, val2 = 0;
                STACK_POP(&stack, &val1, program -> ip - 1);
                STACK_POP(&stack, &val2, program -> ip - 1);
                STACK_PUSH(&stack, val2 * val1, program -> ip - 1);
                break;
            }

            case CMD_DIV: {
                int val1 = 0, val2 = 0;
                STACK_POP(&stack, &val1, program -> ip - 1);
                STACK_POP(&stack, &val2, program -> ip - 1);
                STACK_PUSH(&stack, val2 / val1, program -> ip - 1);
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
