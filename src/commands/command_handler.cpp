#include "./command_handler.h"
#include "./../protocol/resp_parser.h"

CommandHandler::CommandHandler() {
    respParser = RESPParser();
    echoCommand = EchoCommand();
}

CommandHandler::CommandType CommandHandler::commandType(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return std::tolower(c); });

    if (cmd == "ping") {
        return CommandType::PING;
    } else if (cmd == "echo") {
        return CommandType::ECHO;
    } else {
        throw std::invalid_argument("Unknown command: " + command);
    }
}

std::string CommandHandler::handleCommand(const std::string& input) {
    std::vector<std::string> tokens = respParser.parse(input);

    if (tokens.empty()) {
        return "-Error: Empty command\r\n";
    }

    CommandType type;
    try {
        type = commandType(tokens[0]);
    } catch (const std::invalid_argument& e) {
        return "-Error: " + std::string(e.what()) + "\r\n";
    }

    switch (type) {
        case CommandType::PING:
            return "+PONG\r\n";
        case CommandType::ECHO:
            return respParser.serialize(echoCommand.execute(tokens));
        default:
            return "-Error: Unknown command type\r\n";
    }
}