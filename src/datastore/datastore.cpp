#include "./datastore.h"

DataStore::DataStore() {}

void DataStore::set(const std::string& key, const std::string& val, std::optional<std::chrono::steady_clock::time_point> expire_time = std::nullopt) {
    store[key] = Entry{val, expire_time};
}

std::string DataStore::get(const std::string& key) {
    if (store.find(key) == store.end()) return "";
    return store[key].value;
}

bool DataStore::exist(const std::string& key) {
    if (store.find(key) == store.end()) return false;
    if (isExpire(key)) {
        store.erase(key);
        return false;
    }
    return true;
}

bool DataStore::isExpire(const std::string& key) {
    if (store[key].expire_time.has_value() && std::chrono::steady_clock::now() >= store[key].expire_time.value()) {
        return true;
    }
    return false;
}