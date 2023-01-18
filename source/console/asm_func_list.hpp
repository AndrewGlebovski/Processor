void set_input_file(char *argv[], void *data);  ///< -i parser
void set_output_file(char *argv[], void *data); ///< -o parser
void show_help(char *argv[], void *data);       ///< -h parser


void set_input_file(char *argv[], void *data) {
	if (*(++argv)) {
		*(int *)(data) = open(*argv, O_RDONLY);

        if (*(int *)(data) == -1)
            printf("Can't open file %s!\n", *argv);
	}
	else {
		printf("No filename after -i, argument ignored!\n");
	}
}


void set_output_file(char *argv[], void *data) {
    if (*(++argv)) {
        *(int *)(data) = open(*argv, O_WRONLY | O_CREAT | O_BINARY, 00770);

        if (*(int *)(data) == -1)
            printf("Can't open file %s!\n", *argv);
    }
    else {
        printf("No filename after -o, argument ignored!\n");
    }
}


void show_help(char *argv[], void *data) {
    size_t i = 0;

    for(; strcmp(((Command *)(data))[i].short_name, "-h") != 0; i++) {
        printf("%s %s %s\n", ((Command *)(data))[i].short_name, ((Command *)(data))[i].long_name, ((Command *)(data))[i].desc);
    }

    printf("%s %s %s\n", ((Command *)(data))[i].short_name, ((Command *)(data))[i].long_name, ((Command *)(data))[i].desc);
}