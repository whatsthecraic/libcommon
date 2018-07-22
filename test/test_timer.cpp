#include "gtest/gtest.h"

#include <iostream>
#include <unistd.h> // sleep
#include "timer.hpp"

using namespace std;
using namespace common;

TEST(Timer, basic_usage){
    Timer<> t;
    t.start();
    sleep(1);
    t.stop();
    ASSERT_EQ(t.to_string(), string{"1.000 seconds"});
    t.resume();
    sleep(1);
    t.stop();
    ASSERT_EQ(t.to_string(), string{"2.000 seconds"});
}