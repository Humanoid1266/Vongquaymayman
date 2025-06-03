# Network Communication Module

## Mô tả
Module này xử lý giao tiếp mạng cho trò chơi "Vòng quay may mắn" sử dụng TCP sockets. Bao gồm:
1. "TCPConnection": Kết nối client tới server
2. "TCPServer": Quản lý kết nối từ phía server

## Thư viện cần thiết
- Hệ thống Unix/Linux (sử dụng thư viện socket POSIX)
- C++11 trở lên

## Cách sử dụng

### Client Side
```cpp
#include "NetworkConnection.hpp"

TCPConnection conn;
try {
    conn.connectToServer("127.0.0.1", 8080);
    conn.sendData("Hello Server!");
    std::string response = conn.receiveData();
} catch (const NetworkException& e) {
    // Xử lý lỗi
}


### Giải thích
1. **TCPConnection**: 
   - Kết nối client tới server qua TCP
   - Xử lý gửi/nhận dữ liệu
   - Bọc lỗi mạng trong NetworkException

2. **TCPServer**:
   - Khởi tạo server socket
   - Chấp nhận kết nối từ client
   - Quản lý nhiều client sockets (chuẩn bị cho branch 4)
   - Gửi/nhận dữ liệu với client cụ thể

3. **An toàn**:
   - Đóng socket tự động trong destructor
   - Kiểm tra trạng thái kết nối trước khi truyền nhận
   - Xử lý gói tin lớn bằng cơ chế gửi/nhận lặp

