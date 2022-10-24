#define ASSERT(cond, msg)           \
    do {                            \
        if (!(cond)) {              \
            printf("%s\n", msg);    \
            return 1;               \
        }                           \
    } while (0)


#define ASSERT_IP(cond, msg, ip)    \
    do {                            \
        if (!(cond)) {              \
            printf("IP %llu\n", ip);\
            printf("%s\n", msg);    \
            return 1;               \
        }                           \
    } while (0)
