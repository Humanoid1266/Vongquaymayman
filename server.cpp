#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")

#define PORT 2808

DWORD WINAPI HandleClient(LPVOID lpParam) {
    SOCKET clientSocket = *(SOCKET*)lpParam;
    delete (SOCKET*)lpParam; // Giải phóng bộ nhớ cấp phát động
    char buffer[1024];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;

        buffer[bytesReceived] = '\0';
        std::cout << "Client gui: " << buffer << std::endl;

        if (strcmp(buffer, "SPIN") == 0) {
            int result = rand() % 8 + 1; // Từ 1 đến 8
            std::string message = "Ban trung giai #" + std::to_string(result);
            send(clientSocket, message.c_str(), (int)message.length(), 0);
        }
    }

    closesocket(clientSocket);
    std::cout << "Client da ngat ket noi." << std::endl;
    return 0;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Khong khoi dong duoc Winsock." << std::endl;
        return 1;
    }

    srand((unsigned)time(0)); // Khởi tạo random 1 lần

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Tao socket that bai." << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind that bai." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen that bai." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server dang lang nghe ket noi..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept that bai." << std::endl;
            continue;
        }

        std::cout << "Client ket noi thanh cong." << std::endl;

        SOCKET* pClientSocket = new SOCKET(clientSocket);
        HANDLE hThread = CreateThread(nullptr, 0, HandleClient, pClientSocket, 0, nullptr);
        if (hThread == nullptr) {
            std::cerr << "Tao thread that bai." << std::endl;
            closesocket(clientSocket);
            delete pClientSocket;
        } else {
            CloseHandle(hThread); // Đóng handle thread ngay, thread vẫn chạy
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
