#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SFML/Graphics.hpp>

#pragma comment(lib, "ws2_32.lib")
#define PORT 2808
#define SERVER_ADDR "127.0.0.1"

using namespace std;

int main() {
    // Khởi tạo Winsock
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
    inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
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
    int bytesReceived = recv(clientSocket, buffer, 1023, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Du lieu nhan duoc tu server: " << buffer << endl;
    } else {
        cerr << "Khong nhan duoc du lieu: " << WSAGetLastError() << endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    // SFML Giao diện 
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");

    // Hình tròn đại diện vòng quay
    sf::CircleShape wheel(200);
    wheel.setFillColor(sf::Color::Green);
    wheel.setPosition(300, 100); 

    // Nút "Spin"
    sf::RectangleShape spinButton(sf::Vector2f(100, 50));
    spinButton.setFillColor(sf::Color::Red);
    spinButton.setPosition(350, 500);

     while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (spinButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    const char* spinMessage = "SPIN";
                    send(clientSocket, spinMessage, 4, 0);
                    cout << "Da gui yeu cau SPIN!" << endl;

                    // Nhận kết quả từ server
                    char resultBuffer[1024];
                    int resultBytes = recv(clientSocket, resultBuffer, 1023, 0);
                    if (resultBytes > 0) {
                        resultBuffer[resultBytes] = '\0';
                        cout << "Ket qua tu server: " << resultBuffer << endl;
                    } else {
                        cerr << "Khong nhan duoc ket qua: " << WSAGetLastError() << endl;
                    }
                }
            }
        }

        window.clear(sf::Color::White);
        window.draw(wheel);
        window.draw(spinButton);
        window.display();
    }

    return 0;
}
