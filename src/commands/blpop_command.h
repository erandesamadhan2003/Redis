#pragma once
#include "./../datastore/datastore.h"
#include <vector>
#include <string>

class BlPOPCommand {
public:
    std::vector<std::string> execute(std::vector<std::string>& args, DataStore& datastore, int client_fd);
};