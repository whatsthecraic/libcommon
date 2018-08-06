/*
 * timer.hpp
 *
 *  Created on: 22/07/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef COMMON_TIMER_HPP
#define COMMON_TIMER_HPP

#include <chrono>
#include <ostream>
#include "error.hpp"
#include "optimisation.hpp"

namespace common {

template<bool use_barrier = false>
class Timer {
//    Timer(const Timer&) = delete;
//    Timer& operator=(const Timer& timer) = delete;
    using clock = std::chrono::steady_clock;

    clock::time_point m_t0; // start time
    clock::time_point m_t1; // end time

public:
    Timer(){ }

    void start(){
        m_t1 = clock::time_point{};
        if(use_barrier) barrier();
        m_t0 = clock::now();
        if(use_barrier) barrier();
    }

    void resume(){
        if(m_t0 != clock::time_point{}) return; // already running;
        if(m_t1 == clock::time_point{}){ // this timer has never been executed
            start();
        } else {
            if(use_barrier) barrier();
            m_t0 = clock::now() - (m_t1 - m_t0);
            if(use_barrier) barrier();
        }
    }

    void stop() {
        if(use_barrier) barrier();
        m_t1 = clock::now();
        if(use_barrier) barrier();
    }

    template<typename D>
    uint64_t convert() const {
        return (uint64_t) std::chrono::duration_cast<D>(m_t1 - m_t0).count();
    }

    uint64_t nanoseconds() const{ return convert<std::chrono::nanoseconds>(); }

    uint64_t microseconds() const{ return convert<std::chrono::microseconds>(); }

    uint64_t milliseconds() const{ return convert<std::chrono::milliseconds>(); }

    uint64_t seconds() const{ return convert<std::chrono::seconds>(); }

    std::string to_string() const;
};

template<bool use_barrier>
std::ostream& operator<<(std::ostream& out, const common::Timer<use_barrier>& timer);

} // namespace common

#endif //COMMON_TIMER_HPP
