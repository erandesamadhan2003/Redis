#include "./echo_command.h"

EchoCommand::EchoCommand() {}

std::vector<std::string> EchoCommand::execute(const std::vector<std::string>& args) {
    if(args.size() <= 1) {
        return {};
    }

    return std::vector<std::string>(args.begin() + 1, args.end());
}