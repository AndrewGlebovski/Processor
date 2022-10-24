void set_input_file(char *argv[], void *data);  ///< -i parser
void show_help(char *argv[], void *data);       ///< -h parser


void set_input_file(char *argv[], void *data) {
    if (*(++argv)) {
        *(int *)(data) = open(*argv, O_RDONLY | O_BINARY);

        if (*(int *)(data) == -1)
            printf("Can't open file %s!\n", *argv);
    }
    else {
        printf("No filename after -i, argument ignored!\n");
	}
}


void show_help(char *argv[], void *data) {
	for(size_t i = 0; i < 2; i++) {
		printf("%s %s %s\n", ((Command *)(data))[i].short_name, ((Command *)(data))[i].long_name, ((Command *)(data))[i].desc);
	}
}