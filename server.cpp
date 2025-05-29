#include <iostream>
#include <winsock2.h>
#include <random>
#pragma comment(lib, "ws2_32.lib")
#define port 2808 
using namespace std;

int generateReward() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 5);
    int rewards[] = {100, 200, 300, 0, 50, 150};
    return rewards[dis(gen)];
}

int main(){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ket noi WSAS that bai" << endl;
        return 1;
    }
    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Khong the tao socket" << WSAGetLastError()<< endl;
        WSACleanup();
        return 1;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Khong the bind socket" << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Khong the lang nghe socket" << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout<< "Server dang chay tren cong " << port << endl;

    SOCKET clientSocket=accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Chap nhan ket noi khong thanh cong" << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1024, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; 
        cout << "Du lieu nhan duoc: " << buffer << endl;
        send(clientSocket, "Da nhan du lieu", 2, 0);
    } else {
        cerr << "Khong nhan duoc du lieu" << WSAGetLastError() << endl;
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
