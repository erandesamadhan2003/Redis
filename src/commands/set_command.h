#pragma once
#include "./../datastore/datastore.h"
#include <vector>
#include <string>

class SetCommand {
public:
    void execute(const std::vector<std::string>& args, DataStore& store);
};