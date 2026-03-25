#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
class Socket {
    int server_fd;
    struct sockaddr_in server_addr;
public:
    Socket();

    void createSocket();
    void bindSocket(int port);
    void listenSocket();
    int acceptClient();
    
    int getServerFd();

    ~Socket();
};