#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

// Gửi yêu cầu quay vòng (client gọi)
bool sendSpinRequest(SOCKET sock) {
    const char *msg = "SPIN";
    int ret = send(sock, msg, strlen(msg) + 1, 0);
    return (ret != SOCKET_ERROR);
}

// Nhận kết quả điểm từ server (client gọi)
int receiveResult(SOCKET sock) {
    char buf[16] = "";
    int bytes = recv(sock, buf, sizeof(buf), 0);
    if (bytes > 0) {
        return atoi(buf);
    }
    return -1;
}

// Nhận yêu cầu từ client (server gọi)
string receiveRequest(SOCKET clientSock) {
    char buf[16] = "";
    int bytes = recv(clientSock, buf, sizeof(buf), 0);
    if (bytes > 0) return string(buf, bytes);
    return "";
}

// Gửi kết quả điểm cho client (server gọi)
bool sendResult(SOCKET clientSock, int score) {
    char buf[16];
    sprintf(buf, "%d", score);
    int ret = send(clientSock, buf, strlen(buf) + 1, 0);
    return (ret != SOCKET_ERROR);
}
