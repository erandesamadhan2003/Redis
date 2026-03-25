#include "./server.h"
#include <unistd.h>
#include <iostream>

Server::Server(int port) : port(port) {}

void Server::start() {
    server_socket.createSocket();
    server_socket.bindSocket(port);
    server_socket.listenSocket();

    std::cout << "Waiting for a client to connect...\n";
    std::cout << "Logs from your program will appear here!\n";
}

void Server::run() {
    int client_fd = server_socket.acceptClient();
    std::cout << "Client connected\n";

    close(client_fd);
}

Server::~Server() {}