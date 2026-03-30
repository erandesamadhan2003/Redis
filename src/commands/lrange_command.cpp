#include "./lrange_command.h"

std::vector<std::string> LRANGECommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() != 4) throw std::invalid_argument("wrong number of arguments for 'lrange' command");
    int start, stop;
    try {
        start = std::stoi(args[2]);
        stop = std::stoi(args[3]);
        std::deque<std::string> result = store.lrange(args[1], start, stop);
        return std::vector<std::string>(result.begin(), result.end());
    } catch (const std::exception& e) {
        throw std::invalid_argument("value is not an integer or out of range");
    }
}