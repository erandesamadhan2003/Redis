#include "./lpop_command.h"

std::vector<std::string> LPOPCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 2) 
        throw std::invalid_argument("wrong number of arguments for 'lpop' command");
    
    if (!store.exist(args[1])) 
        return {};
    
    RedisType type = store.getType(args[1]);
    if (type != RedisType::LIST) 
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value"); 
    
    std::vector<std::string> result;
    
    int n = args.size() == 2 ? 1 : std::stoi(args[2]);

    for (int i = 0; i < n; ++i) {
        std::string val = store.lpop(args[1]);
        if (val.empty()) break;
        result.push_back(val);
    } 

    return result;
}