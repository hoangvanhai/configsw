#include "Random.h"
#include <random>
using namespace cppframework::rand;

int Random::generate(void* data, int len){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    unsigned char* d = (unsigned char*)data;
    int rlen = 0;
    while(len > 0){
        *d = dist(mt);
        d++;
        len --;
        rlen ++;
    }
    return rlen;
}
int Random::generate(int min, int max){
    if (max == min) return 0;
    if(max < min) std::swap(min, max);
    std::random_device rd;    
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

//end of file

