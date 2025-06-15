#include <iostream>
#include <winsock2.h>
#include <random>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#define PORT 2808

using namespace std;

// Hàm xử lý mỗi client trong một luồng riêng
void handleClient(SOCKET clientSocket, string (*processRequest)(const string&, const RewardConfig&, mt19937&), RewardConfig config) {
    char buffer[1024];
    random_device rd;
    mt19937 gen(rd()); // Sửa lỗi bằng cách thêm ()

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cerr << "Client ngat ket noi hoac loi: " << WSAGetLastError() << endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        cout << "Du lieu nhan tu client: " << buffer << endl;

        string request(buffer);
        string response = processRequest(request, config, gen);
        int score = (response.find("RESULT:") != string::npos) ? stoi(response.substr(7)) : 0;
        sendResult(clientSocket, score); // Sử dụng hàm từ network-communication
    }

    closesocket(clientSocket);
}