#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>


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

    free(program.code);

    printf("Processor!");

    return 0;
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
