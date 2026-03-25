#include "./server.h"
#include <unistd.h>
#include <iostream>
#include <cstring>

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

    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytes_read = read(client_fd, buffer, sizeof(buffer));

        if (bytes_read <= 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        const char *response = "+PONG\r\n";
        send(client_fd, response, strlen(response), 0);
    }

    close(client_fd);
}

Server::~Server() {}