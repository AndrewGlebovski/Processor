typedef struct {
    char *str = nullptr; ///< Pointer
    int len = -1; ///< Length
} String;


typedef struct {
    String *lines = nullptr; ///< Array of strings
    char* buffer = nullptr; ///< Pointer to an array of chars
    int size = -1; ///< Lines count
} Text;


size_t get_file_size(int file);


int replace_in_buffer(char *buffer, const char old_char, const char new_char);


int free_text(Text *text);


size_t read_in_buffer(int file, char **buffer, size_t size);


int read_file(int file, Text *text);


/**
 * \brief Finds token in string
 * \param [in] origin Search start pointer
 * \param [in] solo Solo delimeters
 * \param [in] extra Interpreted as end of line
 * \warning Token is not always end with \0, so you need to rely on String length field
 * \return Token pointer and size
*/
String get_token(char *origin, const char *solo, const char *extra);


/**
 * \brief Compares two strings structs
 * \param [in] str1 First string to compare
 * \param [in] str2 Second string to compare
 * \return Non zero value means equality
*/
int is_equal(String *str1, String *str2);


/**
 * \brief Compares two strings structs
 * \param [in] str1 First string to compare
 * \param [in] str2 Second string to compare
 * \warning Compares first str1.len symbols from str1 and str2
 * \return Non zero value means equality
*/
int is_equal(String *str1, const char *str2);


/**
 * \brief Converts string in number
 * \param [in] str String to converts
 * \param [in] value Expected integer value
 * \return Non zero value means unconvertable string
*/
int str_to_int(String *str, int *value);


/**
 * \brief Prints string based on it's length field
 * \param [in] str String to print
*/
void print_string(String *str);
