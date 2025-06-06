#include <iostream>
#include <random>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

// Cấu trúc quản lý phần thưởng với xác suất
struct RewardConfig {
    map<int, double> rewards; // <giá trị phần thưởng, xác suất>
    double totalProbability = 0.0;

    RewardConfig() {
        // Khởi tạo danh sách phần thưởng với xác suất
        rewards[100] = 0.30; // 30% cơ hội
        rewards[200] = 0.25; // 25% cơ hội
        rewards[300] = 0.15; // 15% cơ hội
        rewards[0] = 0.10;   // 10% cơ hội (không có thưởng)
        rewards[50] = 0.15;  // 15% cơ hội
        rewards[150] = 0.05; // 5% cơ hội (phần thưởng hiếm)

        // Tính tổng xác suất
        for (const auto& reward : rewards) {
            totalProbability += reward.second;
        }
        if (abs(totalProbability - 1.0) > 0.01) {
            throw runtime_error("Tong xac suat phai gan bang 1.0");
        }
    }
};

// Hàm tạo phần thưởng ngẫu nhiên với xác suất
int generateReward() {
    static RewardConfig config;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double randomValue = dis(gen);

    double cumulativeProbability = 0.0;
    for (const auto& reward : config.rewards) {
        cumulativeProbability += reward.second;
        if (randomValue <= cumulativeProbability) {
            return reward.first;
        }
    }
    return 0; // Giá trị mặc định nếu có lỗi
}

// Hàm xử lý yêu cầu quay
string processSpinRequest(const string& request) {
    if (request.empty()) {
        return "ERROR: Yeu cau rong";
    }
    if (request != "SPIN") {
        return "ERROR: Yeu cau khong hop le";
    }

    int reward = generateReward();
    return "RESULT:" + to_string(reward);
}