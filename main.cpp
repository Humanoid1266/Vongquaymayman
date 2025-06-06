#include <iostream>
#include <thread>
#include <winsock2.h>

#include "multi-client/multi-client.cpp"
#include "Server/server.cpp"
#include "runInterface/interface.cpp"
#include "network-communication/network-communication.cpp"

#pragma comment(lib, "ws2_32.lib")

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
    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        thread clientThread([clientSocket]() {
            string request = receiveRequest(clientSocket);
            string response = processSpinRequest(request);
            int score = 0;

            if (response.find("RESULT:") != string::npos) {
                score = stoi(response.substr(7));
            }
            sendResult(clientSocket, score);
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
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // localhost

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Khong ket noi duoc den server.\n";
        return;
    }

    if (sendSpinRequest(sock)) {
        int score = receiveResult(sock);
        cout << "Nhan ket qua tu server: " << score << endl;
    } else {
        cout << "Loi khi gui SPIN\n";
    }

    closesocket(sock);
    WSACleanup();
}

int main() {
    cout << "Chon che do:\n1. Server\n2. Client\n3. Giao dien (SFML)\n> ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        runServer();
    } else if (choice == 2) {
        runClient();
    } else if (choice == 3) {
        runInterface(); // Chạy giao diện quay
    } else {
        cout << "Lua chon khong hop le!\n";
    }

    return 0;
}
