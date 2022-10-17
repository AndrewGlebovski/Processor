
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "text.hpp"

#include "command.hpp"


const char *REGISTERS[] = {
    "RAX",
    "RBX",
    "RCX",
    "RDX"
};


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
    Label *labels = nullptr; ///< Process labels
    int labels_count = 0; ///< Labels count
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




int main() {
    int input = open("debug/source.txt", O_RDONLY);

    if (input == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    Text text = {};

    read_file(input, &text);

    close(input);

    Process process = {};
    
    process.code = (int *) calloc(text.size * 3, sizeof(int));
    process.labels = (Label *) calloc(text.size, sizeof(Label));

    FILE *listing = fopen("debug/listing.txt", "w");

    fprintf(listing, "First pass\n");
    if (translate(&process, &text, listing))
        return 1;

    fprintf(listing, "\nSecond pass\n");
    if (translate(&process, &text, listing))
        return 1;

    fprintf(listing, "\nProcess\n");
    print_process(&process, listing);

    fclose(listing);

    int output = open("debug/binary.txt", O_WRONLY | O_CREAT | O_BINARY);

    if (output == -1) {
        printf("Couldn't open file!\n");
        return 1;
    }

    write_file(output, &process);

    close(output);

    free_process(&process);
    free_text(&text);

    printf("Assembler!\n");

    return 0;
}


#define DEF_CMD(name, num, arg, action) \
    if (!strnicmp(cmd.str, #name, cmd.len)) { \
        process -> code[process -> ip++] = num; \
        if (arg) { \
            if (action) { \
                printf("Wrong argument in line %i!\n", i + 1); \
                return 1; \
            } \
        } \
        else { \
            fprintf(listing, "%.4i %.8X             %s\n", process -> ip - 1, num, text -> lines[i].str); \
        } \
        continue; \
    } \
    else


int translate(Process *process, Text *text, FILE *listing) {
    if (!listing) {
        printf("No listing file provided!\n");
        return 1;
    }

    fprintf(listing, "IP   COMMAND  ARG 1 ARG 2 NAME\n");

    process -> ip = 0;

    for(int i = 0; text -> lines[i].str != nullptr && text -> lines[i].len != -1; i++) {
        String cmd = get_token(text -> lines[i].str, "[+]:", "#");

        if (!cmd.str) continue;

        #include "cmd.hpp"
        /*else*/ if (set_label_value(process, &cmd)) {
            printf("Unknown command in line %i!\n", i + 1);
            return 1;
        }
    }

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

    int b = write(file, &(process -> count), sizeof(int));

    b += write(file, process -> code, (unsigned int) process -> count * sizeof(int));

    if (b != (process -> count + 1) * (int) sizeof(int)) {
        printf("Expected bytes %i, actualy written %i", b, (process -> count + 1) * (int) sizeof(int));
        return 1;
    }

    return 0;
}


int get_label_value(Process *process, String *label) {
    for(int i = 0; i < process -> labels_count; i++) {
        if (process -> labels[i].name.len != label -> len)
            continue;

        if (!strnicmp(process -> labels[i].name.str, label -> str, label -> len))
            return process -> labels[i].value;
    }

    return -1;
}


int str_to_int(String *str, int *value) {
    char c = *(str -> str + str -> len);

    float arg = 0;

    int result = sscanf(str -> str, "%f%n", &arg, value);

    if (*value != str -> len) return 0;

    *value = (int)(arg * 1000);

    *(str -> str + str -> len) = c;

    return result;
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
                process -> labels[process -> labels_count++] = {process -> ip, *cmd};

            return 0;
        }

        else if (!strnicmp(arg.str, "=", arg.len)) {
            arg = get_token(arg.str + arg.len, "[+]:", "#");

            if (arg.str) {
                int value = 0;

                if (str_to_int(&arg, &value)) {
                    if (get_label_value(process, cmd) == -1)
                        process -> labels[process -> labels_count++] = {value, *cmd};
                            
                    return 0;
                }
            }
        }
    }

    return 1;
}
