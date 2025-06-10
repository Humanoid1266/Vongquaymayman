# 🎯 Đề tài: Vòng Quay May Mắn - Mô hình Multi Client-Server sử dụng Winsock

## 📌 Mô tả đề tài

Đây là một ứng dụng mô phỏng **trò chơi vòng quay may mắn** được xây dựng dựa trên mô hình **Client-Server nhiều người dùng (multi client-server)** sử dụng thư viện **Winsock** trong ngôn ngữ lập trình C++.

Người chơi (client) có thể kết nối đến máy chủ (server) và thực hiện thao tác "quay số" để nhận được kết quả ngẫu nhiên từ vòng quay. Server sẽ xử lý kết quả quay và gửi phản hồi tương ứng đến từng client.

---

## ⚙️ Công nghệ sử dụng

- **Ngôn ngữ lập trình**: C++
- **Thư viện mạng**: Winsock API (Windows Socket)
- **Giao diện đồ họa (ở client)**: SFML (Simple and Fast Multimedia Library)
- **Mô hình mạng**: TCP Socket, hỗ trợ nhiều client đồng thời

---

## 🧩 Kiến trúc chương trình

### Server:
- Khởi tạo Winsock và lắng nghe trên một cổng cố định.
- Hỗ trợ nhiều client kết nối cùng lúc bằng đa luồng (`std::thread` hoặc `CreateThread`).
- Xử lý yêu cầu quay số từ client và gửi lại kết quả.

### Client:
- Kết nối đến địa chỉ IP và port của server.
- Gửi yêu cầu quay số (khi người dùng nhấn nút "Quay").
- Nhận và hiển thị kết quả từ server (có thể tích hợp giao diện SFML cho vòng quay trực quan).

---

## 📥 Cách chạy chương trình

###  Build và chạy Server
```bash
g++ main.cpp -o <____> -lws2_32 -lsfml-graphics -lsfml-window -lsfml-system
./<___>.exe


✅ Tính năng
Kết nối nhiều client đồng thời

Mỗi client gửi yêu cầu quay số độc lập

Server xử lý và gửi kết quả quay ngẫu nhiên

 🧠 Hướng phát triển
Giao diện trực quan hơn cho vòng quay

Hệ thống thưởng, lưu lịch sử quay

Giao tiếp bằng giao thức tùy chỉnh

Hỗ trợ UDP hoặc WebSocket