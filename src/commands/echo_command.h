#pragma once 
#include <string>
#include <vector>

class EchoCommand {
public: 
    EchoCommand();

    std::vector<std::string> execute(const std::vector<std::string>& args); 
};