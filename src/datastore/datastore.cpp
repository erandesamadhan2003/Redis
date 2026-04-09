#include "./datastore.h"
#include <sys/socket.h>
#include <algorithm>

DataStore::DataStore() {}

bool DataStore::isExpire(const std::string &key)
{
    auto it = store.find(key);
    if (it == store.end())
        return false;
    if (it->second.expire_time.has_value() && std::chrono::steady_clock::now() >= it->second.expire_time.value())
    {
        return true;
    }
    return false;
}

void DataStore::set(const std::string &key, const std::string &val, std::optional<std::chrono::steady_clock::time_point> expire_time)
{
    RedisObject obj;
    obj.type = RedisType::STRING;
    obj.string_value = val;
    store[key] = Entry{obj, expire_time};
}

std::string DataStore::get(const std::string &key)
{
    auto it = store.find(key);
    if (it == store.end())
        return "";
    if (isExpire(key))
    {
        store.erase(key);
        return "";
    }
    if (it->second.data.type != RedisType::STRING)
    {
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value");
    }
    return it->second.data.string_value;
}

bool DataStore::exist(const std::string &key)
{
    auto it = store.find(key);
    if (it == store.end())
        return false;
    if (isExpire(key))
    {
        store.erase(key);
        return false;
    }
    return true;
}

RedisType DataStore::getType(const std::string &key)
{
    return store[key].data.type;
}

int DataStore::rpush(const std::string &key, const std::vector<std::string> &values)
{
    auto it = store.find(key);
    if (it != store.end() && it->second.data.type != RedisType::LIST)
    {
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value");
    }
    if (it == store.end())
    {
        RedisObject obj;
        obj.type = RedisType::LIST;
        store[key] = Entry{obj, std::nullopt};
    }
    for (const auto &val : values)
    {
        store[key].data.list_value.push_back(val);
    }
    return store[key].data.list_value.size();
}

int DataStore::lpush(const std::string &key, const std::vector<std::string> &values)
{
    auto it = store.find(key);
    if (it != store.end() && it->second.data.type != RedisType::LIST)
    {
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value");
    }
    if (it == store.end())
    {
        RedisObject obj;
        obj.type = RedisType::LIST;
        store[key] = Entry{obj, std::nullopt};
    }
    for (const auto &val : values)
    {
        store[key].data.list_value.push_front(val);
    }
    return store[key].data.list_value.size();
}

std::deque<std::string> DataStore::lrange(const std::string &key, int start, int stop)
{
    auto it = store.find(key);
    if (it == store.end())
        return {};
    if (it->second.data.type != RedisType::LIST)
    {
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value");
    }

    std::deque<std::string> &list = it->second.data.list_value;
    int size = list.size();

    if (start < 0)
        start = std::max(0, size + start);
    if (stop < 0)
        stop = size + stop;
    stop = std::min(stop, size - 1);

    if (start > stop)
        return {};

    return std::deque<std::string>(list.begin() + start, list.begin() + stop + 1);
}

int DataStore::llen(const std::string &key)
{
    auto it = store.find(key);
    if (it == store.end())
        return 0;
    if (it->second.data.type != RedisType::LIST)
    {
        throw std::runtime_error("WRONGTYPE Operation against a key holding the wrong kind of value");
    }
    return it->second.data.list_value.size();
}

std::string DataStore::lpop(const std::string &key)
{
    auto it = store.find(key);
    std::string result = it->second.data.list_value.front();
    it->second.data.list_value.pop_front();
    return result;
}

void DataStore::addBlockedClient(const std::string &key, BlockedClient *blockedClient)
{
    if (blockedClient->positions.find(key) == blockedClient->positions.end())
    {
        auto &blocked_list = blocked_on_keys[key];
        blocked_list.push_back(blockedClient);
        auto it = blocked_list.end();
        --it;
        blockedClient->positions[key] = it;
    }

    if (std::find(blockedClients.begin(), blockedClients.end(), blockedClient) == blockedClients.end())
    {
        blockedClients.push_back(blockedClient);
    }
}

bool DataStore::isBlockedClientPresent(const std::string &key)
{
    auto it = blocked_on_keys.find(key);
    if (it == blocked_on_keys.end())
        return false;
    return !it->second.empty();
}

void DataStore::removeClientFromKeys(BlockedClient *client)
{
    for (const auto &entry : client->positions)
    {
        const std::string &key = entry.first;
        auto list_it = blocked_on_keys.find(key);
        if (list_it == blocked_on_keys.end())
            continue;

        list_it->second.erase(entry.second);
        if (list_it->second.empty())
        {
            blocked_on_keys.erase(list_it);
        }
    }
    client->positions.clear();
}

void DataStore::unblockClients(const std::string &key)
{
    auto key_it = blocked_on_keys.find(key);
    if (key_it == blocked_on_keys.end() || key_it->second.empty())
        return;

    BlockedClient *client = key_it->second.front();
    removeClientFromKeys(client);

    if (llen(key) == 0)
        return;
    std::string key_value = lpop(key);
    std::string response = "*2\r\n$" + std::to_string(key.size()) + "\r\n" + key + "\r\n$" + std::to_string(key_value.size()) + "\r\n" + key_value + "\r\n";

    send(client->fd, response.c_str(), response.size(), 0);

    for (auto it = blockedClients.begin(); it != blockedClients.end(); ++it)
    {
        if (*it == client)
        {
            blockedClients.erase(it);
            break;
        }
    }

    delete client;
}

void DataStore::handleBlockedTimeouts()
{
    auto now = std::chrono::steady_clock::now();
    for (auto it = blockedClients.begin(); it != blockedClients.end();)
    {
        BlockedClient *client = *it;
        if (client->expire_time <= now)
        {
            std::string response = "$-1\r\n";
            send(client->fd, response.c_str(), response.size(), 0);

            removeClientFromKeys(client);

            delete client;
            it = blockedClients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void DataStore::removeClient(int fd)
{
    for (auto it = blockedClients.begin(); it != blockedClients.end();)
    {
        BlockedClient *client = *it;

        if (client->fd == fd)
        {
            removeClientFromKeys(client);

            delete client;
            it = blockedClients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}