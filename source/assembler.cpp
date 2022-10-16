
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
    String name = {};
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
void insert_label(Program *program, String *new_label, int new_value);


/**
 * \brief Gets label value
 * \param [in] program Program to search label in
 * \param [in] label_name Label with this name will be searched
 * \return Actual value or -1 if label not found
*/
int get_label_value(Program *program, String *label);


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
 * \brief Compares two strings structs
 * \param [in] str1 First string to compare
 * \param [in] str2 Second string to compare
 * \return Non zero value means equality
*/
int is_equal(String *str1, const char *str2);


int str_to_int(String *str, int *value);


/**
 * \brief Prints string
 * \param [in] str String to print
*/
void print_string(String *str);


int set_push_args(FILE *listing, Program *program, int *code, int *ip, String *cmd);


int set_jmp_args(FILE *listing, Program *program, int *code, int *ip, String *cmd);




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
   /*
    for(int i = 0; i < text.size; i++) {
        String str = get_token(text.lines[i].str, "[+]:");
        while(str.len != -1) {
            print_string(&str);
            str = get_token(str.str + str.len, "[+]:");
        }
    }
    */
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

    print_program(&program);

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


#define DEF_CMD(name, num, arg, action) \
    if (is_equal(&cmd, #name)) { \
        program -> code[program -> ip++] = num; \
        if (arg) { \
            if (action) { \
                printf("Wrong argument in line %i!\n", i + 1); \
                return 1; \
            } \
        } \
        else { \
            fprintf(listing, "%.4i %.8X             %s\n", program -> ip - 1, num, text -> lines[i].str); \
        } \
        continue; \
    } \
    else


int translate(Program *program, Text *text, FILE *listing) {
    if (!listing) {
        printf("No listing file provided!\n");
        return 1;
    }

    fprintf(listing, "IP   COMMAND  ARG 1 ARG 2 NAME\n");

    program -> ip = 0;

    for(int i = 0; text -> lines[i].str != nullptr && text -> lines[i].len != -1; i++) {
        String cmd = get_token(text -> lines[i].str, "[+]:");

        if (!cmd.str) continue;

        #include "cmd.hpp"
        /*else*/ {
            String arg = get_token(cmd.str + cmd.len, "[+]:");

            if (!arg.str) return 1;

            if (is_equal(&arg, ":")) {
                insert_label(program, &cmd, program -> ip);
                continue;
            }

            printf("Unknown command in line %i!\n", i + 1);
            return 1;
        }
    }

    program -> count = program -> ip;

    return 0;
}


#undef DEF_CMD


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


void insert_label(Program *program, String *new_label, int new_value) {
    for(int i = 0; i < program -> labels_count; i++) {
        if (is_equal(&(program -> labels[i].name), new_label))
            return;
    }

    program -> labels[program -> labels_count++] = {new_value, *new_label};
}


int get_label_value(Program *program, String *label) {
    for(int i = 0; i < program -> labels_count; i++) {
        if (is_equal(&(program -> labels[i].name), label)) {
            return (program -> labels)[i].value;
        }
    }

    return -1;
}


int is_equal(String *str1, String *str2) {
    if (str1 -> len != str2 -> len) return 0;
    for(int i = 0; i < str1 -> len; i++)
        if (tolower(str1 -> str[i]) != tolower(str2 -> str[i])) 
            return 0;
    
    return 1;
}


int is_equal(String *str1, const char *str2) {
    for(int i = 0; i < str1 -> len; i++)
        if (tolower(str1 -> str[i]) != tolower(str2[i])) 
            return 0;
    
    return 1;
}


int str_to_int(String *str, int *value) {
    char c = *(str -> str + str -> len);

    int result = sscanf(str -> str, "%i", value);

    *(str -> str + str -> len) = c;

    return result;
}


String get_token(char *origin, const char *solo) {
    String token = {origin, 1};

    while (isspace(*token.str)) token.str++;

    if (*token.str == '\0') return {nullptr, -1};

    if (strchr(solo, *token.str)) return token;

    else if (isalpha(*token.str))
        while (isalnum(*(token.str + token.len))) token.len++;

    else if (isdigit(*token.str) || *token.str == '-')
        while (isdigit(*(token.str + token.len))) token.len++;

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
        print_string(&program -> labels[i].name);
}


int set_push_args(FILE *listing, Program *program, int *code, int *ip, String *cmd) {
    fprintf(listing, "%.4i %.8X             %s\n", *ip, code[*ip], cmd -> str);
    return 0;
    /*
    if (!arg.str) {
        printf("Wrong argument in line %i!\n", i);
        return 1;
    }

    if (is_equal(&arg, "[")) {
        arg = get_token(arg.str + arg.len, "[+]:");
        int flag = CMD_PUSH, arg1 = 0, arg2 = 0; 

        while (!arg.str) {
            if (is_equal(&arg, "]")) {
                break;
            }

            else if (is_equal(&arg, "+")) {

            }

            else {
                int value = get_label_value(program, &arg);

                if (value > -1) {
                    arg2 = value;
                    flag |= BIT_REG;
                }
                else {
                    arg2 = str_to_int(&arg);
                    flag |= BIT_CONST;
                }
            }

            arg = get_token(arg.str + arg.len, "[+]:");
        }
    }

    fprintf(listing, "%.4i %.8X %.4i        %s\n", program -> ip - 2, program -> code[program -> ip - 2], text -> lines[i].str);
    */
}


int set_jmp_args(FILE *listing, Program *program, int *code, int *ip, String *cmd) {
    String arg = get_token(cmd -> str + cmd -> len, "[+]:");

    if (!arg.str) return 1;

    int value = 0;

    if (str_to_int(&arg, &value))
        code[(*ip)++] = value;
    else
        code[(*ip)++] = get_label_value(program, &arg);

    fprintf(listing, "%.4i %.8X %.4i        %s\n", *ip - 2, code[*ip - 2], code[*ip - 1], cmd -> str);

    return 0;
}
