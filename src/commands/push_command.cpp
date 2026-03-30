#include "./push_command.h"

int RPushCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 3) throw std::invalid_argument("wrong number of arguments for 'rpush' command");
    std::vector<std::string> values(args.begin() + 2, args.end());
    return store.rpush(args[1], values);
}