#include "./set_command.h"
#include <stdexcept>
#include <algorithm>

void SetCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() < 3) throw std::invalid_argument("wrong number of arguments for 'set' command");

    const std::string& key = args[1];
    const std::string& val = args[2];
    std::optional<std::chrono::steady_clock::time_point> expire_time = std::nullopt;

    if (args.size() >= 5) {
        std::string option = args[3];
        std::transform(option.begin(), option.end(), option.begin(), [](unsigned char c){ return std::tolower(c); });

        if (option == "ex") {
            int seconds = std::stoi(args[4]);
            expire_time = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
        } else if (option == "px") {
            int ms = std::stoi(args[4]);
            expire_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
        } else {
            throw std::invalid_argument("unknown option for 'set' command");
        }
    }

    store.set(key, val, expire_time);
}