#include "./socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Socket::Socket() {
    server_fd = -1;
    memset(&server_addr, 0, sizeof(server_addr));
}

void Socket::createSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Failed to create server socket\n";
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
        std::cerr << "setsockopt failed\n";
        exit(1);
    }
}

void Socket::bindSocket(int port) {
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Failed to bind to port " << port << "\n";
        exit(1);
    }
}

void Socket::listenSocket() {
    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        std::cerr << "listen failed\n";
        exit(1);
    }
}

int Socket::acceptClient() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_fd < 0) {
        std::cerr << "Failed to accept client connection\n";
        exit(1);
    }

    return client_fd;
}

int Socket::getServerFd() {
    return server_fd;
}

Socket::~Socket() {
    if (server_fd >= 0) {
        close(server_fd);
    }
}