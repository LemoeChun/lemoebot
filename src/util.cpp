#include <random>

int random(int min, int max){
    std::random_device seed;
    std::mt19937_64 gen{seed()};
    std::uniform_int_distribution<> dist{min,max};
    return dist(gen);
}
int random(int max){
    return random(0,max);
}
