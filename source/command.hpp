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
    CMD_DUP  = 8, ///< Duplicates last number
    CMD_POP  = 9, ///< Pop into register or memory
} COMMANDS;


/// Argument type bit
typedef enum {
    BIT_CONST = 0x1000000, ///< Constant bit
    BIT_REG   = 0x2000000, ///< Register bit
    BIT_MEM   = 0x4000000, ///< Memory bit
} ARG_TYPE;