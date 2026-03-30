#pragma once
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include "./../protocol/resp_parser.h"
#include "./echo_command.h"
#include "./set_command.h"
#include "./get_command.h"
#include "./push_command.h"
#include "./../datastore/datastore.h"
#include "./lrange_command.h"
class CommandHandler {
private:
    DataStore* store;
    RESPParser respParser;
    EchoCommand echoCommand;
    SetCommand setCommand;
    GetCommand getCommand;
    RPushCommand rpushCommand;
    LRANGECommand lrangeCommand;
    enum class CommandType { PING, ECHO, GET, SET, RPUSH, LRANGE };
    CommandType commandType(const std::string& command);

public:
    CommandHandler() = default;
    CommandHandler(DataStore* store);
    std::string handleCommand(const std::string& input);
};