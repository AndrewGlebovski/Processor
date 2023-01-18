Command command_list[] = {
    {
        "-i", "--input", 
        0, 
        &set_input_file, 
        &input,
        "<filepath> Path to assembler source file"
    },
    {
        "-o", "--output", 
        0, 
        &set_output_file, 
        &output,
        "<filepath> Path to binary output file"
    },
    {
        "-h", "--help", 
        0, 
        &show_help, 
        &command_list,
        "Prints all commands descriptions"
    },
};