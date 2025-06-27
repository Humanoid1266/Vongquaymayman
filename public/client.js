// client.js - File client sử dụng WebSocket cho trò chơi trực tuyến
let socket;

let isSpinning = false;
let spinHistory = [];
let reconnectAttempts = 0;
const maxReconnectAttempts = 5;

const audioContext = new (window.AudioContext || window.webkitAudioContext)();

// Sound effects
function playSound(frequency, duration, type = 'sine') {
    if (audioContext.state === 'suspended') audioContext.resume();
    try {
        const oscillator = audioContext.createOscillator();
        const gainNode = audioContext.createGain();
        oscillator.connect(gainNode);
        gainNode.connect(audioContext.destination);
        oscillator.frequency.value = frequency;
        oscillator.type = type;
        gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + duration);
        oscillator.start(audioContext.currentTime);
        oscillator.stop(audioContext.currentTime + duration);
    } catch (e) {
        console.log('Audio not supported:', e);
    }
}

// Confetti effect
function createConfetti() {
    const colors = ['#ff6b6b', '#4ecdc4', '#45b7d1', '#96ce79', '#f7e987'];
    for (let i = 0; i < 50; i++) {
        const confetti = document.createElement('div');
        confetti.className = 'confetti';
        confetti.style.left = Math.random() * 100 + '%';
        confetti.style.backgroundColor = colors[Math.floor(Math.random() * colors.length)];
        confetti.style.animationDelay = Math.random() * 3 + 's';
        document.body.appendChild(confetti);
        setTimeout(() => confetti.remove(), 3000);
    }
}

// Canvas setup
const canvas = document.getElementById('wheelCanvas');
const ctx = canvas.getContext('2d');
const center = { x: canvas.width / 2, y: canvas.height / 2 };
const radius = 160;
let currentRotation = 0;

function drawWheel(angle) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    const segmentAngle = (2 * Math.PI) / 5;

    ['1000$', '500$', '200$', '100$', 'Chúc may mắn'].forEach((reward, index) => {
        const startAngle = index * segmentAngle + angle;
        const endAngle = (index + 1) * segmentAngle + angle;

        ctx.beginPath();
        ctx.moveTo(center.x, center.y);
        ctx.arc(center.x, center.y, radius, startAngle, endAngle);
        ctx.closePath();
        ctx.fillStyle = ['#ff6b6b', '#4ecdc4', '#45b7d1', '#96ce79', '#f7e987'][index];
        ctx.fill();
        ctx.strokeStyle = '#fff';
        ctx.lineWidth = 3;
        ctx.stroke();

        ctx.save();
        ctx.translate(center.x, center.y);
        ctx.rotate(startAngle + segmentAngle / 2);
        ctx.textAlign = 'center';
        ctx.fillStyle = '#fff';
        ctx.font = 'bold 16px Nunito';
        ctx.fillText(reward, radius * 0.7, 5);
        ctx.restore();
    });
}

drawWheel(0);

// WebSocket connection
function connectWebSocket() {
    socket.onopen = () => {
        console.log('✅ Kết nối WebSocket thành công lúc', new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }));
        reconnectAttempts = 0;
        updateStatus('🟢 Kết nối thành công - Sẵn sàng chơi!', 'success');
        enableSpinButton();
        socket.send(JSON.stringify({ type: 'sync_history' }));
    };

    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        handleServerMessage(data);
    };

    socket.onclose = () => {
        console.log('❌ Kết nối bị ngắt lúc', new Date().toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }));
        updateStatus('🔴 Mất kết nối - Đang thử kết nối lại...', 'error');
        disableSpinButton();
        attemptReconnect();
    };

    socket.onerror = (error) => {
        console.error('❌ Lỗi WebSocket:', error);
        updateStatus('❌ Lỗi kết nối', 'error');
    };
}

// Handle server messages
function handleServerMessage(data) {
    switch (data.type) {
        case 'welcome':
            updateStatus(data.message, 'success');
            if (data.stats) updateGameStats(data.stats);
            break;
        case 'spin_result':
            handleSpinResult(data);
            break;
        case 'players_update':
            updatePlayersCount(data.playersOnline);
            break;
        case 'error':
            showNotification(data.message, 'error');
            isSpinning = false;
            enableSpinButton();
            break;
        case 'stats_response':
            displayDetailedStats(data.stats);
            break;
        case 'sync_history':
            if (data.history) {
                spinHistory = data.history;
                updateSpinHistory();
            }
            break;
    }
}

// Handle spin result
function handleSpinResult(data) {
    const { result, angle, spinNumber, timestamp } = data;
    console.log(`[${new Date(timestamp).toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' })}] Kết quả: ${result.name} (ID: ${result.id})`);

    spinHistory.unshift({
        id: spinNumber,
        reward: result.name,
        timestamp: new Date(timestamp).toLocaleString('vi-VN', { timeZone: 'Asia/Ho_Chi_Minh' }),
        isWin: result.id !== 5
    });

    if (spinHistory.length > 10) spinHistory.pop();

    animateWheel(angle / 360 * 2 * Math.PI, () => {
        displayResult(result);
        updateSpinHistory();
        if (result.id !== 5) {
            playSound(800, 0.5);
            createConfetti();
            showNotification(`🎉 Chúc mừng! Bạn đã trúng ${result.name}!`, 'success');
        } else {
            playSound(200, 0.3);
            showNotification('😢 Chúc may mắn lần sau!', 'info');
        }
        isSpinning = false;
        enableSpinButton();
    });
}

// Animate wheel
function animateWheel(targetAngle, callback) {
    const duration = 3000;
    const startTime = Date.now();

    function animate() {
        const elapsed = Date.now() - startTime;
        const progress = Math.min(elapsed / duration, 1);
        const easeOut = 1 - Math.pow(1 - progress, 3);
        currentRotation = easeOut * targetAngle;

        drawWheel(currentRotation);

        if (progress < 1) requestAnimationFrame(animate);
        else callback();
    }
    animate();
}

// Spin wheel
function spinWheel() {
    
    if (socket.readyState !== WebSocket.OPEN || isSpinning) return;

    isSpinning = true;
    disableSpinButton();
    updateStatus('⏳ Đang quay...', 'info');
    playSound(400, 0.2);
    socket.send(JSON.stringify({ type: 'spin' }));
}