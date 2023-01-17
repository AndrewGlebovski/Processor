#include <stdio.h>
#include <stdlib.h>
#include "stack.hpp"


/**
 * \brief Does some action in case of condition fail
 * \param [in] condition Condition to check
 * \param [in] action This code will be executed if condition fails
*/
#define CHECK(condition, action)                \
do {                                            \
    if (!(condition)) {                         \
        action;                                 \
    }                                           \
} while(0)


/**
 * \brief If stack is invalid returns an error code
 * \param [in] stack Stack to check
*/
#define RETURN_ON_ERROR(stack)                  \
do {                                            \
    int error = stack_verificator(stack);       \
    if (error) return error;                    \
} while(0)


/**
 * \brief Resizes stack
 * \param [in] stack This stack will be resized automaticaly
 * \return Non zero value means error
*/
static int stack_resize(Stack *stack);




int stack_constructor(Stack *stack, int capacity) {
    CHECK(stack, return EXIT_CODES::INVALID_ARGUMENT);
    CHECK(capacity > 0, return EXIT_CODES::INVALID_ARGUMENT);

    stack -> data = (stack_data_t *) calloc(capacity, sizeof(stack_data_t));
    CHECK(stack -> data, return EXIT_CODES::ALLOCATE_FAIL);

    for(int i = 0; i < capacity ; i++)
        (stack -> data)[i] = POISON_VALUE;
    
    stack -> capacity = capacity;
    stack -> size = 0;

    RETURN_ON_ERROR(stack);

    return 0;
}


static int stack_resize(Stack *stack) {
    RETURN_ON_ERROR(stack);

    if (4 * stack -> size < stack -> capacity)
        stack -> capacity /= 2;
    
    else if (stack -> size == stack -> capacity)
        stack -> capacity *= 2;

    else 
        return 0;

    stack -> data = (stack_data_t *) realloc(stack -> data, stack -> capacity * sizeof(stack_data_t));
    CHECK(stack -> data, return EXIT_CODES::ALLOCATE_FAIL);

    for(int i = stack -> size; i < stack -> capacity ; i++)
        (stack -> data)[i] = POISON_VALUE;

    RETURN_ON_ERROR(stack);

    return 0;
}


int stack_push(Stack *stack, stack_data_t new_data) {
    RETURN_ON_ERROR(stack);

    (stack -> data)[(stack -> size)++] = new_data;

    RETURN_ON_ERROR(stack);

    return stack_resize(stack);
}


int stack_pop(Stack *stack, stack_data_t *data) {
    CHECK(data, return EXIT_CODES::INVALID_ARGUMENT);

    RETURN_ON_ERROR(stack);

    CHECK(stack -> size, return EXIT_CODES::EMPTY_STACK);

    *data = (stack -> data)[--(stack -> size)];
    (stack -> data)[(stack -> size)] = POISON_VALUE;

    RETURN_ON_ERROR(stack);

    return stack_resize(stack);
}


int stack_destructor(Stack *stack) {
    RETURN_ON_ERROR(stack);

    free((char *)(stack -> data));

    stack -> data = nullptr;
    
    stack -> capacity = 0;
    stack -> size = 0;

    return 0;
}


int stack_verificator(Stack *stack) {
    CHECK(stack, return EXIT_CODES::INVALID_ARGUMENT);

    CHECK(stack -> data, return EXIT_CODES::INVALID_DATA);

    CHECK(stack -> capacity >= 0 && stack -> capacity <= MAX_CAPACITY_VALUE, return EXIT_CODES::INVALID_CAPACITY);

    CHECK(stack -> size >= 0 && stack -> size <= stack -> capacity, return EXIT_CODES::INVALID_SIZE);

    for(int i = 0; i < stack -> capacity; i++) {
        if (i < stack -> size)
            CHECK((stack -> data)[i] != POISON_VALUE, return EXIT_CODES::UNEXP_POISON_VAL);
        else
            CHECK((stack -> data)[i] == POISON_VALUE, return EXIT_CODES::UNEXP_NORMAL_VAL);
    }
    
    return 0;
}


void stack_dump(Stack *stack, int error, FILE *stream) {
    CHECK(stack, return);
    
    fprintf(stream, "Stack[%p]:\n", stack);

    fprintf(stream, "\%2s%-8s: %i\n", "", "Capacity", stack -> capacity);
    fprintf(stream, "\%2s%-8s: %i\n", "", "Size", stack -> size);

    fprintf(stream, "\%2sData[%p]", "", stack -> data);
    
    if (error == EXIT_CODES::INVALID_DATA || error == EXIT_CODES::INVALID_CAPACITY) {
        fputc('\n', stream);
        return;
    }
    
    fprintf(stream, ":\n");

    for(int i = 0; i < stack -> capacity; i++) {
        fprintf(stream, "%4s[%03i] ", "", i); // stack_data_t index

        fprintf(stream, OBJECT_TO_STR, (stack -> data)[i]); // print value function (possible macros)

        if ((stack -> data)[i] == POISON_VALUE) fprintf(stream, " (POISON VALUE)"); // poison value warning
            
        fputc('\n', stream); // new line
    }

    fputc('\n', stream);
}
