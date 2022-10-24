/**
 * \brief Pushes value to stack
*/
#define PUSH_(value)                                                            \
    ASSERT_IP(!stack_push(stack, value), "Stack push error!", OFFSET(ip - 1));  \
    do {} while(0)


/**
 * \brief Creates variable and pops value from stack into it
*/
#define POP_(var)                                                               \
    int var = 0;                                                                \
    ASSERT_IP(!stack_pop(stack, &var), "Empty stack pop!", OFFSET(ip - 1));     \
    do {} while(0)


/**
 * \brief Sets ip to its argument
*/
#define JMP_()                                                                  \
    arg = *((arg_t *)(ip));                                                     \
    ASSERT_IP(arg > -1, "Jump to -1!", OFFSET(ip - 1));                         \
    ip = process -> code + arg;                                                 \
    do {} while(0)


/**
 * \brief Jumps only if condition is true
*/
#define JMP_IF_(condition)                                                      \
    if (condition) {JMP_();}                                                    \
    else ip += sizeof(arg_t);                                                   \
    do {} while(0)


/**
 * \brief Calls jmp and remembers its position
*/
#define CALL_()                                                                 \
    stack_push(call_stack, (int)(OFFSET(ip) + sizeof(arg_t)));                  \
    JMP_();                                                                     \
    do {} while(0)


/**
 * \brief Returns to previous call position
*/
#define RET_()                                                                              \
    int offset = 0;                                                                         \
    ASSERT_IP(!stack_pop(call_stack, &offset), "Empty call stack pop!", OFFSET(ip - 1));    \
    ip = process -> code + (size_t) offset;                                                 \
    do {} while(0)


/**
 * \brief Prints last stack element
*/
#define OUT_()                                                                  \
    POP_(value);                                                                \
    printf("%g\n", (float) value / PRECISION);                                  \
    do {} while(0)
