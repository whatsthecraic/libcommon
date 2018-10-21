#include "gtest/gtest.h"

#include <cinttypes>
#include <random>
#include "lib/common/profiler.hpp"

using namespace std;
using namespace common;


volatile uint64_t cache_faults_count = 0;

TEST(Profiler, cache_faults){
    constexpr int64_t A_sz = (1ull << 20); // 1M
    int64_t* A = new int64_t[A_sz];

    // init
    mt19937_64 random{1ull};
    CachesProfiler c;
    c.start();
    A[0] = 0;
    for(int64_t i = 1; i < A_sz; i++){
        int64_t j = uniform_int_distribution<int64_t>{0, i-1}(random);
        A[i] = A[j];
        A[j] = i; // ptr to A[i];
    }
    c.stop();

    cout << "Init, sequential accesses, faults: " << c.snapshot() << endl;

    // run
    c.start();
    int64_t index = 0;
    for(int i = 1; i < (1ull << 20); i++){
        index = A[index];
        cache_faults_count += index;
    }
    c.stop();
    cout << "Run, random accesses, faults: " << c.snapshot() << endl;

    delete[] A; A = nullptr;
}