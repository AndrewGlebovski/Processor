#define DEF_CMD(name, ...) \
    CMD_##name,

/// List of commands
typedef enum {
    #include "cmd.hpp"
} COMMANDS;

#undef DEF_CMD

/// Argument type bit
typedef enum {
    BIT_CONST = 0x20, ///< Constant bit
    BIT_REG   = 0x40, ///< Register bit
    BIT_MEM   = 0x80, ///< Memory bit
} ARG_TYPE;


/// Real number precision
const int PRECISION = 1000;


/// Signature
const char *SIGN = "AT-AT";


/// Version
const int VERSION = 1;


/// Command type
typedef unsigned char cmd_t;


/// Command argument type
typedef int arg_t;
