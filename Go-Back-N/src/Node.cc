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
    // sender's parameters
    nextFrameToSend = 0;
    ackExpected = 0;


    // receiver's parameter
    frameExpected = 0;


    numberOfBufferedFrames = 0;

    // enable network layer
    network_layer_enabled = true;

    // set node id
    std::string nodeName(getName());
    nodeId = nodeName.back();

    std::string output_file_name = "../src/output";
    output_file_name.push_back(nodeId);
    output_file_name.append(".txt");
    output_file.open(output_file_name);
    EV << "Output File is open = " << output_file.is_open() <<endl;

    // buffers to store pointers to messages
    frames_buffer.resize(MAX_SEQ);
    timeouts_buffer.resize(MAX_SEQ);

}

std::string Node::packetFraming(std::string payload) {
    std::string frame = "";
    frame += '$';
    for(int i = 0; i < payload.size(); i++){
        if(payload[i] == '$' || payload[i] == '/')
            frame+='/';

        frame += payload[i];
    }
    frame+='$';

    return frame;
}

double Node::duplicateFrame(double time, MyMessage_Base *msg) {
    MyMessage_Base *duplicatedMsg = msg->dup();
    time += getParentModule()->par("DD").doubleValue();
    sendDelayed(duplicatedMsg, time, "out_port");
    return time;
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
