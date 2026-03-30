#pragma once
#include "./../datastore/datastore.h"
#include <vector>
#include <string>

class RPushCommand {
public:
    int execute(const std::vector<std::string>& args, DataStore& store);
};

class LPUSHCommand {
public:
    int execute(const std::vector<std::string>& args, DataStore& store);
};