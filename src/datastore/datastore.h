#pragma once 
#include <unordered_map>
#include <string>
#include <optional>
#include <chrono>
#include <deque>
#include <vector>
#include <stdexcept>

enum class RedisType {
    STRING,
    LIST,
    SET,
    HASH
};

struct RedisObject {
    RedisType type;
    std::string string_value;
    std::deque<std::string> list_value;
};

struct Entry {
    RedisObject data;
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
    RedisType getType(const std::string& key);
    int rpush(const std::string& key, const std::vector<std::string>& values);
    int lpush(const std::string& key, const std::vector<std::string>& values);
    std::deque<std::string> lrange(const std::string& key, int start, int stop);
    int llen(const std::string& key);
};