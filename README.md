# 🎰 Vòng Quay May Mắn - Mô hình Multi Client-Server sử dụng WebSocket

## 🌟 Mô tả đề tài

Đây là một ứng dụng mô phỏng trò chơi vòng quay may mắn được xây dựng dựa trên mô hình Client-Server nhiều người dùng, sử dụng WebSocket để giao tiếp.

Ứng dụng được phát triển với **HTML, CSS, JavaScript (client)** kết hợp **Node.js + thư viện ws (server)**, mang đến trải nghiệm mượt mà và tương tác.

> Người chơi kết nối qua trình duyệt, nhấn nút quay để nhận kết quả ngẫu nhiên từ các phần thưởng như 💰 100$, 200$, 500$, 1000$, hoặc 🎈 Chúc may mắn.  
> Server xử lý yêu cầu và trả về kết quả tức thì với hiệu ứng sinh động.

---

## 🛠️ Công nghệ sử dụng

### Frontend:
- Ngôn ngữ: **HTML5, CSS3, JavaScript**
- Thư viện: 🌐 **WebSocket API**, 🎨 **Canvas API**, 🔊 **Web Audio API**

### Backend:
- Ngôn ngữ: **Node.js**
- Thư viện: 🚀 `Express.js` (phục vụ file tĩnh), `ws` (WebSocket)

### Lưu trữ:
- 💾 `LocalStorage` (lịch sử quay cục bộ)
- 📁 `JSON file` (thống kê game phía server)

> Mô hình mạng: WebSocket (TCP-based), hỗ trợ nhiều client đồng thời.

---

## 🏗️ Kiến trúc chương trình

### 🔧 Server (`server.js`)
- Khởi tạo server Express và WebSocket trên cổng mặc định `2307`.
- Quản lý nhiều client đồng thời qua WebSocket.
- Xử lý yêu cầu quay, tạo kết quả ngẫu nhiên dựa trên xác suất và gửi về client.
- Lưu trữ thống kê (tổng lượt quay, số người chơi, lịch sử) trong file `game_stats.json`.

### 🖥️ Client (`client.js`, `index.html`, `style.css`)
- Kết nối đến server qua WebSocket `ws://<server-host>:<port>`.
- Gửi yêu cầu quay khi nhấn nút **QUAY NGAY! 🎉**
- Hiển thị vòng quay bằng **Canvas API**, hiệu ứng 🎊 *confetti* và âm thanh 🔊.
- Cập nhật lịch sử quay, thống kê, và trạng thái kết nối thời gian thực.

---

## 🚀 Cách chạy chương trình

### 📋 Yêu cầu:
- **Node.js**: Phiên bản 14 trở lên
- **Trình duyệt**: Chrome, Firefox (hỗ trợ WebSocket và Canvas)
- **npm**: Quản lý phụ thuộc

### 📡 Truy cập ứng dụng:
- Mở trình duyệt tại: `http://localhost:2307`
- Hoặc sử dụng cổng tùy chỉnh nếu cấu hình qua biến môi trường `PORT`

---

## ✅ Tính năng

- 🌍 **Kết nối nhiều client**: Hỗ trợ hàng loạt người chơi cùng lúc.
- 🎲 **Quay số độc lập**: Mỗi client nhận kết quả riêng biệt.
- 🎰 **Kết quả ngẫu nhiên**: Server tạo phần thưởng dựa trên xác suất.
- 🖼️ **Giao diện sinh động**: Vòng quay Canvas, hiệu ứng 🎉 *confetti*, âm thanh 🔊, và thông báo.
- 📊 **Thống kê thời gian thực**: Tổng lượt quay, số người chơi online, lịch sử quay.
- ⏳ **Hạn chế tốc độ quay**: Giới hạn quay mỗi 3 giây.
- 💾 **Lưu trữ lịch sử**: Lưu cục bộ (client) và toàn cục (server).
- 📱 **Responsive**: Tương thích máy tính và thiết bị di động.

---

## 🚀 Hướng phát triển

- 🎨 **Giao diện nâng cao**: Thêm hoạt hình 3D hoặc hiệu ứng phức tạp.
- 🏆 **Hệ thống thưởng**: Tích hợp điểm số, phần thưởng thực tế, bảng xếp hạng.
- 🗄️ **Lưu trữ nâng cao**: Sử dụng MongoDB hoặc MySQL để quản lý dữ liệu lịch sử.
- 📡 **Tối ưu giao thức**: Tùy chỉnh dữ liệu truyền qua WebSocket.
- 🔒 **Bảo mật**: Hỗ trợ `wss://`, xác thực người dùng.
- 🎮 **Tính năng mới**: Thêm mini-game hoặc chế độ quay đặc biệt.

---

## 📂 Cấu trúc dự án

```txt
lucky-wheel/
├── public/
│   ├── client.js         # 🎮 Logic client (WebSocket, Canvas, UI)
│   ├── style.css         # 🎨 CSS cho giao diện và hiệu ứng
│   └── index.html        # 🖼️ Giao diện chính
├── server.js             # 🛠️ Server Express và WebSocket
├── game_stats.json       # 💾 Thống kê game (tự động tạo)
├── package.json          # 📦 Quản lý phụ thuộc npm
└── README.md             # 📜 Tài liệu hướng dẫn
