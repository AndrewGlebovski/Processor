DEF_CMD(HLT, 0) ///< Program end
DEF_CMD(PUSH, 1) ///< Push number to stack
DEF_CMD(OUT, 2) ///< Print number
DEF_CMD(ADD, 3) ///< Add two numbers
DEF_CMD(SUB, 4) ///< Subtract two numbers
DEF_CMD(MUL, 5) ///< Multiply two numbers
DEF_CMD(DIV, 6) ///< Divide two numbers
DEF_CMD(JMP, 7) ///< Jump to the specific line of code
DEF_CMD(DUP, 8) ///< Duplicates last number
DEF_CMD(POP, 9) ///< Pop into register or memory
DEF_CMD(JB, 10) ///< Jump if below
DEF_CMD(JA, 11) ///< Jump if ahead
DEF_CMD(JE, 12) ///< Jump if equal
DEF_CMD(JNE, 13) ///< Jump if not equal
DEF_CMD(CALL, 14) ///< Function call
DEF_CMD(RET, 15) ///< Function return