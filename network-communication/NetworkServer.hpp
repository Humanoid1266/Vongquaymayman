#pragma once
#include "NetworkConnection.hpp"
#include <vector>

class TCPServer {
private:
    int server_fd;
    sockaddr_in address;
    std::vector<int> client_sockets;
    
public:
    void startServer(int port) {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw NetworkException("Server socket creation failed");
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        
        if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0)
            throw NetworkException("Bind failed");
        
        if (listen(server_fd, 5) < 0)
            throw NetworkException("Listen failed");
    }
    
    int acceptClient() {
        sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &addrlen);
        if (client_fd < 0) throw NetworkException("Accept failed");
        client_sockets.push_back(client_fd);
        return client_fd;
    }
    
    void sendToClient(int client_fd, const std::string& data) const {
        send(client_fd, data.c_str(), data.size(), 0);
    }
    
    std::string receiveFromClient(int client_fd) const {
        char buffer[1024];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes < 0) throw NetworkException("Receive from client failed");
        buffer[bytes] = '\0';
        return std::string(buffer);
    }
    
    void stopServer() {
        for (int sock : client_sockets) close(sock);
        close(server_fd);
    }
    
    ~TCPServer() {
        if (server_fd >= 0) stopServer();
    }
};