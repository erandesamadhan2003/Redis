#pragma once 
#include "./../datastore/datastore.h"
#include <vector>
#include <string>

class LPOPCommand {
public:
    std::vector<std::string> execute(const std::vector<std::string>& args, DataStore& store);
};