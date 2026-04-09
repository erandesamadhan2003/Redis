#pragma once
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include "./../protocol/resp_parser.h"
#include "./set_command.h"
#include "./get_command.h"
#include "./push_command.h"
#include "./../datastore/datastore.h"
#include "./lrange_command.h"
#include "./lpop_command.h"
#include "./blpop_command.h"
class CommandHandler {
private:
    DataStore& store;
    RESPParser respParser;
    SetCommand setCommand;
    GetCommand getCommand;
    RPushCommand rpushCommand;
    LPUSHCommand lpushCommand;
    LRANGECommand lrangeCommand;
    LPOPCommand lpopCommand;
    BlPOPCommand blpopCommand;
    enum class CommandType { PING, ECHO, GET, SET, RPUSH, LPUSH, LRANGE, LLEN, LPOP, BLPOP };
    CommandType commandType(const std::string& command);

public:
    CommandHandler(DataStore& store);
    std::string handleCommand(const std::string& input, int client_fd);
};