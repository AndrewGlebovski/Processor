DEF_CMD(HLT, 0, 0, 
    return 0;
)

DEF_CMD(PUSH, 1, set_push_args(listing, process, process -> code, &process -> ip, &cmd), 
    if (cmd & BIT_CONST) arg = process -> code[(*ip)++];
    if (cmd & BIT_REG) arg += reg[process -> code[(*ip)++] - 1]; // FORGOT TO DECREMENT ARGUMENT
    if (cmd & BIT_MEM) {
        ASSERT_IP(arg > -1 && arg / 1000 < (int) RAM_SIZE, "Segmentation fault! Wrong RAM index!", *ip - 1);

        arg = ram[arg / PRECISION];
    }

    PUSH_(arg);
)

DEF_CMD(OUT, 0, 0,
    OUT_();
)

DEF_CMD(ADD, 0, 0,
    POP_(val1);
    POP_(val2);
    PUSH_(val2 + val1);
)

DEF_CMD(SUB, 0, 0,
    POP_(val1);
    POP_(val2);
    PUSH_(val2 - val1);
)

DEF_CMD(MUL, 0, 0,
    POP_(val1);
    POP_(val2);
    PUSH_((int)((long long)val2 * (long long)val1 / (long long)PRECISION));
)

DEF_CMD(DIV, 0, 0,
    POP_(val1);
    POP_(val2);

    ASSERT_IP(val1, "Zero division!", *ip - 1);

    PUSH_((int)((float)val2 / (float)val1 * PRECISION));
)

DEF_CMD(JMP, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    JMP_();
)

DEF_CMD(DUP, 0, 0,
    POP_(value);
    PUSH_(value);
    PUSH_(value);
)

DEF_CMD(POP, 1, set_push_args(listing, process, process -> code, &process -> ip, &cmd), 
    if (execute_pop(process, ip, cmd, arg))
        return 1;
)

DEF_CMD(JB, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 < val1);
)

DEF_CMD(JA, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 > val1);
)

DEF_CMD(JE, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 == val1);
)

DEF_CMD(JNE, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 != val1);
)


DEF_CMD(JAE, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 >= val1);
)


DEF_CMD(JBE, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    POP_(val1);
    POP_(val2);

    JMP_IF_(val2 <= val1);
)


DEF_CMD(CALL, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    CALL_();
)

DEF_CMD(RET, 0, 0,
    RET_();
)

DEF_CMD(SQRT, 0, 0,
    POP_(val);

    ASSERT_IP(val >= 0, "Negative number under root!", *ip - 1);

    PUSH_((int) (sqrt((float)val / PRECISION) * PRECISION));
)

DEF_CMD(IN, 0, 0,
    float value = 0;

    ASSERT(scanf("%f", &value), "Wrong argument given!");

    PUSH_((int)(value * PRECISION));
)

DEF_CMD(SHOW, 0, 0,
    if (show_ram(process))
        return 1;
)


DEF_CMD(CLR, 0, 0,
    system("CLS");
    //printf("\e[H\e[2J\e[3J");
)
