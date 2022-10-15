#define DEF_CMD(name, num, ...) \
    CMD_##name = num,

/// List of commands
typedef enum {
    #include "cmd.hpp"
} COMMANDS;

#undef DEF_CMD

/// Argument type bit
typedef enum {
    BIT_CONST = 0x1000000, ///< Constant bit
    BIT_REG   = 0x2000000, ///< Register bit
    BIT_MEM   = 0x4000000, ///< Memory bit
} ARG_TYPE;
