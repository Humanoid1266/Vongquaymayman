#include <iostream>
#include <winsock2.h>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define PORT 2808

const int sectorCount = 8;

int main() {
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Khởi tạo Winsock thất bại: " << WSAGetLastError() << "\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Tạo socket thất bại: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Không thể kết nối đến server: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Tạo cửa sổ SFML
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");
    window.setFramerateLimit(60); // Giới hạn FPS để mượt mà hơn

    // Font
    sf::Font font;
    if (!font.loadFromFile("OpenSans_Condensed-Bold.ttf")) {
        std::cerr << "Không tìm thấy file font\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Text kết quả
    sf::Text resultText("", font, 32);
    resultText.setPosition(250, 30);
    resultText.setFillColor(sf::Color::Blue);
    resultText.setStyle(sf::Text::Bold);

    // Mũi tên
    sf::ConvexShape pointer;
    pointer.setPointCount(3);
    pointer.setPoint(0, sf::Vector2f(0, 0));
    pointer.setPoint(1, sf::Vector2f(-20, 60));
    pointer.setPoint(2, sf::Vector2f(20, 60));
    pointer.setFillColor(sf::Color::Red);
    pointer.setPosition(400, 100); // Đặt ở trên vòng quay

    // Nút SPIN
    sf::RectangleShape spinButton(sf::Vector2f(120, 50));
    spinButton.setFillColor(sf::Color::Green);
    spinButton.setPosition(340, 520);

    sf::Text spinText("SPIN", font, 24);
    spinText.setFillColor(sf::Color::White);
    spinText.setPosition(360, 530);

    // Tạo vòng quay bằng CircleShape
    std::vector<sf::CircleShape> sectors;
    std::vector<sf::Text> sectorLabels;
    float centerX = 400.f, centerY = 300.f;
    float radius = 200.f;

    for (int i = 0; i < sectorCount; ++i) {
        sf::CircleShape sector;
        sector.setRadius(radius);
        sector.setPointCount(50); // Tăng số điểm để tạo hình tròn mượt
        sector.setFillColor(i % 2 == 0 ? sf::Color::Yellow : sf::Color(255, 180, 0));
        sector.setOrigin(radius, radius); // Đặt gốc ở tâm hình tròn
        sector.setPosition(centerX, centerY);
        sector.setRotation(360.f / sectorCount * i); // Xoay để tạo sector
        sectors.push_back(sector);

        // Thêm nhãn cho sector
        sf::Text label(std::to_string(i + 1), font, 20);
        float labelAngle = 2 * M_PI * (i + 0.5) / sectorCount; // Góc giữa sector
        float labelRadius = radius * 0.7; // Đặt nhãn gần tâm hơn
        label.setPosition(centerX + labelRadius * cos(labelAngle) - 10,
                         centerY + labelRadius * sin(labelAngle) - 10);
        label.setFillColor(sf::Color::Black);
        sectorLabels.push_back(label);
    }

    bool isSpinning = false;
    float angle = 0;
    sf::Clock spinClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {
                if (spinButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    if (!isSpinning) {
                        isSpinning = true;
                        spinClock.restart();
                        resultText.setString("");
                        std::cout << "Bắt đầu quay\n";
                    }
                }
            }
        }

        if (isSpinning) {
            float elapsed = spinClock.getElapsedTime().asSeconds();
            if (elapsed < 3.0f) {
                angle += 5.f; // Tốc độ quay
                for (auto& sector : sectors) {
                    sector.setRotation(sector.getRotation() + 5.f);
                }
                for (auto& label : sectorLabels) {
                    int index = std::stoi(label.getString().toAnsiString()) - 1;
                    float labelAngle = 2 * M_PI * (index + 0.5) / sectorCount + (angle * M_PI / 180);
                    float labelRadius = radius * 0.7;
                    label.setPosition(centerX + labelRadius * cos(labelAngle) - 10,
                                     centerY + labelRadius * sin(labelAngle) - 10);
                }
            } else {
                isSpinning = false;
                std::cout << "Dừng quay, gửi yêu cầu đến server\n";

                // Gửi yêu cầu SPIN đến server
                const char* msg = "SPIN";
                if (send(clientSocket, msg, (int)strlen(msg), 0) == SOCKET_ERROR) {
                    std::cerr << "Lỗi gửi dữ liệu: " << WSAGetLastError() << "\n";
                    resultText.setString("Lỗi gửi dữ liệu đến server");
                } else {
                    char buffer[1024];
                    int received = recv(clientSocket, buffer, 1023, 0);
                    if (received > 0) {
                        buffer[received] = '\0';
                        resultText.setString(buffer);
                        std::cout << "Nhận kết quả từ server: " << buffer << "\n";
                    } else {
                        resultText.setString("Lỗi nhận dữ liệu từ server");
                        std::cerr << "Lỗi nhận dữ liệu: " << WSAGetLastError() << "\n";
                    }
                }
            }
        }

        // Vẽ giao diện
        window.clear(sf::Color::White);

        // Vẽ vòng quay
        sf::CircleShape wheel(radius);
        wheel.setOrigin(radius, radius);
        wheel.setPosition(centerX, centerY);
        wheel.setFillColor(sf::Color::White);
        window.draw(wheel); // Vẽ nền trắng cho vòng quay

        for (const auto& sector : sectors) {
            window.draw(sector);
        }
        for (const auto& label : sectorLabels) {
            window.draw(label);
        }

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