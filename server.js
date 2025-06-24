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
function getRandomReward() {
    const rand = Math.random();
    let cumulativeProb = 0;
    for (const reward of rewards) {
        cumulativeProb += reward.probability;
        if (rand <= cumulativeProb) return reward;
    }
    return rewards[rewards.length - 1]; // fallback nếu có sai sót cộng dồn
}

function calculateWheelAngle(rewardId) {
    const index = rewardId - 1; // vì ID bắt đầu từ 1
    const segmentCount = 5; // tổng số phần thưởng
    const segmentAngle = 360 / segmentCount;
    const baseAngle = index * segmentAngle + segmentAngle / 2;
    const randomSpins = Math.floor(Math.random() * 3 + 5) * 360; // quay 5–7 vòng
    const randomOffset = Math.random() * 10 - 5; // lệch góc ngẫu nhiên ±5 độ
    return randomSpins + baseAngle + randomOffset;
}

const rateLimiter = new Map();

function isRateLimited(ws) {
    const now = Date.now();
    const lastSpin = rateLimiter.get(ws) || 0;
    const cooldown = 3000; // 3 giây cooldown
    if (now - lastSpin < cooldown) return true;
    rateLimiter.set(ws, now);
    return false;
}
function broadcast(data) {
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(data));
        }
    });
}

wss.on('connection', (ws) => {
    gameStats.playersOnline++;
    console.log(`✅ Người chơi mới kết nối. Online: ${gameStats.playersOnline} lúc`, new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }));

    ws.send(JSON.stringify({
        type: 'welcome',
        message: '🎮 Chào mừng đến với Vòng Quay May Mắn!',
        stats: {
            totalSpins: gameStats.totalSpins,
            playersOnline: gameStats.playersOnline
        }
    }));

    broadcast({
        type: 'players_update',
        playersOnline: gameStats.playersOnline
    });

    ws.on('message', (msg) => {
        try {
            const data = JSON.parse(msg);
            if (data.type === 'spin') {
                if (isRateLimited(ws)) {
                    ws.send(JSON.stringify({
                        type: 'error',
                        message: '⏰ Vui lòng đợi 3 giây trước khi quay tiếp!'
                    }));
                    return;
                }

                const result = getRandomReward();
                const angle = calculateWheelAngle(result.id);

                gameStats.totalSpins++;
                gameStats.rewardStats[result.id]++;

                const today = new Date().toISOString().split('T')[0];
                gameStats.dailyStats[today] = (gameStats.dailyStats[today] || 0) + 1;

                // Ghi lại lịch sử quay (giới hạn 50 lượt gần nhất)
                gameStats.spinHistory.unshift({
                    id: gameStats.totalSpins,
                    reward: result.name,
                    timestamp: new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }),
                    isWin: result.id !== 5
                });
                if (gameStats.spinHistory.length > 50) {
                    gameStats.spinHistory.pop();
                }

                console.log(`🎯 Lần quay #${gameStats.totalSpins}: ${result.name} (ID: ${result.id})`);

                broadcast({
                    type: 'spin_result',
                    result,
                    angle,
                    spinNumber: gameStats.totalSpins,
                    timestamp: Date.now()
                });

                if (gameStats.totalSpins % 10 === 0) {
                    saveStats();
                }

            } else if (data.type === 'get_stats') {
                ws.send(JSON.stringify({
                    type: 'stats_response',
                    stats: gameStats
                }));
            } else if (data.type === 'sync_history') {
                ws.send(JSON.stringify({
                    type: 'sync_history',
                    history: gameStats.spinHistory.slice(0, 10)
                }));
            }
        } catch (error) {
            console.error('❌ Lỗi xử lý tin nhắn:', error);
            ws.send(JSON.stringify({
                type: 'error',
                message: 'Có lỗi xảy ra, vui lòng thử lại!'
            }));
        }
    });

    ws.on('close', () => {
        gameStats.playersOnline = Math.max(0, gameStats.playersOnline - 1);
        console.log(`❌ Người chơi ngắt kết nối. Online: ${gameStats.playersOnline} lúc`, new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }));
        rateLimiter.delete(ws);
        broadcast({
            type: 'players_update',
            playersOnline: gameStats.playersOnline
        });
    });

    ws.on('error', (error) => {
        console.error('❌ Lỗi WebSocket:', error);
    });
});



