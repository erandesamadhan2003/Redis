#include "./set_command.h"

void SetCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 3) {
        std::cerr << "Error: SET command requires at least 2 arguments" << std::endl;
        return;
    }
    const std::string& key = args[1];
    const std::string& val = args[2];
    std::optional<std::chrono::steady_clock::time_point> expire_time = std::nullopt;
    if (args.size() > 3 && args[3] == "EX") {
        if (args.size() < 5) {
            std::cerr << "Error: SET command with EX option requires a time argument" << std::endl;
            return;
        }
        int seconds = std::stoi(args[4]);
        expire_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    }
    if (args.size() > 3 && args[3] == "PX") {
        if (args.size() < 5) {
            std::cerr << "Error: SET command with PX option requires a time argument" << std::endl;
            return;
        }
        int milliseconds = std::stoi(args[4]);
        expire_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds);
    }
    store.set(key, val, expire_time);
    
}