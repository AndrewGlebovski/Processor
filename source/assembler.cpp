#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include "text.hpp"


/// Contains information about label
typedef struct {
    int value = 0;
    char *name = nullptr;
} Label;


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr; ///< Operation code 
    int count = 0; ///< Operation count
    int ip = 0; ///< Current operation
    Label *labels = nullptr; ///< Program labels
    int labels_count = 0; ///< Labels count
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
 * \brief Inserts label if it does not exist
 * \param [in] program Program to insert label
 * \param [in] new_label This label will be inserted
 * \note It's impossible to redefine value of label
*/
void insert_label(Program *program, Label *new_label);


/**
 * \brief Gets label value
 * \param [in] program Program to search label in
 * \param [in] label_name Label with this name will be searched
 * \return Actual value or -1 if label not found
*/
int get_label_value(Program *program, char *label_name);


/**
 * \brief Free program memory
 * \param [in] program This program will be reinitialize
 * \return Non zero value means error
*/
int free_program(Program *program);


/**
 * \brief Prints all information about program
 * \param [in] program Program to print
*/
void print_program(Program *program);




int main() {
    int input = open("debug/source.txt", O_RDONLY);

    if (input == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    Text text = {};

    read_file(input, &text);
    /*
    for(int i = 0; i < text.size; i++)
        printf("[%.3i] %s\n", text.lines[i].len, text.lines[i].str);
    */
    Program program = {};
    
    translate(&program, &text);

    print_program(&program);

    free_program(&program);
    free_text(&text);

    printf("Assembler!\n");

    return 0;
}


int translate(Program *program, Text *text) {
    program -> code = (int *) calloc(text -> size * 2, sizeof(int));
    program -> labels = (Label *) calloc(text -> size, sizeof(Label));

    if (!program -> code) {
        printf("Failed to allocate memory!\n");
        return 1;
    }

    for(int i = 0; (text -> lines)[i].str != nullptr && (text -> lines)[i].len != -1; i++) {
        /*
        const char *comment  = strchr((text -> lines)[i].str, '#');
        if (comment)
            (text -> lines)[i].str[comment - (text -> lines)[i].str] = '\0';
        */

        if (strchr((text -> lines)[i].str, ':')) {
            (text -> lines)[i].str[strchr((text -> lines)[i].str, ':') - (text -> lines)[i].str] = '\0';

            Label new_label = {program -> ip, (text -> lines)[i].str};
            insert_label(program, &new_label);
            continue;
        }

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
        else if (strcmp(cmd, "jmp") == 0) {
            (program -> code)[program -> ip++] = CMD_JMP;
            (program -> code)[program -> ip++] = get_label_value(program, (text -> lines)[i].str + n + 1);
        }
        else {
            printf("Unknown command %s\n", cmd);
            return 1;
        }
    }

    program -> count = program -> ip;
    program -> ip = 0;

    program -> code = (int *) realloc(program -> code, program -> count * sizeof(int));
    program -> labels = (Label *) realloc(program -> labels, program -> labels_count * sizeof(Label));

    if (!program -> code || !program -> labels) {
        printf("Failed to reallocate memory!\n");
        return 1;
    }

    return 0;
}


void insert_label(Program *program, Label *new_label) {
    for(int i = 0; i < program -> labels_count; i++) {
        if (strcmp((program -> labels)[i].name, new_label -> name) == 0)
            return;
    }

    program -> labels[program -> labels_count++] = *new_label;
}


int get_label_value(Program *program, char *label_name) {
    for(int i = 0; i < program -> labels_count; i++) {
        if (strcmp((program -> labels)[i].name, label_name) == 0) {
            return (program -> labels)[i].value;
        }
    }

    return -1;
}


int free_program(Program *program) {
    if (!program -> code || !program -> labels) {
        printf("Failed to free memory due to null pointer!\n");
        return 1;
    }

    free(program -> code);
    program -> code = nullptr;

    free(program -> labels);
    program -> labels = nullptr;

    program -> count = 0;
    program -> ip = 0;

    return  0;
}


void print_program(Program *program) {
    printf("Operation count: %i\n", program -> count);

    for(int i = 0; i < program -> count; i++)
        printf("%i ", program -> code[i]);

    printf("\nLabels count: %i\n", program -> labels_count);
    for(int i = 0; i < program -> labels_count; i++)
        printf("%s %i\n", program -> labels[i].name, program -> labels[i].value);
}
