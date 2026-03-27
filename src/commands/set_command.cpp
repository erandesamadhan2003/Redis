#include "./set_command.h"

void SetCommand::execute(const std::vector<std::string>& args, DataStore& store) {
    if (args.size() != 3) throw std::invalid_argument("Invalid Command");
    store.set(args[1], args[2]);
}