#pragma once

#include <string>
#include <vector>
#include <iostream>

class RESPParser {
    public:
    RESPParser();
    std::vector<std::string> parse(const std::string& input);
    std::string serialize(const std::vector<std::string>& data);
};