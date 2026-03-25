#pragma once
#include "./../network/socket.h"
#include <iostream>

class Server {
    Socket server_socket;
    int port;

public:
    Server(int port);
    void start();
    void run();
    ~Server();
};