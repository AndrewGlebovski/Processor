#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "text.hpp"
#include "command.hpp"


/// Sets operation code and prints that to the listing file
#define SET_OPERATION(CMD) \
do { \
    (program -> code)[program -> ip++] = CMD; \
    fprintf(listing, "%.4i %.8X             %s\n", program -> ip - 1, CMD, (text -> lines)[i].str); \
} while(0)


/// Sets operation code and arg and prints that to the listing file
#define SET_OPERATION_AND_ARG(CMD, ARG) \
do { \
    (program -> code)[program -> ip++] = CMD; \
    (program -> code)[program -> ip++] = ARG; \
    fprintf(listing, "%.4i %.8X % .4i       %s\n", program -> ip - 2, CMD, ARG, (text -> lines)[i].str); \
} while(0)


/// Sets operation code and args and prints that to the listing file
#define SET_OPERATION_AND_ARGS(CMD, ARG1, ARG2) \
do { \
    (program -> code)[program -> ip++] = CMD; \
    (program -> code)[program -> ip++] = ARG1; \
    (program -> code)[program -> ip++] = ARG2; \
    fprintf(listing, "%.4i %.8X % .4i % .4i %s\n", program -> ip - 3, CMD, ARG1, ARG2, (text -> lines)[i].str); \
} while(0)


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


/**
 * \brief Translate text from file to actual code
 * \param [out] program This struct will be filled with information
 * \param [in]  text Text to analyze
 * \param [in]  listing File for listing
 * \return Non zero value means error
 * \note Don't forget to allocate code and labels before and free code and labels after using translate
*/
int translate(Program *program, Text *text, FILE *listing);


/**
 * \brief Write binary output to file
 * \param [out] file Output file
 * \param [in]  program Program to write
 * \return Non zero value means error
*/
int write_file(int file, Program *program);


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


/**
 * \brief Finds token in string
 * \param [in] origin Search start pointer
 * \param [in] solo Solo delimeters
 * \return Token pointer and size
*/
String get_token(char *origin, const char *solo);


/**
 * \brief Compares two strings structs
 * \param [in] str1 First string to compare
 * \param [in] str2 Second string to compare
 * \return Non zero value means equality
*/
int is_equal(String *str1, String *str2);


/**
 * \brief Prints string
 * \param [in] str String to print
*/
void print_string(String *str);




int main() {
    int input = open("debug/source.txt", O_RDONLY);

    if (input == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    Text text = {};

    read_file(input, &text);

    close(input);
    /*
    for(int i = 0; i < text.size; i++)
        printf("[%.3i] %s\n", text.lines[i].len, text.lines[i].str);
    */

    for(int i = 0; i < text.size; i++) {
        String str = get_token(text.lines[i].str, "[+]:");
        while(str.len != -1) {
            print_string(&str);
            str = get_token(str.str + str.len, "[+]:");
        }
    }

    Program program = {};
    
    program.code = (int *) calloc(text.size * 3, sizeof(int));
    program.labels = (Label *) calloc(text.size, sizeof(Label));

    FILE *listing = fopen("debug/listing.txt", "w");

    fprintf(listing, "First pass\n");
    if (translate(&program, &text, listing))
        return 1;

    fprintf(listing, "\nSecond pass\n");
    if (translate(&program, &text, listing))
        return 1;

    //print_program(&program);

    int output = open("debug/binary.txt", O_WRONLY | O_CREAT | O_BINARY);

    if (output == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    write_file(output, &program);

    close(output);

    free_program(&program);
    free_text(&text);

    printf("Assembler!\n");

    return 0;
}


int translate(Program *program, Text *text, FILE *listing) {
    if (!listing) {
        printf("No listing file provided!\n");
        return 1;
    }

    fprintf(listing, "IP   COMMAND  ARG 1 ARG 2 NAME\n");

    program -> ip = 0;

    for(int i = 0; (text -> lines)[i].str != nullptr && (text -> lines)[i].len != -1; i++) {
        /*
        const char *comment  = strchr((text -> lines)[i].str, '#');
        if (comment)
            (text -> lines)[i].str[comment - (text -> lines)[i].str] = '\0';
        */

        if (*(text -> lines)[i].str == '\0') continue; // Empty line

        int n = 0;
        char cmd[20] = "";
        sscanf((text -> lines)[i].str, "%s%n", cmd, &n);

        if (strchr((text -> lines)[i].str, ':')) {
            if (strcmp(cmd, "jmp") == 0) {
                SET_OPERATION_AND_ARG(CMD_JMP, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else if (strcmp(cmd, "jb") == 0) {
                SET_OPERATION_AND_ARG(CMD_JB, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else if (strcmp(cmd, "ja") == 0) {
                SET_OPERATION_AND_ARG(CMD_JA, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else if (strcmp(cmd, "je") == 0) {
                SET_OPERATION_AND_ARG(CMD_JE, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else if (strcmp(cmd, "jne") == 0) {
                SET_OPERATION_AND_ARG(CMD_JNE, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else if (strcmp(cmd, "call") == 0) {
                SET_OPERATION_AND_ARG(CMD_CALL, get_label_value(program, (text -> lines)[i].str + n + 1));
            }
            else {
                Label new_label = {program -> ip, (text -> lines)[i].str};
                insert_label(program, &new_label);
            }
            
            continue;
        }

        if (strcmp(cmd, "hlt") == 0) {
            (program -> code)[program -> ip++] = CMD_HLT;
            break;
        }
        else if (strcmp(cmd, "push") == 0) {
            char *arg = (text -> lines)[i].str + n + 1;
            int value = atoi(arg), cmd_code = CMD_PUSH;

            if (strchr(arg, '[')) {
                if (!strchr(arg, ']')) {
                    printf("No closing bracket after push in line %i!\n", i + 1);
                    return 1;
                }

                arg = strchr(arg, '[') + 1;
                sscanf(arg, "%i", &value);
                cmd_code |= BIT_MEM;
            }

            if (strchr(arg, '+')) {
                sscanf(arg, "%i%n", &value, &n);

                SET_OPERATION_AND_ARGS(cmd_code | (BIT_CONST | BIT_REG), value, (arg + n + 1)[1] - 'A' + 1);
            }
            else if (value != 0) {
                SET_OPERATION_AND_ARG(cmd_code | BIT_CONST, value);
            }
            else if  (strlen(arg) == 3 && arg[0] == 'R'&& arg[2] == 'X') {
                SET_OPERATION_AND_ARG(cmd_code | BIT_REG, arg[1] - 'A' + 1);
            }
            else {
                printf("Wrong argument to push %s in line %i!\n", arg, i + 1);
                return 1;
            }
        }
        else if (strcmp(cmd, "pop") == 0) {
            char *arg = (text -> lines)[i].str + n + 1;
            int value = atoi(arg), cmd_code = CMD_POP;

            if (strchr(arg, '[')) {
                if (!strchr(arg, ']')) {
                    printf("No closing bracket after pop in line %i!\n", i + 1);
                    return 1;
                }

                arg = strchr(arg, '[') + 1;
                sscanf(arg, "%i", &value);
                cmd_code |= BIT_MEM;
            }

            if (strchr(arg, '+')) {
                sscanf(arg, "%i%n", &value, &n);

                SET_OPERATION_AND_ARGS(cmd_code | (BIT_CONST | BIT_REG), value, (arg + n + 1)[1] - 'A' + 1);
            }
            else if (value != 0) {
                SET_OPERATION_AND_ARG(cmd_code | BIT_CONST, value);
            }
            else if  (strlen(arg) == 3 && arg[0] == 'R'&& arg[2] == 'X') {
                SET_OPERATION_AND_ARG(cmd_code | BIT_REG, arg[1] - 'A' + 1);
            }
            else {
                printf("Wrong argument to pop %s in line %i!\n", arg, i + 1);
                return 1;
            }
        }
        else if (strcmp(cmd, "out") == 0) {
            SET_OPERATION(CMD_OUT);
        }
        else if (strcmp(cmd, "jb") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to jb at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_JB, value);
        }
        else if (strcmp(cmd, "ja") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to ja at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_JA, value);
        }
        else if (strcmp(cmd, "je") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to je at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_JE, value);
        }
        else if (strcmp(cmd, "jne") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to jne at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_JNE, value);
        }
        else if (strcmp(cmd, "add") == 0) {
            SET_OPERATION(CMD_ADD);
        }
        else if (strcmp(cmd, "sub") == 0) {
            SET_OPERATION(CMD_SUB);
        }
        else if (strcmp(cmd, "mul") == 0) {
            SET_OPERATION(CMD_MUL);
        }
        else if (strcmp(cmd, "div") == 0) {
            SET_OPERATION(CMD_DIV);
        }
        else if (strcmp(cmd, "ret") == 0) {
            SET_OPERATION(CMD_RET);
        }
        else if (strcmp(cmd, "call") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to jmp at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_CALL, value);
        }
        else if (strcmp(cmd, "jmp") == 0) {
            int value = 0;
            if (sscanf((text -> lines)[i].str + n, "%i", &value) == 0){
                printf("Wrong argument to jmp at line %i!\n", i + 1);
                return 1;
            }
            SET_OPERATION_AND_ARG(CMD_JMP, value);
        }
        else if (strcmp(cmd, "dup") == 0) {
            SET_OPERATION(CMD_DUP);
        }
        else {
            printf("Unknown command %s in line %i\n", cmd, i);
            return 1;
        }
    }

    program -> count = program -> ip;

    program -> code = (int *) realloc(program -> code, program -> count * sizeof(int));
    
    if (program -> labels_count)
        program -> labels = (Label *) realloc(program -> labels, program -> labels_count * sizeof(Label));

    if (!program -> code || !program -> labels) {
        printf("Failed to reallocate memory!\n");
        return 1;
    }

    return 0;
}


int write_file(int file, Program *program) {
    if (file == -1) {
        printf("Invalid file!\n");
        return 1;
    }

    if (!program) {
        printf("Can't work with then null pointer!\n");
        return 1;
    }

    int b = write(file, &(program -> count), sizeof(int));

    b += write(file, program -> code, (unsigned int) program -> count * sizeof(int));

    if (b != (program -> count + 1) * (int) sizeof(int)) {
        printf("Expected bytes %i, actualy written %i", b, (program -> count + 1) * (int) sizeof(int));
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


int is_equal(String *str1, String *str2) {
    if (str1 -> len != str2 -> len) return 0;
    for(int i = 0; i < str1 -> len; i++)
        if (str1 -> str[i] != str2 -> str[i]) return 0;
    
    return 1;
}


String get_token(char *origin, const char *solo) {
    String token = {origin, 0};

    while (isspace(*token.str)) token.str++;

    if (*token.str == '\0') return {nullptr, -1};

    if (strchr(solo, *token.str))
        token.len = 1;

    else if (isalpha(*token.str))
        while (isalnum(*(token.str + token.len))) token.len++;

    else if (isdigit(*token.str))
        while (isdigit(*(token.str + token.len))) token.len++;

    else return {token.str, 1};

    return token;
}


void print_string(String *str) {
    for(int i = 0; i < str -> len; i++)
        putchar(str -> str[i]);
    putchar('\n');
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
    program -> labels_count = 0;

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
