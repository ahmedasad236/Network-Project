Define_Module(Node);

//this function loads the input file and stores it in the input_data_vect
//the file looks like this:
/*
1010 A flower, sometimes
0000 known as a bloom or blossom in flowering plants
*/

void Node::read_input(std::string input_file_name)
{
    std::string line;
    int lines = 0;
    while (std::getline(inputFile, line))
    {
        //split the line into the code and the data
        std::string code = line.substr(0, 4);
        std::string message = line.substr(5, line.length());
        printf("code: %s, message: %s\n", code.c_str(), message.c_str());
        //push bach to member variables
        messages.push_back(message);
        errors.push_back(code);
        lines++;
    }
    numOfLines = lines;
    inputFile.close();
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

void Node::startTimer(int frameSeqNum, double delayTime = 0) {
    // start new timer for the current message
   cMessage* timeout_message = new cMessage("timeout");
   timeout_message->addPar("frame_seq");
   timeout_message->par("frame_seq").setLongValue(frameSeqNum);

   // set the timer value according to TO parameter
   scheduleAt(simTime()+static_cast<simtime_t>(getParentModule()->par("TO").doubleValue() + delayTime), timeout_message);

   // store the timer msg pointer to access later in case of stop_timer
   timeoutsBuffer[frameSeqNum] = timeout_message;
}


void Node::stopTimer(int frameSeqNum) {
    cancelAndDelete(timeoutsBuffer[frameSeqNum]);
    timeoutsBuffer[frameSeqNum] = nullptr;
}

void Node::incrementFrameSeq(int& frameSeqNum) {

   // WS = sequence_numbers - 1
   // So, we take modulo (WS + 1)
   frameSeqNum = (frameSeqNum + 1) % (getParentModule()->par("WS").intValue() + 1);
}

void Node::handleTimeout(int frameSequence) {
    int currBufferedFrames = numberOfBufferedFrames;
    numberOfBufferedFrames = 0;

    // get the sequence number of the first buffered message
    nextFrameToSend = framesInBuffer[0].second->getHeader();

    double sendingTime = 0;

    // re-transmit all frames in the buffer
    for (int i = 0; i < currBufferedFrames; i++){

        std::pair<std::string, MyMessage_Base*> frameInfo = frames_buffer[i];
        std::string payload(frameInfo.second->getM_Payload());
        /*
         * all the messages in the sender’s window will be transmitted again as
         * usual with the normal calculations for errors and delays except for
         * the first frame that is the cause for the timeout,
         * it will be sent error free
         * */

        sendingTime += getParentModule()->par("PT").doubleValue();

        // remove the timeout for this frame if any
        stopTimer(frameInfo.second->getHeader());

        if(frameInfo.second->getHeader() == frameSequence){
            log_to_file(TIMEOUT_EVENT, simTime().dbl(), nodeId, frameInfo.second, false);
            sendData(payload, "0000", sendingTime);
        }
        else {
            sendData(payload, frameInfo.first, sendingTime);
        }
    }


}

void Node::sendData(std::string payload, std::string error, double sendingOffsetTime = 0.0) {

    // create the message name based on the sequence number
    std::string messageName = "message " + std::to_string(nextFrameToSend);
    MyMessage_Base* msg = new MyMessage_Base(messageName.c_str());

    // fill message information
    msg->setHeader(nextFrameToSend);
    msg->setM_Payload(payload.c_str());
    msg->setFrame_Type(0);
    int ackNum = (nextFrameToSend + 1) % (MAX_SEQ + 1);
    msg->setAckNum(ackNum);

    // clone the message to store a copy of it
    MyMessage_Base* duplicatedMsg = msg->dup();

    // apply framing algorithm and update the payload of the message
   std::string framedPayload = framePacket(payload);
   msg->setM_Payload(framedPayload);

   // Set parity
   applyParity(msg);

   // Store the message in the buffer
   framesInBuffer[nextFrameToSend] = {error, duplicatedMsg};

   // apply errors on the message according to the error code

   // Start the timer for this frame
   startTime(nextFrameToSend, sendingOffsetTime);


   // Expand the sender's window
   numberOfBufferedFrames++;
   incrementFrameSeq(nextFrameToSend);

}


void Node::applyParity(MyMessage_Base *msg) {

    std::bitset<8> parityByte(0);

    for(int i = 0; i < msg->M_Payload.size(); i++)
        parityByte ^= std::bitset<8>( msg->M_Payload.c_str()[i]);

    msg->setTrailer(parityByte.to_ulong());
}

void Node::startProtocol() {
    MAX_SEQ = getParentModule()->par("PT").intValue();
    sender = true;

    std::string inputFileName = "../src/input";
    inputFileName.push_back(nodeId);
    inputFileName.append(".txt");

    // prepare input file for reading messages
    inputFile.open(input_file_name);

    EV << "Input File is open = " << inputFile.is_open() <<endl;

    framesInBuffer.resize(MAX_SEQ);
    timeoutsBuffer.resize(MAX_SEQ);

    readInputFile();
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
    enableNetworkLayer = true;

    // set node id
    std::string nodeName(getName());
    nodeId = nodeName.back();

    std::string output_file_name = "../src/output";
    output_file_name.push_back(nodeId);
    output_file_name.append(".txt");
    output_file.open(output_file_name);
    EV << "Output File is open = " << output_file.is_open() <<endl;

    // buffers to store pointers to messages
    framesInBuffer.resize(MAX_SEQ);
    timeoutsBuffer.resize(MAX_SEQ);

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

double Node::delayFrame(double time, MyMessage_Base *msg) {
    time += getParentModule()->par("ED").doubleValue();
    return time;
}


int Node::modifyFrame(double time, MyMessage_Base *msg){
    std::string payload(msg->getM_Payload());

    // select random bit index
    int bit_index =  intuniform(0,static_cast<int>(payload.length())*8-1, 0);

    // get the char that contains that bit
    char selected_char =  payload[bit_index/8];

    // construct bitset of the char to flip the selected bit
    std::bitset<8> char_bits(selected_char);

    // flip the selected bit
    char_bits[bit_index%8].flip();

    // update the payload after the modification
    payload[bit_index/8] = static_cast<char>(char_bits.to_ulong());

    msg->setM_Payload(payload.c_str());

    return bit_index;
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
