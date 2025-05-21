#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 2808
#define SERVER_ADDR "127.0.0.1"

using namespace std;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Khoi tao Winsock that bai: " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Khong the tao socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr); // Sử dụng inet_pton thay inet_addr
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ket noi den server that bai: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Ket noi den server thanh cong!" << endl;

    const char* message = "Xin chao tu client!";
    if (send(clientSocket, message, static_cast<int>(strlen(message)), 0) == SOCKET_ERROR) {
        cerr << "Gui du lieu that bai: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, 1023, 0); // Giảm 1 byte để chừa chỗ cho '\0'
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Du lieu nhan duoc tu server: " << buffer << endl;
    }
    else {
        cerr << "Khong nhan duoc du lieu: " << WSAGetLastError() << endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}


#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // SFML window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");
    // wheel static
    sf::circleShape wheel(200);
    wheel.setFillColor(sf::yellow::green);
    wheel.setPosition(300, 200);
    //spin button
    sf::Reactangleshape spinbutton(sf::vector2f(100, 50));
    spinbutton.setFillColor(sf::Color::Red);
    spinbutton.setPosition(350, 500);
}