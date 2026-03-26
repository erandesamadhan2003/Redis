#pragma once
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include "./../protocol/resp_parser.h"
#include "./echo_command.h"

class CommandHandler {
private:
    RESPParser respParser;
    EchoCommand echoCommand;

    enum class CommandType {
        PING,
        ECHO
    };

    CommandType commandType(const std::string& command) {}

public:
    CommandHandler();
    std::string handleCommand(const std::string& input) {}
};