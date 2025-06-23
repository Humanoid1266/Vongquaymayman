const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const fs = require('fs');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Cấu hình phần thưởng (mock nếu chưa có reward_config.json)
const rewards = [
    { id: 1, name: "100$", probability: 0.3 },
    { id: 2, name: "200$", probability: 0.25 },
    { id: 3, name: "500$", probability: 0.2 },
    { id: 4, name: "1000$", probability: 0.15 },
    { id: 5, name: "Chúc may mắn", probability: 0.1 }
];

// Thống kê game (lưu trên server)
let gameStats = {
    totalSpins: 0,
    playersOnline: 0,
    rewardStats: {},
    dailyStats: {},
    spinHistory: [] // Thêm lịch sử toàn cục
};

// Khởi tạo thống kê phần thưởng
rewards.forEach(reward => {
    gameStats.rewardStats[reward.id] = 0;
});

function saveStats() {
    try {
        fs.writeFileSync('./game_stats.json', JSON.stringify(gameStats, null, 2));
        console.log('📝 Thống kê đã được lưu lúc', new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }));
    } catch (error) {
        console.error('❌ Lỗi lưu thống kê:', error);
    }
}

function loadStats() {
    try {
        if (fs.existsSync('./game_stats.json')) {
            const data = fs.readFileSync('./game_stats.json', 'utf8');
            const loadedStats = JSON.parse(data);
            gameStats.totalSpins = loadedStats.totalSpins || 0;
            gameStats.playersOnline = loadedStats.playersOnline || 0;
            gameStats.rewardStats = { ...gameStats.rewardStats, ...loadedStats.rewardStats };
            gameStats.dailyStats = loadedStats.dailyStats || {};
            gameStats.spinHistory = loadedStats.spinHistory || [];
        } else {
            console.log('ℹ️ Không tìm thấy game_stats.json, khởi tạo mới.');
        }
    } catch (error) {
        console.error('❌ Lỗi đọc thống kê:', error);
    }
}

loadStats();

