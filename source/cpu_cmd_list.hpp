Command command_list[] = {
    {
        "-i", "--input", 
        0, 
        &set_input_file, 
        &input,
        "<relative path to a file> Changes input file"
    },
    {
        "-h", "--help", 
        0, 
        &show_help, 
        &command_list,
        "Prints all commands descriptions"
    },
};