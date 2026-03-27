#include "./datastore.h"

DataStore::DataStore() {}

void DataStore::set(const std::string& key, const std::string& val) {
    store[key] = val;
}

std::string DataStore::get(const std::string& key) {
    if (store.find(key) == store.end()) return "";
    return store[key];
}

bool DataStore::exist(const std::string& key) {
    return store.find(key) != store.end();
}