#pragma once 
#include <unordered_map>
#include <string>
#include <iostream>

class DataStore {
private: 
    std::unordered_map<std::string, std::string> store;

public: 
    DataStore();
    void set(const std::string& key, const std::string& val);
    std::string get(const std::string& key);
    bool exist(const std::string& key);
};

