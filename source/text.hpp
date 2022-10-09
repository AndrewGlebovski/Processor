typedef struct {
    const char *str = nullptr; ///< Pointer
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
