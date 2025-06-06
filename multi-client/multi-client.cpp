#include <iostream>
#include <winsock2.h>
#include <random>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#define PORT 2808 
using namespace std;

// Hàm xử lý mỗi client trong một luồng riêng
void handleClient(SOCKET clientSocket) {
    char buffer[1024];

    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cerr << "Client ngắt kết nối hoặc lỗi: " << WSAGetLastError() << endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        cout << "Dữ liệu nhận từ client: " << buffer << endl;

 
        const char* msg = "Server da nhan thong diep!";
        send(clientSocket, msg, strlen(msg) + 1, 0);
    }

    closesocket(clientSocket);
}