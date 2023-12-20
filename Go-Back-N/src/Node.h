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

#ifndef __GO_BACK_N_NODE_H_
#define __GO_BACK_N_NODE_H_

#include <omnetpp.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include "MyMessage_m.h"
using namespace omnetpp;

enum CODES {
    NO_ERRORs,
    DELAY,
    DUP,
    DUP_DELAY,
    LOSS,
    LOSS_BOTH,
    MOD,
    MOD_DELAY,
    MOD_DUP,
    MOD_DUP_DELAY,
};

std::map<std::string, CODES> error_codes = {
        {"0000", NO_ERRORs},
        {"0001", DELAY},
        {"0010", DUP},
        {"0011", DUP_DELAY},
        {"0100", LOSS},
        {"0101", LOSS},
        {"0110", LOSS_BOTH},
        {"0111", LOSS_BOTH},
        {"1000", MOD},
        {"1001", MOD_DELAY},
        {"1010", MOD_DUP},
        {"1011", MOD_DUP_DELAY},
        {"1100", LOSS},
        {"1101", LOSS},
        {"1110", LOSS_BOTH},
        {"1111", LOSS_BOTH},
};

enum LOG_INFO_TYPE {
    UPON_READING,
    BEFORE_TRANS,
    TIMEOUT_EVENT,
    CONTROL_FRAME,
};

class Node : public cSimpleModule
{
private:
   std::vector<std::string> messages;
   std::vector<std::string> errors;

   //function to read input file
   void read_input(std::string input_file_name);
   void print_messages_and_errors();

   protected:
   virtual void initialize() override;
   virtual void handleMessage(cMessage *msg) override;
};

#endif
