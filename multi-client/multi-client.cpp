#include <iostream>
#include <winsock2.h>
#include <random>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#define PORT 2808 
using namespace std;

int generateReward() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 5);
    int rewards[] = {100, 200, 300, 0, 50, 150};
    return rewards[dis(gen)];
}
