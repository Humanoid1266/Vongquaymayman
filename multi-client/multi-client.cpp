#include <iostream>
#include <winsock2.h>
#include <random>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#define PORT 2808 
using namespace std;

while (true) {
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Chap nhan ket noi khong thanh cong" << WSAGetLastError() << endl;
        continue;
    }
    cout << "Client moi ket noi!" << endl;
    clientThreads.emplace_back(thread(handleClient, clientSocket));
}
