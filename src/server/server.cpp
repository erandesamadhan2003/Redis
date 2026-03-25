#include "./server.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <sys/select.h>

Server::Server(int port) : port(port) {}

void Server::start() {
    server_socket.createSocket();
    server_socket.bindSocket(port);
    server_socket.listenSocket();

    std::cout << "Waiting for a client to connect...\n";
    std::cout << "Logs from your program will appear here!\n";
}

void Server::run() {
    // int client_fd = server_socket.acceptClient();
    // std::cout << "Client connected\n";

    // char buffer[1024];

    // while (true) {
    //     memset(buffer, 0, sizeof(buffer));

    //     int bytes_read = read(client_fd, buffer, sizeof(buffer));

    //     if (bytes_read <= 0) {
    //         std::cout << "Client disconnected\n";
    //         break;
    //     }

    //     std::cout << "Received: " << buffer << std::endl;

    //     const char *response = "+PONG\r\n";
    //     send(client_fd, response, strlen(response), 0);
    // }

    // close(client_fd);

    std::vector<int> clients;
    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        FD_SET(server_socket.getServerFd(), &read_fds);
        int max_fd = server_socket.getServerFd();

        for (int c : clients) {
            FD_SET(c, &read_fds);
            if(c > max_fd) max_fd = c;
        }

        select(max_fd+1, &read_fds, nullptr, nullptr, nullptr);

        // new client connection
        if(FD_ISSET(server_socket.getServerFd(), &read_fds)) {
            int client_fd = server_socket.acceptClient();
            clients.push_back(client_fd);
            std::cout << "New Client connected\n";
        }

        int n = clients.size();
        // existing client messages
        for (int i = 0; i < n; i++) {
            int client_fd = clients[i];

            if(FD_ISSET(client_fd, &read_fds)) {
                char buffer[1024];

                if (read(client_fd, buffer, 1024) <= 0) {
                    close(client_fd);
                    clients.erase(clients.begin() + i);
                    i--;
                    n--;
                } else {
                    send(client_fd, "+PONG\r\n", 7, 0);
                }
            }
        }
        
    }
}

Server::~Server() {}