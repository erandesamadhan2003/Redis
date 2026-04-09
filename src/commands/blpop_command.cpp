#include "./blpop_command.h"

std::vector<std::string> BlPOPCommand::execute(std::vector<std::string>& args, DataStore& store, int client_fd) {
    if(args.size() < 3) 
        throw std::invalid_argument("ERR wrong number of arguments for 'blpop' command");
    
    std::vector<std::string> keys(args.begin() + 1, args.end() - 1);

    for(auto key: keys) 
        if(store.exist(key) && store.getType(key) == RedisType::LIST && store.llen(key) > 0) 
            return {store.lpop(key)};
    
    BlockedClient* client = new BlockedClient();
    client->fd = client_fd;
    for(auto key: keys) 
        client->keys.push_back(key);

    int time = stoi(args[args.size()-1]);
    if(time > 0) 
        client->expire_time = std::chrono::steady_clock::now() + std::chrono::seconds(time);
    else 
        client->expire_time = std::chrono::steady_clock::time_point::max();
    
    for(auto key: keys) 
        store.addBlockedClient(key, client);
    
    return {}; 
}