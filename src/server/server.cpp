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
    std::vector<int> clients; // stores all connected client socket file descriptors

    while (true) {
        fd_set read_fds;    // set of file descriptors to monitor for READ events
        FD_ZERO(&read_fds); // clear the set (initialize empty set)

        // FD_SET(fd, fd_set*)
        // Adds a file descriptor into the set so select() can monitor it
        FD_SET(server_socket.getServerFd(), &read_fds);

        // max_fd stores highest file descriptor value
        int max_fd = server_socket.getServerFd();

        // Add all client sockets into read_fds
        for (int c : clients) {
            FD_SET(c, &read_fds); // add client socket to read set
            if (c > max_fd)
                max_fd = c; // update max fd
        }

        // select(nfds, readfds, writefds, errorfds, timeout)
        // nfds     -> highest fd + 1
        // readfds  -> sockets to monitor for incoming data
        // writefds -> sockets ready for writing (not used here)
        // errorfds -> sockets with errors (not used)
        // timeout  -> how long to wait (nullptr = wait forever)
        select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);

        // ---------- NEW CLIENT CONNECTION ----------
        // FD_ISSET(fd, fd_set*)
        // Checks whether this fd became READY after select()
        if (FD_ISSET(server_socket.getServerFd(), &read_fds)) {
            // accept() accepts new client connection
            int client_fd = server_socket.acceptClient();
            clients.push_back(client_fd); // store client socket
            std::cout << "New Client connected\n";
        }

        int n = clients.size();

        // ---------- EXISTING CLIENT MESSAGES ----------
        for (int i = 0; i < n; i++) {
            int client_fd = clients[i];

            // Check if this client sent data
            if (FD_ISSET(client_fd, &read_fds)) {
                char buffer[1024];

                // read(fd, buffer, bytes)
                // fd     -> socket to read from
                // buffer -> where data will be stored
                // bytes  -> max bytes to read
                // return -> number of bytes read
                if (read(client_fd, buffer, 1024) <= 0) {
                    // client disconnected or error
                    close(client_fd);                   // close socket
                    clients.erase(clients.begin() + i); // remove from list
                    i--;                                // adjust index after erase
                    n--;                                // reduce total clients count
                }
                else {
                    // send(fd, data, bytes, flags)
                    // fd    -> socket to send to
                    // data  -> message
                    // bytes -> number of bytes to send
                    // flags -> usually 0
                    send(client_fd, "+PONG\r\n", strlen("+PONG\r\n"), 0);
                }
            }
        }
    }
}

Server::~Server() {}