Command command_list[] = {
    {
        "-i", "--input", 
        0, 
        &set_input_file, 
        &input,
        "<filepath> Path to binary file for execution"
    },
    {
        "-h", "--help", 
        0, 
        &show_help, 
        &command_list,
        "Prints all commands descriptions"
    },
};