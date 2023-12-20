Define_Module(Node);

//this function loads the input file and stores it in the input_data_vect
//the file looks like this:
/*
1010 A flower, sometimes
0000 known as a bloom or blossom in flowering plants
*/
void Node::read_input(std::string input_file_name)
{
    std::ifstream input_file(input_file_name);
    std::string line;
    while (std::getline(input_file, line))
    {
        //split the line into the code and the data
        std::string code = line.substr(0, 4);
        std::string message = line.substr(5, line.length());
        printf("code: %s, message: %s\n", code.c_str(), message.c_str());
        //push bach to member variables
        messages.push_back(message);
        errors.push_back(code);
    }
    input_file.close();
}

void Node::print_messages_and_errors()
{
    //iterating over member variables : messages, errors
    for (int i = 0; i < messages.size(); i++)
    {
        std::cout << "error: " << errors[i] << ", message: " << messages[i] << std::endl;
        EV << "error: " << errors[i] << ", message: " << messages[i] << std::endl;
    }
}


void Node::initialize()
{
    // TODO - Generated method body
    //read the input file
    read_input("./input_files/input0.txt");
    //print the messages and errors
    print_messages_and_errors();
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
