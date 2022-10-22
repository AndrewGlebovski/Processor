/**
 * \brief Pushes value to stack
*/
#define PUSH_(value)                                        \
    if (stack_push(stack, value)) {                         \
        printf("Stack push error in operation %i!\n", *ip); \
        return 1;                                           \
    }                                                       \
    do {} while(0)


/**
 * \brief Creates variable and pops value from stack into it
*/
#define POP_(var)                                           \
    int var = 0;                                            \
    if (stack_pop(stack, &var)) {                           \
        printf("Empty stack pop in operation %i!\n", *ip);  \
        return 1;                                           \
    }                                                       \
    do {} while(0)


/**
 * \brief Sets ip to its argument
*/
#define JMP_()                                              \
    arg = process -> code[*ip];                             \
    if (arg == -1) {                                        \
        printf("Jump to -1 in operation %i!\n", *ip);       \
        return 1;                                           \
    }                                                       \
    *ip = arg;                                              \
    do {} while(0)


/**
 * \brief Jumps only if condition is true
*/
#define JMP_IF_(condition)                                  \
    if (condition) {JMP_();}                                \
    else (*ip)++;                                           \
    do {} while(0)


/**
 * \brief Calls jmp and remembers its position
*/
#define CALL_()                                             \
    stack_push(call_stack, *ip + 1);                            \
    JMP_();                                                 \
    do {} while(0)


/**
 * \brief Returns to previous call position
*/
#define RET_()                                              \
    if (stack_pop(call_stack, ip)) {                        \
        printf("Empty call stack pop!\n");                  \
        return 1;                                           \
    }                                                       \
    do {} while(0)


/**
 * \brief Prints last stack element
*/
#define OUT_()                                              \
    POP_(value);                                            \
    printf("%g\n", (float) value / PRECISION);              \
    do {} while(0)
