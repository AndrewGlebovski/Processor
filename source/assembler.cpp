#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "text.hpp"
#include "parser.hpp"
#include "asm_func_list.hpp"
#include "command.hpp"


const char *REGISTERS[] = {
    "RAX",
    "RBX",
    "RCX",
    "RDX"
};


/// Hash type integer
typedef unsigned long long hash_t;


#include "hash.hpp"


/// Contains information about label
typedef struct {
    int value = 0;
    String name = {};
    hash_t hash = 0;
} Label;


/// Contains information about byte code to execute
typedef struct {
    int *code = nullptr;        ///< Operation code 
    int count = 0;              ///< Operation count
    int ip = 0;                 ///< Current operation
    Label *labels = nullptr;    ///< Process labels
    int labels_count = 0;       ///< Labels count
} Process;


/**
 * \brief Translate text from file to actual code
 * \param [out] process This struct will be filled with information
 * \param [in]  text Text to analyze
 * \param [in]  listing File for listing
 * \return Non zero value means error
 * \note Don't forget to allocate code and labels before and free code and labels after using translate
*/
int translate(Process *process, Text *text, FILE *listing);


/**
 * \brief Write binary output to file
 * \param [out] file Output file
 * \param [in]  process Process to write
 * \return Non zero value means error
*/
int write_file(int file, Process *process);


/**
 * \brief Gets label value
 * \param [in] process Process to search label in
 * \param [in] label_name Label with this name will be searched
 * \return Actual value or -1 if label not found
*/
int get_label_value(Process *process, String *label);


/**
 * \brief Gets register index
 * \param [in] name Register name
 * \return Actual index or -1 if register not found
*/
int get_register_index(String *name);


/**
 * \brief Allocates process memory
 * \param [in] process This process memory will be allocate
 * \note Free process memory to prevent memory leaks
 * \return Non zero value means error
*/
int alloc_process(Process *process, Text *text);


/**
 * \brief Reallocates process memory
 * \param [in] process This process will be reallocate
 * \note You should allocate process memory before call
 * \return Non zero value means error
*/
int realloc_process(Process *process);


/**
 * \brief Free process memory
 * \param [in] process This process will be reinitialize
 * \return Non zero value means error
*/
int free_process(Process *process);


/**
 * \brief Prints all information about process
 * \param [in]  process Process to print
 * \param [out] stream Output file
*/
void print_process(Process *process, FILE *stream);


/**
 * \brief Sets push arguments
 * \param [out] listing File for listing
 * \param [out] code Arguments will be written to this code
 * \param [out] ip This instruction pointer will be moved
 * \param [in]  cmd Current command string
 * \return Non zero value means error
*/
int set_push_args(FILE *listing, Process *process, int *code, int *ip, String *cmd);


/**
 * \brief Sets jmp arguments
 * \param [out] listing File for listing
 * \param [in] process For label search
 * \param [out] code Arguments will be written to this code
 * \param [out] ip This instruction pointer will be moved
 * \param [in]  cmd Current command string
 * \return Non zero value means error
*/
int set_jmp_args(FILE *listing, Process *process, int *code, int *ip, String *cmd);


/**
 * \brief Inserts new label
 * \param [in] process For label search
 * \param [in] cmd Current command string
 * \return Non zero value means error
*/
int set_label_value(Process *process, String *cmd);


/**
 * \brief Gets object hash sum
 * \param [in] ptr  Pointer to object
 * \param [in] size Object size in bytes
 * \warning No difference in hash sum between lower and upper case
 * \return Object hash sum or zero if wrong args given
*/
hash_t gnu_hash(const void *ptr, size_t size);


/**
 * \brief Generates or updates hash.hpp
 * \note Please, launch this function only if you want to update hash.hpp
*/
void generate_hash_file();




int main(int argc, char *argv[]) {
#ifdef UPDATE_HASH
    generate_hash_file();
#else
    int input = -1, output = -1;

    #include "asm_cmd_list.hpp"

    if (parse_args(argc, argv, command_list, sizeof(command_list) / sizeof(Command)))
        return 1;

    if (input == -1 || output == -1)
        return 1;

    Text text = {};

    read_file(input, &text);

    close(input);

    Process process = {};
    
    if (alloc_process(&process, &text))
        return 1;

    FILE *listing = fopen("listing.txt", "w");

    fprintf(listing, "First pass\n");
    if (!translate(&process, &text, listing)) {
        if (realloc_process(&process))
            return 1;

        fprintf(listing, "\nSecond pass\n");
        translate(&process, &text, listing);

        write_file(output, &process);
    }

    fprintf(listing, "\nProcess\n");
    print_process(&process, listing);

    fclose(listing);

    close(output);

    free_process(&process);
    free_text(&text);

    printf("Assembler!\n");

    return 0;
#endif
}


#define DEF_CMD(name, arg, action, ...) \
    case (CMD_##name##_HASH): { \
        process -> code[process -> ip++] = CMD_##name; \
        if (arg) { \
            if (action) { \
                printf("Wrong argument in line %i!\n", i + 1); \
                return 1; \
            } \
        } \
        else { \
            fprintf(listing, "%.4i %.8X             %s\n", process -> ip - 1, CMD_##name, text -> lines[i].str); \
        } \
        break; \
    }


int translate(Process *process, Text *text, FILE *listing) {
    if (!listing) {
        printf("No listing file provided!\n");
        return 1;
    }

    fprintf(listing, "IP   COMMAND  ARG 1 ARG 2 NAME\n");

    process -> ip = 0;
#ifndef UPDATE_HASH
    for(int i = 0; text -> lines[i].str != nullptr && text -> lines[i].len != -1; i++) {
        String cmd = get_token(text -> lines[i].str, "[+]:", "#");

        if (!cmd.str) continue;

        hash_t cmd_hash = gnu_hash(cmd.str, cmd.len);

        switch(cmd_hash) {
            #include "cmd.hpp"
            default:
                if (set_label_value(process, &cmd)) {
                    printf("Unknown command in line %i!\n", i + 1);
                    return 1;
                }
        }
    }
#endif
    process -> count = process -> ip;

    return 0;
}


#undef DEF_CMD


int write_file(int file, Process *process) {
    if (file == -1) {
        printf("Invalid file!\n");
        return 1;
    }

    if (!process) {
        printf("Can't work with then null pointer!\n");
        return 1;
    }

    int bytes = write(file, SIGN, sizeof(SIGN));

    bytes += write(file, &VERSION, sizeof(int));
    
    bytes += write(file, &(process -> count), sizeof(int));

    bytes += write(file, process -> code, (unsigned int) process -> count * sizeof(int));

    if (bytes != (int) sizeof(SIGN) + (process -> count + 2) * (int) sizeof(int)) {
        printf("Expected bytes %i, actualy written %i", bytes, (process -> count + 1) * (int) sizeof(int));
        return 1;
    }

    return 0;
}


int get_label_value(Process *process, String *label) {
    hash_t label_hash = gnu_hash(label -> str, label -> len);

    for(int i = 0; i < process -> labels_count; i++) {
        if (process -> labels[i].hash == label_hash)
            return process -> labels[i].value;
    }

    return -1;
}


int str_to_int(String *str, int *value) {
    char *end = nullptr;
    *value = (int)(strtod(str -> str, &end) * PRECISION);
    return (end == str -> str + str -> len);
}


String get_token(char *origin, const char *solo, const char *extra) {
    String token = {origin, 1};

    while (isspace(*token.str)) token.str++;

    if (*token.str == '\0') return {nullptr, -1};

    if (strchr(solo, *token.str)) return token;

    else if (isalpha(*token.str))
        while (isalnum(*(token.str + token.len)) || *(token.str + token.len) == '_') token.len++;

    else if (isdigit(*token.str) || *token.str == '-')
        while (isdigit(*(token.str + token.len)) || *(token.str + token.len) == '.') token.len++;

    if (strchr(extra, *token.str)) return {nullptr, -1};

    return token;
}


int get_register_index(String *name) {
    for(size_t i = 0; i < sizeof(REGISTERS) / sizeof(*REGISTERS); i++) {
        if (!strnicmp(name -> str, REGISTERS[i], name -> len)) return (int) i + 1;
    }

    return -1;
}


int alloc_process(Process *process, Text *text) {
    process -> code = (int *) calloc(text -> size * 3, sizeof(int));

    if (!process -> code) {
        printf("Can't allocate memory for code!\n");
        return 1;
    }

    process -> labels = (Label *) calloc(text -> size, sizeof(Label));

    if (!process -> code) {
        printf("Can't allocate memory for labels!\n");
        return 1;
    }

    return 0;
}


int realloc_process(Process *process) {
    process -> code = (int *) realloc(process -> code, process -> count * sizeof(int));

    if (!process -> code) {
        printf("Can't reallocate memory for code!\n");
        return 1;
    }

    process -> labels = (Label *) realloc(process -> labels, process -> labels_count * sizeof(Label));

    if (!process -> code) {
        printf("Can't reallocate memory for labels!\n");
        return 1;
    }

    return 0;
}


int free_process(Process *process) {
    if (!process -> code || !process -> labels) {
        printf("Failed to free memory due to null pointer!\n");
        return 1;
    }

    free(process -> code);
    process -> code = nullptr;

    free(process -> labels);
    process -> labels = nullptr;

    process -> count = 0;
    process -> ip = 0;
    process -> labels_count = 0;

    return  0;
}


void print_process(Process *process, FILE *stream) {
    fprintf(stream, "Operation count: %i\n", process -> count);

    for(int i = 0; i < process -> count; i++)
        fprintf(stream, "%i ", process -> code[i]);

    fprintf(stream, "\nLabels count: %i\n", process -> labels_count);

    for(int i = 0; i < process -> labels_count; i++)
        fprintf(stream, "%.*s %i\n", process -> labels[i].name.len, process -> labels[i].name.str, process -> labels[i].value);
}


int set_push_args(FILE *listing, Process *process, int *code, int *ip, String *cmd) {
    String arg = get_token(cmd -> str + cmd -> len, "[+]:", "#");
    int *flag = code + *ip - 1, value = 0;

    if (!arg.str) return 1;

    if (!strnicmp(arg.str, "[", arg.len)) {
        *flag |= BIT_MEM;

        arg = get_token(arg.str + arg.len, "[+]:", "#");

        if (!arg.str) return 1;
    }

    if (str_to_int(&arg, &value) || (value = get_label_value(process, &arg)) != -1) {
        *flag |= BIT_CONST;
        code[(*ip)++] = value;

        arg = get_token(arg.str + arg.len, "[+]:", "#");

        if (!arg.str) {
            fprintf(listing, "%.4i %.8X %.4i        %s\n", *ip - 2, *flag, code[*ip - 1], cmd -> str);

            return (*flag & BIT_MEM);
        }
    }

    if (!strnicmp(arg.str, "+", arg.len)) {
        arg = get_token(arg.str + arg.len, "[+]:", "#");

        if (!arg.str || !strnicmp(arg.str, "]", arg.len)) return 1;
    }

    if ((value = get_register_index(&arg)) != -1) {
        *flag |= BIT_REG;
        code[(*ip)++] = value;

        arg = get_token(arg.str + arg.len, "[+]:", "#");

        if (!arg.str) {
            if (*flag & BIT_CONST)
                fprintf(listing, "%.4i %.8X %.4i  %.4i  %s\n", *ip - 3, *flag, code[*ip - 2], code[*ip - 1], cmd -> str);
            else
                fprintf(listing, "%.4i %.8X %.4i        %s\n", *ip - 2, *flag, code[*ip - 1], cmd -> str);

            return (*flag & BIT_MEM);
        }
    }

    if (!strnicmp(arg.str, "]", arg.len) && (*flag & BIT_MEM)) {
        if ((*flag & BIT_CONST) && (*flag & BIT_REG)) 
            fprintf(listing, "%.4i %.8X %.4i  %.4i  %s\n", *ip - 3, *flag, code[*ip - 2], code[*ip - 1], cmd -> str);
        
        else if ((*flag & BIT_CONST) || (*flag & BIT_REG))
            fprintf(listing, "%.4i %.8X %.4i        %s\n", *ip - 2, *flag, code[*ip - 1], cmd -> str);
        
        else
            return 1;

        return 0;
    }

    return 1;
}


int set_jmp_args(FILE *listing, Process *process, int *code, int *ip, String *cmd) {
    String arg = get_token(cmd -> str + cmd -> len, "[+]:", "#");

    if (!arg.str) return 1;

    int value = 0;

    if (str_to_int(&arg, &value))
        code[(*ip)++] = value;
    else
        code[(*ip)++] = get_label_value(process, &arg);

    fprintf(listing, "%.4i %.8X %.4i        %s\n", *ip - 2, code[*ip - 2], code[*ip - 1], cmd -> str);

    return 0;
}


int set_label_value(Process *process, String *cmd) {
    String arg = get_token(cmd -> str + cmd -> len, "[+]:", "#");

    if (arg.str) {
        if (!strnicmp(arg.str, ":", arg.len)) {

            if (get_label_value(process, cmd) == -1)
                process -> labels[process -> labels_count++] = {process -> ip, *cmd, gnu_hash(cmd -> str, cmd -> len)};

            return 0;
        }

        else if (!strnicmp(arg.str, "=", arg.len)) {
            arg = get_token(arg.str + arg.len, "[+]:", "#");

            if (arg.str) {
                int value = 0;

                if (str_to_int(&arg, &value)) {
                    if (get_label_value(process, cmd) == -1)
                        process -> labels[process -> labels_count++] = {value, *cmd, gnu_hash(cmd -> str, cmd -> len)};
                            
                    return 0;
                }
            }
        }
    }

    return 1;
}


#define DEF_CMD(name, ...) \
    fprintf(hash_file, "    CMD_"#name"_HASH = %llu,\n", gnu_hash(#name, sizeof(#name) - 1)); 


void generate_hash_file() {
    FILE *hash_file = fopen("source/hash.hpp", "w");

    fprintf(hash_file, "// THIS FILE WAS CREATED AUTOMATICALLY!\n");
    fprintf(hash_file, "// PLEASE, DO NOT UPDATE IT MANUALY!\n");
    fprintf(hash_file, "// USE generate_hash_file() in assembler.cpp INSTEAD!\n\n");

    fprintf(hash_file, "typedef enum {\n");

    #include "cmd.hpp"

    fprintf(hash_file, "} COMMANDS_HASH;\n");
}


#undef DEF_CMD


hash_t gnu_hash(const void *ptr, size_t size) {
    if (!ptr || !size)
        return 0;

    hash_t hash = 5381;

    for(size_t i = 0; i < size; i++)
        hash = hash * 33 + tolower(((const char *)(ptr))[i]);

    return hash;
}
