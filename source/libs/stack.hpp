/// Stack data type
typedef int stack_data_t;


/// Stack
typedef struct {
    stack_data_t *data = nullptr;       ///< Array of stack_data_t elements
    int capacity       =       0;       ///< Maximum size
    int size           =       0;       ///< Actual number of elements
} Stack;


#define POISON_VALUE 0xC0FFEE
#define MAX_CAPACITY_VALUE 100000
#define OBJECT_TO_STR "%i"


/// Exit codes for stack functions
enum EXIT_CODES {
    INVALID_DATA     =  1,        ///< Invalid stack data points
    INVALID_SIZE     =  2,        ///< Size is larger than capacity or negative
    INVALID_CAPACITY =  3,        ///< Capacity is larger than type's maximum or negative
    UNEXP_POISON_VAL =  4,        ///< Unexpected poison value before size
    UNEXP_NORMAL_VAL =  5,        ///< Unexpected normal value after size
    INVALID_ARGUMENT =  6,        ///< Invalid argument given to the function
    EMPTY_STACK      =  7,        ///< No elements to pop
    ALLOCATE_FAIL    =  8,        ///< Memory allocate return NULL
};


/**
 * \brief Prints stack content
 * \param [in] stack Stack to print
 * \param [in] error This error code will printed
*/
#define STACK_DUMP(stack, error)                                                        \
do {                                                                                    \
    fprintf(stdout, "%s at %s(%d)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);         \
    stack_dump(stack, error, stdout);                                                   \
} while(0)


/**
 * \brief Constructs the stack
 * \param [in] stack    This stack will be filled
 * \param [in] capacity New stack capacity
 * \note Free stack before contsructor to prevent memory leak
 * \return Non zero value means error
*/
int stack_constructor(Stack *stack, int capacity);


/**
 * \brief Adds object to stack
 * \param [in] stack  This stack will be pushed
 * \param [in] object This object will be added to the end of stack
 * \note Stack will try to resize to hold all the objects
 * \return Non zero value means error
*/
int stack_push(Stack *stack, stack_data_t object);


/**
 * \brief Returns last object from stack and removes it
 * \param [in] stack  This stack will be popped
 * \param [in] object Value of popped object will be written to this pointer
 * \note Stack will try to resize if hold too few objects for its size
 * \return Non zero value means error
*/
int stack_pop(Stack *stack, stack_data_t *object);


/**
 * \brief Destructs the stack
 * \param [in] stack This stack will be destructed
 * \note Stack won't be free in case of verification error so get ready for memory leak
 * \return Non zero value means error
*/
int stack_destructor(Stack *stack);


/**
 * \brief Stack verificator
 * \param [in] stack Stack to check
 * \return Non zero value means error
*/
int stack_verificator(Stack *stack);


/**
 * \brief Prints stack content
 * \param [in]  stack  This stack will printed
 * \param [in]  error  This error code will be printed
 * \param [out] stream File to dump in
*/
void stack_dump(Stack *stack, int error, FILE *stream);
