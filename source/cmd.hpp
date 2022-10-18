DEF_CMD(HLT, 0, 0, 0, 
    stack_destructor(stack);
    stack_destructor(call_stack);
    return 0;
)

DEF_CMD(PUSH, 1, 1, set_push_args(listing, process, process -> code, &process -> ip, &cmd), 
    if (cmd & BIT_CONST) arg = (process -> code)[(*ip)++];
    if (cmd & BIT_REG) arg += reg[(process -> code)[(*ip)++] - 1]; // FORGOT TO DECREMENT ARGUMENT
    if (cmd & BIT_MEM) arg = ram[arg / PRECISION];

    STACK_PUSH(stack, arg, *ip);
)

DEF_CMD(OUT, 2, 0, 0,
    int value = 0;
    STACK_POP(stack, &value, *ip);
    printf("%.3f\n", (float) value / PRECISION);
)

DEF_CMD(ADD, 3, 0, 0,
    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);
    STACK_PUSH(stack, val2 + val1, *ip);
)

DEF_CMD(SUB, 4, 0, 0,
    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);
    STACK_PUSH(stack, val2 - val1, *ip);
)

DEF_CMD(MUL, 5, 0, 0,
    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);
    STACK_PUSH(stack, (int)((long long)val2 * (long long)val1 / (long long)PRECISION), *ip);
)

DEF_CMD(DIV, 6, 0, 0,
    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);

    if (val1 == 0) {
        printf("Zero division in operation %i!\n", *ip);
        return 1;
    }

    STACK_PUSH(stack, (int)((float)val2 / (float)val1 * PRECISION), *ip);
)

DEF_CMD(JMP, 7, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return 1;
    }

    *ip = arg;
)

DEF_CMD(DUP, 8, 0, 0,
    int value = 0;
    STACK_POP(stack, &value, *ip);
    STACK_PUSH(stack, value, *ip);
    STACK_PUSH(stack, value, *ip);
)

DEF_CMD(POP, 9, 1, set_push_args(listing, process, process -> code, &process -> ip, &cmd),
    if (cmd & BIT_MEM) {
        if (cmd & BIT_CONST) arg = (process -> code)[(*ip)++];
        if (cmd & BIT_REG) arg += reg[(process -> code)[(*ip)++] - 1]; // ADD REGISTER INDEX CHECK

        arg /= PRECISION;

        if (arg < 0 || arg > (int) (sizeof(process -> ram) / sizeof(*process -> ram)) - 1) {
            printf("Segmentation fault! Wrong RAM index in operation %i!\n", *ip);
            return 1;
        }

        STACK_POP(stack, &ram[arg], *ip);
    }
    else if (cmd & BIT_CONST) {
        int value = 0;
        STACK_POP(stack, &value, *ip);
    }
    else if (cmd & BIT_REG) {
        arg = (process -> code)[(*ip)++];

        if (arg < 1 || arg > (int) (sizeof(process -> reg) / sizeof(*process -> reg))) {
            printf("Segmentation fault! Wrong register index in operation %i!\n", *ip);
            return 1;
        }

        STACK_POP(stack, &reg[arg - 1], *ip);
    }
)

DEF_CMD(JB, 10, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);

    if (!(val2 < val1))
        break;

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return 1;
    }

    *ip = arg;
)

DEF_CMD(JA, 11, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);

    if (!(val2 > val1))
        break;

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return 1;
    }

    *ip = arg;
)

DEF_CMD(JE, 12, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);

    if (!(val2 == val1))
        break;

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return 1;
    }

    *ip = arg;
)

DEF_CMD(JNE, 13, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    int val1 = 0, val2 = 0;
    STACK_POP(stack, &val1, *ip);
    STACK_POP(stack, &val2, *ip);

    if (!(val2 != val1))
        break;

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return 1;
    }

    *ip = arg;
)

DEF_CMD(CALL, 14, 1, set_jmp_args(listing, process, process -> code, &process -> ip, &cmd),
    arg = (process -> code)[(*ip)++];

    if (arg == -1) {
        printf("Jump to -1 in operation %i!\n", *ip);
        return -1;
    }

    STACK_PUSH(call_stack, *ip, *ip);

    *ip = arg;
)

DEF_CMD(RET, 15, 0, 0,
    STACK_POP(call_stack, &(*ip), *ip);
)

DEF_CMD(SQRT, 16, 0, 0,
    int val = 0;
    STACK_POP(stack, &val, *ip);

    if (val < 0) {
        printf("Negative number under root %i!\n", *ip);
        return 1;
    }

    STACK_PUSH(stack, (int) (sqrt((float)val / PRECISION) * PRECISION), *ip);
)

DEF_CMD(IN, 17, 0, 0,
    float value = 0;

    if (!scanf("%f", &value)) {
        printf("Wrong argument given!\n");
        return 1;
    }

    STACK_PUSH(stack, (int)(value * PRECISION), process -> ip);
)