#pragma once
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

class NetworkException : public std::runtime_error {
public:
    NetworkException(const std::string& msg) : std::runtime_error(msg) {}
};

class TCPConnection {
private:
    int sockfd;
    sockaddr_in address;
    bool is_connected = false;
    
public:
    TCPConnection() : sockfd(-1) {}
    
    void connectToServer(const std::string& ip, int port) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw NetworkException("Socket creation failed");
        
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        
        if (inet_pton(AF_INET, ip.c_str(), &address.sin_addr) <= 0)
            throw NetworkException("Invalid address format");
        
        if (connect(sockfd, (sockaddr*)&address, sizeof(address)) < 0)
            throw NetworkException("Connection failed");
        
        is_connected = true;
    }
    
    void sendData(const std::string& data) const {
        if (!is_connected) throw NetworkException("Not connected");
        size_t sent = 0;
        while (sent < data.size()) {
            ssize_t bytes = send(sockfd, data.c_str() + sent, data.size() - sent, 0);
            if (bytes < 0) throw NetworkException("Send failed");
            sent += bytes;
        }
    }
    
    std::string receiveData() const {
        if (!is_connected) throw NetworkException("Not connected");
        char buffer[1024];
        ssize_t bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes < 0) throw NetworkException("Receive failed");
        buffer[bytes] = '\0';
        return std::string(buffer);
    }
    
    void closeConnection() {
        if (sockfd >= 0) close(sockfd);
        is_connected = false;
    }
    
    ~TCPConnection() {
        if (is_connected) closeConnection();
    }
};