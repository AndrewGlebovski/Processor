DEF_CMD(HLT, 0, 0, 0)

DEF_CMD(PUSH, 1, 1, set_push_args(listing, process -> code, &process -> ip, &cmd))

DEF_CMD(OUT, 2, 0, 0)

DEF_CMD(ADD, 3, 0, 0)

DEF_CMD(SUB, 4, 0, 0)

DEF_CMD(MUL, 5, 0, 0)

DEF_CMD(DIV, 6, 0, 0)

DEF_CMD(JMP, 7, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(DUP, 8, 0, 0)

DEF_CMD(POP, 9, 1, set_push_args(listing, process -> code, &process -> ip, &cmd))

DEF_CMD(JB, 10, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(JA, 11, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(JE, 12, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(JNE, 13, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(CALL, 14, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd))

DEF_CMD(RET, 15, 0, 0)