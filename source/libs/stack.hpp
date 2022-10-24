#define POISON_VALUE 0xC0FFEE
#define MAX_CAPACITY_VALUE 100000
#define OBJECT_TO_STR "%i"

#define CANARY_PROTECT 1
#define HASH_PROTECT 2


#ifndef PROTECT_LEVEL
    #define PROTECT_LEVEL (CANARY_PROTECT + HASH_PROTECT)
#endif


#if (PROTECT_LEVEL & CANARY_PROTECT)
    #define ON_CANARY_PROTECT(...) __VA_ARGS__
#else
    #define ON_CANARY_PROTECT(...) 
#endif


#if (PROTECT_LEVEL & HASH_PROTECT)
    #define ON_HASH_PROTECT(...) __VA_ARGS__
#else
    #define ON_HASH_PROTECT(...) 
#endif


typedef int Object; ///< Stack object type
typedef long long StackSize; ///< Type for stack size and capacity
typedef unsigned long long ErrorBits; ///< Type for holding error codes
typedef unsigned long long CanaryType; ///< Type for holding canary value
typedef unsigned long long HashType; ///< Type for holding hash sum


/// Structure for holding stack
typedef struct {
    ON_CANARY_PROTECT(CanaryType canary_begin = 0;)

    Object *data = NULL;
    StackSize size = 0;
    StackSize capacity = 0;

    ON_HASH_PROTECT(HashType struct_hash = 0;)
    ON_HASH_PROTECT(HashType buffer_hash = 0;)

    ON_CANARY_PROTECT(CanaryType canary_end = 0;)
} Stack;


/// Error bit codes
enum ERROR_BIT_FLAGS {
    STACK_OK         =     0ull, ///< Stack is ok
    NULL_DATA        =     1ull, ///< Stack data points to NULL
    INVALID_SIZE     =  1ull<<1, ///< Size is larger than capacity or negative
    INVALID_CAPACITY =  1ull<<2, ///< Capacity is larger than type's maximum or negative
    UNEXP_POISON_VAL =  1ull<<3, ///< Unexpected poison value before size
    UNEXP_NORMAL_VAL =  1ull<<4, ///< Unexpected normal value after size
    INVALID_ARGUMENT =  1ull<<5, ///< Invalid argument given to the function
    EMPTY_STACK      =  1ull<<6, ///< No elements to pop
    ALLOCATE_FAIL    =  1ull<<7, ///< Memory allocate return NULL
    STRUCT_CANARY    =  1ull<<8, ///< Stack canary value was overwritten
    BUFFER_CANARY    =  1ull<<9, ///< Buffer canary value was overwritten
    BUFFER_HASH_FAIL = 1ull<<10, ///< Wrong buffer hash sum
    STRUCT_HASH_FAIL = 1ull<<11, ///< Wrong stack hash sum
};


/**
 * \brief Constructs the stack
 * \param stack This stack will be filled
 * \param capacity New stack capacity
 * \note Free stack before contsructor to prevent memory leak
 * \return Error code (see #ERROR_BIT_FLAGS)
*/
ErrorBits stack_constructor(Stack *stack, StackSize capacity);


/**
 * \brief Adds object to stack
 * \param stack This stack will be pushed
 * \param object This object will be added to the end of stack
 * \note Stack will try resize to hold all the objects
 * \return Error code (see #ERROR_BIT_FLAGS)
*/
ErrorBits stack_push(Stack *stack, Object object);


/**
 * \brief Pops last object from stack
 * \param stack This stack will be popped
 * \param object Value of popped object will be written to this pointer
 * \note Stack will try resize if hold too few objects for its size
 * \return Error code (see #ERROR_BIT_FLAGS)
*/
ErrorBits stack_pop(Stack *stack, Object *object);


/**
 * \brief Destructs the stack
 * \param stack This stack will be destructed
 * \note Stack won't be free in case of verification error so get ready for memory leak
 * \return Error code (see #ERROR_BIT_FLAGS)
*/
ErrorBits stack_destructor(Stack *stack);


/**
 * \brief Stack verificator
 * \param stack Stack to check
 * \return Error code (see #ERROR_BIT_FLAGS)
*/
ErrorBits stack_check(Stack *stack);


/**
 * \brief Prints stack content
 * \param stack This stack will printed
 * \param error This error code will be printed
 * \param stream File to dump in
*/
void stack_dump(Stack *stack, ErrorBits error, FILE *stream);


/**
 * \brief Translate bit error code to english
 * \param error Bit error code to print
 * \param stream File to print errors in
*/
void print_errors(ErrorBits error, FILE *stream);
