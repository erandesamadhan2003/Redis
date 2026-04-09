#include "./push_command.h"
#include "./blpop_command.h"

int RPushCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 3) throw std::invalid_argument("wrong number of arguments for 'rpush' command");
    std::vector<std::string> values(args.begin() + 2, args.end());
    int result = store.rpush(args[1], values);

    while (store.isBlockedClientPresent(args[1]) && store.llen(args[1]) > 0) {
        store.unblockClients(args[1]);
    }

    return result;
}

int LPUSHCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 3) throw std::invalid_argument("wrong number of arguments for 'lpush' command");
    std::vector<std::string> values(args.begin() + 2, args.end());
    int result = store.lpush(args[1], values);
    
    while (store.isBlockedClientPresent(args[1]) && store.llen(args[1]) > 0) {
        store.unblockClients(args[1]);
    }
    
    return result;
}