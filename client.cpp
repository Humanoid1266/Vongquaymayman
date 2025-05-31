#include <iostream>
#include <SFML/Graphics.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cmath>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define PORT 2808

const int NUM_REWARDS = 8;
std::string rewards[NUM_REWARDS] = {
    "Giai #1", "Giai #2", "Giai #3", "Giai #4",
    "Giai #5", "Giai #6", "Giai #7", "Giai #8"
};

float degToRad(float degrees) {
    return degrees * 3.14159265f / 180.f;
}

int main() {
    // Socket Init
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        std::cerr << "Khong the khoi tao Winsock\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Tao socket that bai\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Khong ket noi duoc server\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // SFML Init
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");
    sf::Font font;
    if (!font.loadFromFile("OpenSans_Condensed-Bold.ttf")) {
        std::cerr << "Khong tim thay file font!\n";
        return 1;
    }

    sf::Text resultText("", font, 28);
    resultText.setFillColor(sf::Color::Blue);
    resultText.setPosition(220, 30);

    sf::RectangleShape pointer(sf::Vector2f(20, 60));
    pointer.setFillColor(sf::Color::Red);
    pointer.setPosition(390, 80);

    sf::RectangleShape spinButton(sf::Vector2f(120, 50));
    spinButton.setFillColor(sf::Color::Green);
    spinButton.setPosition(340, 520);

    sf::Text spinText("SPIN", font, 20);
    spinText.setFillColor(sf::Color::White);
    spinText.setPosition(370, 530);

    sf::CircleShape wheel(200, NUM_REWARDS);
    wheel.setOrigin(200, 200);
    wheel.setPosition(400, 300);

    std::vector<sf::Text> rewardTexts;
    for (int i = 0; i < NUM_REWARDS; ++i) {
        sf::Text text(rewards[i], font, 16);
        float angle = 360.f / NUM_REWARDS * i;
        float rad = degToRad(angle);
        float x = 400 + std::cos(rad) * 140 - text.getGlobalBounds().width / 2;
        float y = 300 + std::sin(rad) * 140 - text.getGlobalBounds().height / 2;
        text.setPosition(x, y);
        text.setRotation(angle + 90); // làm chữ xoay theo góc
        text.setFillColor(sf::Color::Black);
        rewardTexts.push_back(text);
    }

    bool isSpinning = false;
    float angle = 0;
    float targetAngle = 0;
    sf::Clock spinClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (!isSpinning && spinButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    // gửi yêu cầu SPIN
                    const char* msg = "SPIN";
                    send(clientSocket, msg, strlen(msg), 0);

                    // nhận kết quả
                    char buffer[1024];
                    int received = recv(clientSocket, buffer, 1023, 0);
                    if (received > 0) {
                        buffer[received] = '\0';
                        resultText.setString(buffer);

                        // Lấy số từ kết quả (ví dụ "Ban trung giai #5")
                        int resultNum = buffer[strlen(buffer) - 1] - '1';
                        if (resultNum >= 0 && resultNum < NUM_REWARDS) {
                            targetAngle = 360 * 3 + 360.f / NUM_REWARDS * resultNum + 360.f / (2 * NUM_REWARDS); // random giữa góc
                            spinClock.restart();
                            isSpinning = true;
                        }
                    }
                }
            }
        }

        if (isSpinning) {
            float t = spinClock.getElapsedTime().asSeconds();
            if (t < 3.f) {
                angle = angle + (targetAngle - angle) * 0.05f;  // ease out
                wheel.setRotation(angle);
            } else {
                isSpinning = false;
                angle = targetAngle;
                wheel.setRotation(angle);
            }
        }

        window.clear(sf::Color::White);
        window.draw(wheel);
        for (const auto& text : rewardTexts)
            window.draw(text);
        window.draw(pointer);
        window.draw(spinButton);
        window.draw(spinText);
        window.draw(resultText);
        window.display();
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
