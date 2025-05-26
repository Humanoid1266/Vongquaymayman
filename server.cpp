// server.cpp
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")

#define PORT 2808

DWORD WINAPI HandleClient(LPVOID lpParam) {
    SOCKET clientSocket = *(SOCKET*)lpParam;
    char buffer[1024];
    srand((unsigned)time(0));

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        std::cout << "Client gui: " << buffer << std::endl;

        if (strcmp(buffer, "SPIN") == 0) {
            int result = rand() % 8 + 1; // Từ 1 đến 8
            std::string message = "Ban trung giai #" + std::to_string(result);
            send(clientSocket, message.c_str(), message.length(), 0);
        }
    }

    closesocket(clientSocket);
    return 0;
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);

    std::cout << "Server dang lang nghe ket noi..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        std::cout << "Client ket noi thanh cong." << std::endl;
        CreateThread(nullptr, 0, HandleClient, &clientSocket, 0, nullptr);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
