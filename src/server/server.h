#pragma once
#include "./../network/socket.h"
#include <iostream>
#include "./../commands/command_handler.h"

class Server {
    Socket server_socket;
    int port;
    CommandHandler commandHandler;

public:
    Server(int port);
    void start();
    void run();
    ~Server();
};