#include <iostream>
#include <random>
#include <string>
#include <map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

struct RewardConfig {
    map<int, double> rewards;
    double totalProbability = 0.0;

    RewardConfig() {
        if (!loadFromFile("reward_config.txt")) {
            throw runtime_error("Khong the doc file reward_config.txt");
        }
    }

    bool loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        rewards.clear();
        totalProbability = 0.0;

        string line;
        int value;
        double prob;
        while (getline(file, line)) {
            istringstream iss(line);
            if (!(iss >> value >> prob)) continue;
            rewards[value] = prob;
            totalProbability += prob;
        }
        file.close();

        if (fabs(totalProbability - 1.0) > 0.01) {
            throw runtime_error("Tong xac suat phai gan bang 1.0 (dang: " + to_string(totalProbability) + ")");
        }

        return true;
    }
};

int generateReward(const RewardConfig& config, mt19937& gen) {
    uniform_real_distribution<> dis(0.0, 1.0);
    double randomValue = dis(gen);

    double cumulativeProbability = 0.0;
    for (const auto& reward : config.rewards) {
        cumulativeProbability += reward.second;
        if (randomValue <= cumulativeProbability) {
            return reward.first;
        }
    }
    return 0;
}

string processSpinRequest(const string& request, const RewardConfig& config, mt19937& gen) {
    if (request.empty()) return "ERROR: Yeu cau rong";
    if (request != "SPIN") return "ERROR: Yeu cau khong hop le";

    int reward = generateReward(config, gen);
    return "RESULT:" + to_string(reward);
}
