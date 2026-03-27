#include "./command_handler.h"

CommandHandler::CommandHandler(DataStore* store) : store(store) {}

CommandHandler::CommandType CommandHandler::commandType(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return std::tolower(c); });

    if (cmd == "ping")       return CommandType::PING;
    else if (cmd == "echo")  return CommandType::ECHO;
    else if (cmd == "set")   return CommandType::SET;
    else if (cmd == "get")   return CommandType::GET;
    else throw std::invalid_argument("Unknown command: " + command);
}

std::string CommandHandler::handleCommand(const std::string& input) {
    std::vector<std::string> tokens = respParser.parse(input);

    if (tokens.empty()) return "-Error: Empty command\r\n";

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

        case CommandType::SET:
            try {
                setCommand.execute(tokens, *store);
                return "+OK\r\n";
            } catch (const std::invalid_argument& e) {
                return "-Error: " + std::string(e.what()) + "\r\n";
            }

        case CommandType::GET: {
            if (tokens.size() < 2) return "-Error: GET requires a key\r\n";
            if (!store->exist(tokens[1])) return "$-1\r\n";
            return respParser.serialize({store->get(tokens[1])});
        }

        default:
            return "-Error: Unknown command\r\n";
    }
}