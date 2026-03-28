#include "./get_command.h"

std::vector<std::string> GetCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 2) throw std::invalid_argument("wrong number of arguments for 'get' command");
    if (!store.exist(args[1])) return {"$-1\r\n"};
    return {store.get(args[1])};
}