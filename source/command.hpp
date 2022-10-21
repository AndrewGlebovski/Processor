#define DEF_CMD(name, ...) \
    CMD_##name,

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


/// Real number precision
const int PRECISION = 1000;


/// Signature
const char *sign = "AT-AT";


/// Version
const int version = 1;
