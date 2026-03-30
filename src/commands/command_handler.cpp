#include "./command_handler.h"

CommandHandler::CommandHandler(DataStore* store) : store(store) {}

CommandHandler::CommandType CommandHandler::commandType(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c) { return std::tolower(c); });

    if (cmd == "ping")       return CommandType::PING;
    else if (cmd == "echo")  return CommandType::ECHO;
    else if (cmd == "set")   return CommandType::SET;
    else if (cmd == "get")   return CommandType::GET;
    else if (cmd == "rpush") return CommandType::RPUSH;
    else if (cmd == "lrange") return CommandType::LRANGE;
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
            if (tokens.size() != 2) return "-ERR wrong number of arguments for 'echo' command\r\n";
            return "$" + std::to_string(tokens[1].size()) + "\r\n" + tokens[1] + "\r\n";
            // return respParser.serialize(echoCommand.execute(tokens));
        
        case CommandType::SET:
            try {
                setCommand.execute(tokens, *store);
                return "+OK\r\n";
            } catch (const std::invalid_argument& e) {
                return "-Error: " + std::string(e.what()) + "\r\n";
            }

        case CommandType::GET: {
            try {
                auto result = getCommand.execute(tokens, *store);
                if (result.empty()) return "$-1\r\n";
                if (result[0] == "$-1\r\n") return "$-1\r\n";
                std::string val = result[0];
                return "$" + std::to_string(val.size()) + "\r\n" + val + "\r\n";
            } catch (const std::exception& e) {
                return "-" + std::string(e.what()) + "\r\n";
            }
        }
        
        case CommandType::RPUSH: {
            try {
                int len = rpushCommand.execute(tokens, *store);
                return ":" + std::to_string(len) + "\r\n";
            } catch (const std::exception& e) {
                return "-" + std::string(e.what()) + "\r\n";
            }
        }
        
        case CommandType::LRANGE: {
            try {
                std::vector<std::string> result = lrangeCommand.execute(tokens, *store);
                if (result.empty()) return "*0\r\n";
                return respParser.serialize(result);
            } catch (const std::exception& e) {
                return "-ERR " + std::string(e.what()) + "\r\n";
            }
        }
        default:
            return "-Error: Unknown command\r\n";
    }
}