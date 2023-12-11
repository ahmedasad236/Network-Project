//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Coordinator.h"

Define_Module(Coordinator);

void Coordinator::initialize()
{
    std::string line;
    std::ifstream filestream("../src/coordinator.txt");
    if(!filestream.is_open()) return;



    while (getline(filestream, line)) {

            int* data;
            data = readInput(line);
            int nodeId = data[0]; //atoi(line.c_str());
            int startTime = data[1]; //atoi(line.c_str() + 1);

            EV << "nodeNumber From Coordinator = " << nodeId << endl;
            EV << "startingTime From Coordinator  = " << startTime << endl;

            cMessage *msg = new cMessage("Coordinator start message");
            msg->addPar("nodeId");
            msg->par("nodeId").setLongValue(nodeId);

            msg->addPar("startTime");
            msg->par("startTime").setLongValue(startTime);

            if(nodeId == 0)
                send(msg, "out_ports", 0);
            else if(nodeId == 1)
                send(msg, "out_ports", 1);
    }

    filestream.close();
}

void Coordinator::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

int* readInput(std::string s, std::string del= " ") {
    int start, end = -1*del.size(), i = 0;
    int data [2];

    do {
        start = end + del.size();
        end = s.find(del, start);
        data[i] = atoi(s.substr(start, end - start).c_str());
        i++;
    } while (end != -1);

    return data;
}
