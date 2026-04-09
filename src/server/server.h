#pragma once
#include "./../network/socket.h"
#include <iostream>
#include "./../commands/command_handler.h"
#include "./../datastore/datastore.h"
class Server {
    Socket server_socket;
    int port;
    DataStore store;
    CommandHandler commandHandler;
public:
    Server(int port);
    void start();
    void run();
    ~Server();
};