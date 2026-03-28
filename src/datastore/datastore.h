#pragma once 
#include <unordered_map>
#include <string>
#include <iostream>
#include <optional>
#include <chrono>

struct Entry {
    std::string value;
    std::optional<std::chrono::steady_clock::time_point> expire_time;
};

class DataStore {
private: 
    std::unordered_map<std::string, Entry> store;
    bool isExpire(const std::string& key);
public: 
    DataStore();
    void set(const std::string& key, const std::string& val, std::optional<std::chrono::steady_clock::time_point> expire_time = std::nullopt);
    std::string get(const std::string& key);
    bool exist(const std::string& key);
};

