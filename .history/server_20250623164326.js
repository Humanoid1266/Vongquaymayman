#include <iostream>
#include <thread>
#include <winsock2.h>
#include "runInterface/interface.cpp"
#include "Server/server.cpp"
#include "network-communication/network-communication.cpp"
#include "multi-client/multi-client.cpp"

#pragma comment(lib, "ws2_32.lib")
#define PORT 2808

using namespace std;

void runServer() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    cout << "Server dang cho ket noi...\n";

    // Tạo cấu hình phần thưởng dùng chung
    RewardConfig config;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        thread clientThread([clientSocket, config]() mutable {
            random_device rd;
            mt19937 gen(rd());

            while (true) {
                string request = receiveRequest(clientSocket);
                if (request.empty()) {
                    cout << "[SERVER] Client ngat ket noi.\n";
                    break;
                }

                string response = processSpinRequest(request, config, gen);
                int score = (response.find("RESULT:") != string::npos) ? stoi(response.substr(7)) : 0;
                cout << "[SERVER] Client da quay, ket qua: " << score << endl;
                sendResult(clientSocket, score);
            }

            closesocket(clientSocket);
        });
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

void runClient() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Khong ket noi duoc den server.\n";
        closesocket(sock);
        WSACleanup();
        return;
    }

    // Gọi runInterface với SOCKET để xử lý giao tiếp mạng
    runInterface(sock);

    closesocket(sock);
    WSACleanup();
}

int main() {
    cout << "Chon che do:\n1. Server\n2. Client\n> ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        runServer();
    } else if (choice == 2) {
        runClient();
    } else {
        cout << "Lua chon khong hop le!\n";
    }

    return 0;
}