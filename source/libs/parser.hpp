/**
 * \file
 * \brief Arguments parser module header
*/


/// Contains information about command
typedef struct {
	const char* short_name; ///< Command short name
	const char* long_name; ///< Command long name
	int mode; ///< Mode
	void (*func)(char *argv[], void *data); ///< Command parser function
	void *data; ///< Additional data
	const char *desc; ///< Command description
} Command;


/**
 * \brief Parses arguments
 * \param [in] argc Number of arguments
 * \param [in] argv Array of arguments
 * \param [in] command_list Array of commands
 * \param [in] list_size Number of commands
 * \return Non zero value means error
*/
int parse_args(int argc, char* argv[], Command command_list[], size_t list_size);
