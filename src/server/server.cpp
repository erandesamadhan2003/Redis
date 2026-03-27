#include "./server.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <sys/select.h>

Server::Server(int port) : port(port) {
    store = DataStore();
    commandHandler = CommandHandler(&store);
}

void Server::start() {
    server_socket.createSocket();
    server_socket.bindSocket(port);
    server_socket.listenSocket();

    std::cout << "Waiting for a client to connect...\n";
    std::cout << "Logs from your program will appear here!\n";
}

void Server::run() {
    std::vector<int> clients; 
    
    while (true) {
        fd_set read_fds;
        FD_ZERO(&read_fds);

        FD_SET(server_socket.getServerFd(), &read_fds);

        int max_fd = server_socket.getServerFd();

        for (int c : clients) {
            FD_SET(c, &read_fds);
            if (c > max_fd)
                max_fd = c;
        }

        select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);

        if (FD_ISSET(server_socket.getServerFd(), &read_fds)) {
            int client_fd = server_socket.acceptClient();
            clients.push_back(client_fd);
            std::cout << "New Client connected\n";
        }

        int n = clients.size();

        for (int i = 0; i < n; i++) {
            int client_fd = clients[i];

            if (FD_ISSET(client_fd, &read_fds)) {
                char buffer[1024];

                if (read(client_fd, buffer, 1024) <= 0) {
                    close(client_fd);
                    clients.erase(clients.begin() + i);
                    i--;
                    n--;
                }
                else {
                    std::string response = commandHandler.handleCommand(std::string(buffer));
                    send(client_fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }
}

Server::~Server() {}

/* ============================================================================
  FUNCTION SYNTAX AND CODE EXPLANATION
  ============================================================================
 
  1. CONSTRUCTOR: Server::Server(int port) : port(port)
     Syntax: ClassName::ClassName(parameters) : member(initializer)
     - Initializes the server with a port number
     - Uses member initializer list to set port value
     - Creates a CommandHandler object for processing commands
 
  2. fd_set AND FD_* MACROS
     - fd_set: Structure representing a set of file descriptors to monitor
     - FD_ZERO(fd_set*): Clears/initializes the set to empty
     - FD_SET(fd, fd_set*): Adds a file descriptor to the set
     - FD_ISSET(fd, fd_set*): Checks if fd is ready after select()
 
  3. select() FUNCTION
     Syntax: select(int nfds, fd_set* readfds, fd_set* writefds, 
                    fd_set* errorfds, struct timeval* timeout)
     
     Parameters:
     - nfds: Highest file descriptor value + 1
     - readfds: Set of sockets to monitor for incoming data
     - writefds: Set of sockets to monitor for write readiness
     - errorfds: Set of sockets to monitor for exceptions
     - timeout: How long to wait (nullptr = wait indefinitely)
     
     Returns: Number of ready file descriptors
     - Blocks until at least one monitored socket has activity
 
  4. ACCEPTING CLIENTS
     Syntax: int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
     - Accepts a new client connection on the listening socket
     - Returns a new socket file descriptor for communication with the client
     - Original listening socket continues to accept more connections
 
  5. read() FUNCTION
     Syntax: ssize_t read(int fd, void* buf, size_t count)
     Parameters:
     - fd: File descriptor to read from
     - buf: Buffer to store read data
     - count: Maximum number of bytes to read
     Returns:
     - Positive number: Bytes actually read
     - 0: Client closed connection (EOF)
     - -1: Error occurred
 
  6. send() FUNCTION
     Syntax: ssize_t send(int sockfd, const void* buf, size_t len, int flags)
     Parameters:
     - sockfd: File descriptor to send to
     - buf: Data to send
     - len: Number of bytes to send
     - flags: Usually 0 for standard operation
     Returns: Number of bytes sent, or -1 on error
 
  7. close() FUNCTION
     Syntax: int close(int fd)
     - Closes a file descriptor, freeing system resources
     - Returns 0 on success, -1 on error
 
  8. VECTOR OPERATIONS
     - push_back(value): Adds element to end of vector
     - erase(position): Removes element at specified position
     - size(): Returns number of elements
     - begin() + i: Iterator to element at index i
 
  9. LOOP AND INDEX MANAGEMENT
     When erasing elements from vector while iterating:
     - i--: Adjust index to account for erased element
     - n--: Decrease total count to prevent out-of-bounds access
 
  10. SERVER ARCHITECTURE OVERVIEW
      - Uses single-threaded I/O multiplexing with select()
      - Non-blocking client handling without threads
      - Server socket monitors for new connections
      - Client sockets monitor for incoming messages
      - Each client is handled sequentially in the main loop
 
  11. KEY ADVANTAGES OF THIS APPROACH
      - Efficient for moderate number of clients
      - No thread synchronization overhead
      - Predictable execution flow
      - Cross-platform compatible (POSIX)
 
  12. POTENTIAL LIMITATIONS
      - FD_SETSIZE limit (typically 1024) restricts maximum clients
      - Linear scan of all clients each iteration
      - Blocking operations may affect responsiveness
      - Single-threaded limits CPU utilization
 
  ============================================================================
 */