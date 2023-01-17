/**
 * \file
 * \brief Text module header
*/


/// This structure contains string and its size
typedef struct {
    char *str = nullptr; ///< Pointer
    int len = -1; ///< Length
} String;


/// Contains array of #String and its size
typedef struct {
    String *lines = nullptr; ///< Array of strings
    char* buffer = nullptr; ///< Pointer to an array of chars
    long size = -1; ///< Lines size
} Text;


/**
 * \brief Parse each line from file to the array
 * \param [out] text Text to read in
 * \param [in] file Input file
 * \return Non zero value means error
 * \note New line symbol will be discarded
*/
int read_text(Text *text, int file);


/**
 * \brief Returns file size in bytes
 * \param [in] file File descryptor
 * \return Size in bytes
*/
size_t get_file_size(int file);


/**
 * \brief Replaces specific character in string 
 * \param [out] buffer Char array to change
 * \param [in]  old_char This symbol will be replaced
 * \param [in]  new_char This symbol will be set
 * \return Count of replacement
*/
int replace_in_buffer(char *buffer, const char old_char, const char new_char);


/**
 * \brief Reads file into buffer
 * \param [in]  file File descryptor to read from
 * \param [out] buffer Pointer to char array
 * \param [in]  size Max amount symbols to read
 * \return Actual number of read chars
*/
size_t read_in_buffer(int file, char **buffer, size_t size);


/**
 * \brief Prints array of strings to the given file
 * \param [in] lines Array of strings
 * \param [in] file Output file
 * \return Non zero value means error
 * \note New line symbol will be added to the end of each line
*/
int print_lines(String lines[], int file);


/**
 * \brief Free text
 * \param [in] text It will be free
 * \return Non zero value means error
 * \warning If some pointer is null, nothing will be free
*/
int free_text(Text *text);
