
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include "stack.hpp"
#include "parser.hpp"

#include "command.hpp"


const unsigned int SCREEN_WIDTH  = 50;
const unsigned int SCREEN_HEIGHT = 20;
const unsigned int SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH;

const unsigned int REGISTER_SIZE = 4;
const unsigned int RAM_SIZE = 1000;


/**
 * \brief Shell for stack pop that throws error
 * \param [in]  stack_ptr Pointer to stack
 * \param [out] value_ptr Pointer to value for stack pop
 * \param [in]  ip Current instruction pointer
*/
#define STACK_POP(stack_ptr, value_ptr, ip) \
do { \
    if (stack_pop(stack_ptr, value_ptr)) { \
        printf("Empty stack pop in operation %i!\n", ip); \
        return 1; \
    } \
} while(0)


/**
 * \brief Shell for stack push that throws error
 * \param [in]  stack_ptr Pointer to stack
 * \param [out] value Value for stack push
 * \param [in]  ip Current instruction pointer
*/
#define STACK_PUSH(stack_ptr, value, ip) \
do { \
    if (stack_push(stack_ptr, value)) { \
        printf("Stack push in operation %i!\n", ip); \
        return 1; \
    } \
} while(0)


/// Contains information about process to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    int count = 0; ///< Operation count

    int ip = 0; ///< Instruction pointer

    Stack value_stack = {}; ///< Contains values 
    Stack call_stack = {}; ///< Function backtrace

    int reg[REGISTER_SIZE] = {0}; ///< Process REGISTER
    int ram[RAM_SIZE] = {0}; ///< Process RAM
} Process;


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


int execute_pop(Process *process, int *ip, int cmd, int arg);   ///< Executes pop command
int show_ram(Process *process);                                 ///< Executes show command


void set_input_file(char *argv[], void *data);  ///< -i parser
void show_help(char *argv[], void *data);       ///< -h parser




int main(int argc, char *argv[]) {
    int input = -1;

    Command command_list[] = {
        {
            "-i", "--input", 
            0, 
            &set_input_file, 
            &input,
            "<relative path to a file> Changes input file"
        },
        {
            "-h", "--help", 
            0, 
            &show_help, 
            &command_list,
            "Prints all commands descriptions"
        },
    };

    if (parse_args(argc, argv, command_list, sizeof(command_list) / sizeof(Command)))
        return 1;

    if (input == -1)
        return 1;

    Process process = {};

    stack_constructor(&process.value_stack, 32);
    stack_constructor(&process.call_stack, 32);

    read_file(input, &process);

    close(input);

    if (execute(&process))
        print_process(&process);

    free(process.code);

    printf("Processor!");

    return 0;
}


#define DEF_CMD(name, arg, action, ...) \
    case CMD_##name: { \
        __VA_ARGS__ \
        break; \
    }


int execute(Process *process) {
    /// SHORTCUTS ///
    int *ip = &(process -> ip);

    Stack *stack = &(process -> value_stack);
    Stack *call_stack = &(process -> call_stack);

    int *reg = process -> reg;
    int *ram = process -> ram;


    while(*ip < process -> count) {
        int cmd = process -> code[(*ip)++], arg = 0;

        switch(cmd & 0XFFFFFF) {
            #include "cmd.hpp"

            default: {
                printf("Unknown command %i in operation %i!\n", cmd, *ip);
                return 1;
            }
        }
    }

    stack_destructor(stack);

    printf("[Warning] No hlt at end of the process!\n");
    return 1;
}


#undef DEF_CMD


int read_file(int file, Process *process) {
    if (file == -1) {
        printf("Invalid file!\n");
        return 1;
    }

    if (!process) {
        printf("Can't work with then null pointer!\n");
        return 1;
    }

    int b = read(file, &(process -> count), sizeof(int));

    process -> code = (int *) calloc(process -> count, sizeof(int));
    
    b += read(file, process -> code, (unsigned int) process -> count * sizeof(int));

    if (b != (process -> count + 1) * (int) sizeof(int)) {
        printf("Expected bytes %i, actualy read %i", b, (process -> count + 1) * (int) sizeof(int));
        return 1;
    }

    return 0;
}


void print_process(Process *process) {
    printf("Operation count: %i\n", process -> count);

    for(int i = 0; i < process -> count; i++)
        printf("%i ", process -> code[i]);

    printf("\nRegister:\n");

    for(size_t i = 0; i < sizeof(process -> reg) / sizeof(*process -> reg); i++)
        printf("%i ", process -> reg[i]);

    printf("\nRam:\n");

    for(size_t i = 0; i < sizeof(process -> ram) / sizeof(*process -> ram); i++)
        printf("%i ", process -> ram[i]);

    printf("\nValue stack:\n");

    stack_dump(&process -> value_stack, stack_check(&process -> value_stack), stdout);
    
    printf("Call stack:\n");
    
    stack_dump(&process -> call_stack, stack_check(&process -> call_stack), stdout);

    fflush(stdout);
}


int execute_pop(Process *process, int *ip, int cmd, int arg) {
    if (cmd & BIT_MEM) {
        if (cmd & BIT_CONST) arg = (process -> code)[(*ip)++];
        if (cmd & BIT_REG) arg += process -> reg[(process -> code)[(*ip)++] - 1]; // ADD REGISTER INDEX CHECK

        arg /= PRECISION;

        if (arg < 0 || arg > (int) (sizeof(process -> ram) / sizeof(*process -> ram)) - 1) {
            printf("Segmentation fault! Wrong RAM index in operation %i!\n", *ip);
            return 1;
        }

        STACK_POP(&process -> value_stack, process -> ram + arg, *ip);
    }
    else if (cmd & BIT_CONST) {
        int value = 0;
        STACK_POP(&process -> value_stack, &value, *ip);
    }
    else if (cmd & BIT_REG) {
        arg = (process -> code)[(*ip)++];

        if (arg < 1 || arg > (int) (sizeof(process -> reg) / sizeof(*process -> reg))) {
            printf("Segmentation fault! Wrong register index in operation %i!\n", *ip);
            return 1;
        }

        STACK_POP(&process -> value_stack, process -> reg + arg - 1, *ip);
    }

    return 0;
}


int show_ram(Process *process) {
    if (RAM_SIZE < SCREEN_SIZE) {
        printf("Ram size is less then screen size!\n");
        return 1;
    }

    if (process -> ram[0]) putchar('*');
    else putchar('.');

    for(unsigned int i = 1; i < SCREEN_SIZE; i++) {
        if (i % SCREEN_WIDTH == 0)
            putchar('\n');

        if (process -> ram[i]) putchar('*');
        else putchar('.');
    }

    putchar('\n');

    return 0;
}


void set_input_file(char *argv[], void *data) {
	if (*(++argv)) {
		*(int *)(data) = open(*argv, O_RDONLY | O_BINARY);

        if (*(int *)(data) == -1)
            printf("Can't open file %s!\n", *argv);
	}
	else {
		printf("No filename after -i, argument ignored!\n");
	}
}


void show_help(char *argv[], void *data) {
	for(size_t i = 0; i < 2; i++) {
		printf("%s %s %s\n", ((Command *)(data))[i].short_name, ((Command *)(data))[i].long_name, ((Command *)(data))[i].desc);
	}
}
