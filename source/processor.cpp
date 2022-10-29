#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "libs/stack.hpp"
#include "libs/parser.hpp"
#include "console/cpu_func_list.hpp"
#include "command.hpp"
#include "assert.hpp"


const unsigned int SCREEN_WIDTH  = 50;
const unsigned int SCREEN_HEIGHT = 20;
const unsigned int SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH;

const unsigned int REGISTER_SIZE = 4;
const unsigned int RAM_SIZE = 1200;


/// Contains information about process to execute
typedef struct {
    cmd_t *code = nullptr; ///< Operation code 
    size_t count = 0; ///< Operation count

    cmd_t *ip = 0; ///< Instruction pointer

    Stack value_stack = {}; ///< Contains values 
    Stack call_stack = {}; ///< Function backtrace

    arg_t *reg; ///< Process REGISTER
    arg_t *ram; ///< Process RAM
} Process;


/**
 * \brief Allocates process memory
 * \param process Process to allocate
 * \return Non zero value means error
*/
int init_process(Process *process);


/**
 * \brief Reads binary file
 * \param [out] file Input file
 * \param [in]  process Process to read in
 * \return Non zero value means error
*/
int read_file(int file, Process *process);


/**
 * \brief Executes process
 * \param process Process to execute
 * \return Non zero value means error
*/
int execute(Process *process);


/**
 * \brief Prints all information about process
 * \param [in] process Process to print
*/
void print_process(Process *process);


/**
 * \brief Free process
 * \param process Process to free
 * \return Non zero value means error
*/
int free_process(Process *process);


int execute_pop(Process *process, cmd_t **ip, cmd_t cmd, arg_t arg);   ///< Executes pop command
int show_ram(Process *process);                                        ///< Executes show command




int main(int argc, char *argv[]) {
    int input = -1;

    #include "console/cpu_cmd_list.hpp"

    if (parse_args(argc, argv, command_list, sizeof(command_list) / sizeof(Command)))
        return 1;

    if (input == -1)
        return 1;

    Process process = {};

    init_process(&process);

    if (read_file(input, &process))
        return 1;

    close(input);

    if (execute(&process))
        print_process(&process);

    if (free_process(&process))
        return 1;

    printf("Processor!");

    return 0;
}


#define OFFSET(ip) ip - process -> code


#define DEF_CMD(name, arg, action, ...)     \
    case CMD_##name: {                      \
        __VA_ARGS__                         \
        break;                              \
    }

#include "dsl.hpp"

int execute(Process *process) {
    /// SHORTCUTS ///
    cmd_t *ip = process -> ip;

    Stack *stack = &(process -> value_stack);
    Stack *call_stack = &(process -> call_stack);

    int *reg = process -> reg;
    int *ram = process -> ram;


    while((size_t)(OFFSET(ip)) < process -> count) {
        cmd_t cmd = *ip++;
        arg_t arg = 0;

        // stack_dump(stack, 0, stdout);

        // printf("%i\n", cmd);

        switch(cmd & 0x1F) {
            #include "cmd.hpp"

            default: {
                printf("Unknown command %ui in operation %llu!\n", cmd, OFFSET(ip - 1));
                return 1;
            }
        }
    }

    printf("[Warning] No hlt at end of the process!\n");
    return 0;
}


#undef DEF_CMD


int read_file(int file, Process *process) {
    ASSERT(file > -1, "Invalid file!");
    ASSERT(process, "Can't work with then null pointer!");

    char sig[sizeof(SIGN)] = ""; 

    size_t bytes = read(file, &sig, sizeof(SIGN));

    ASSERT(!strnicmp(sig, SIGN, sizeof(SIGN)), "Signature of file doesn't match!");

    int ver = 0;

    bytes += read(file, &ver, sizeof(int));

    ASSERT(ver == VERSION, "Version of file doesn't match!");

    bytes += read(file, &(process -> count), sizeof(size_t));

    process -> code = (cmd_t *) calloc(process -> count, sizeof(cmd_t));
    
    bytes += read(file, process -> code, (unsigned int)(process -> count * sizeof(cmd_t)));

    size_t expected_bytes = sizeof(SIGN) + sizeof(int) + sizeof(size_t) + process -> count * sizeof(cmd_t);

    if (bytes != expected_bytes) {
        printf("Expected bytes %llu, actualy read %llu", expected_bytes, bytes);
        return 1;
    }

    process -> ip = process -> code;

    return 0;
}


int init_process(Process *process) {
    ASSERT(process, "Can't work with then null pointer!");

    process -> reg = (arg_t *) calloc(REGISTER_SIZE, sizeof(arg_t));

    ASSERT(process -> reg, "Can't allocate process reg!");

    process -> ram = (arg_t *) calloc(RAM_SIZE, sizeof(arg_t));

    ASSERT(process -> ram, "Can't allocate process ram!");

    ASSERT(!stack_constructor(&process -> value_stack, 4), "Unable to construct value stack!");
    ASSERT(!stack_constructor(&process -> call_stack, 4), "Unable to construct call stack!");

    return 0;
}


int free_process(Process *process) {
    ASSERT(process -> reg && process -> ram, "Process has invalid ram or register pointers!");

    free(process -> ram);
    process -> ram = nullptr;

    free(process -> reg);
    process -> reg = nullptr;

    ASSERT(!stack_destructor(&process -> value_stack), "Unable to destroy value stack!");
    ASSERT(!stack_destructor(&process -> call_stack), "Unable to destroy call stack!");

    return 0;
}


void print_process(Process *process) {
    /*
    printf("Operation count: %i\n", process -> count);

    for(int i = 0; i < process -> count; i++)
        printf("%i ", process -> code[i]);
    */
    printf("\nRegister:\n");

    for(size_t i = 0; i < REGISTER_SIZE; i++)
        printf("%i ", process -> reg[i]);
    /*
    printf("\nRam:\n");

    for(size_t i = 0; i < RAM_SIZE; i++)
        printf("%i ", process -> ram[i]);
    */
    printf("\nValue stack:\n");

    stack_dump(&process -> value_stack, stack_check(&process -> value_stack), stdout);
    
    printf("Call stack:\n");
    
    stack_dump(&process -> call_stack, stack_check(&process -> call_stack), stdout);

    fflush(stdout);
}


int execute_pop(Process *process, cmd_t **ip, cmd_t cmd, arg_t arg) {
    if (cmd & BIT_MEM) {
        if (cmd & BIT_CONST) {
            arg = *((arg_t *)(*ip));
            *ip += sizeof(arg_t);
        }
        if (cmd & BIT_REG) {
            arg += process -> reg[*((arg_t *)(*ip)) - 1]; // ADD REGISTER INDEX CHECK
            *ip += sizeof(arg_t);
        }

        arg /= PRECISION;

        ASSERT_IP(arg > -1 && arg < (int) RAM_SIZE, "Segmentation fault! Wrong RAM index!", OFFSET(*ip - 1));

        ASSERT_IP(!stack_pop(&process -> value_stack, process -> ram + arg), "Empty stack pop!", OFFSET(*ip - 1));
    }

    else if (cmd & BIT_CONST) {
        int value = 0;
        
        ASSERT_IP(!stack_pop(&process -> value_stack, &value), "Empty stack pop!", OFFSET(*ip - 1));
    }
    
    else if (cmd & BIT_REG) {
        arg = *(arg_t *)(*ip) - 1;
        *ip += sizeof(arg_t);

        ASSERT_IP(arg > -1 && arg < (int) REGISTER_SIZE, "Segmentation fault! Wrong register index!", OFFSET(*ip - 1));

        ASSERT_IP(!stack_pop(&process -> value_stack, process -> reg + arg), "Empty stack pop!", OFFSET(*ip - 1));
    }

    return 0;
}


int show_ram(Process *process) {
    ASSERT(RAM_SIZE >= SCREEN_SIZE, "Ram size is less then screen size!");

    putchar(process -> ram[0]);

    for(unsigned int i = 1; i < SCREEN_SIZE; i++) {
        if (i % SCREEN_WIDTH == 0)
            putchar('\n');

        putchar(process -> ram[i]);
    }

    putchar('\n');

    fflush(stdout);

    return 0;
}

