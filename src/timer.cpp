/*
 * timer.hpp
 *
 *  Created on: 22/07/18
 *      Author: hello@whatsthecraic.net
 */
#include "timer.hpp"

#include <cmath>
#include <sstream>

using namespace std;
using namespace std::chrono;
using namespace common;

template <typename D>
static string to_nanoseconds(D duration){
    stringstream result;
    result << (uint64_t) duration_cast<chrono::nanoseconds>(duration).count() << " nanosecs";
    return result.str();
}

template <typename D>
static string to_microseconds(D duration){
    uint64_t time_in_nanosecs = (uint64_t) duration_cast<chrono::nanoseconds>(duration).count();
    uint64_t time_in_microsecs = time_in_nanosecs / 1000;

    stringstream result;
    if(time_in_microsecs >= 3){
        result << time_in_microsecs << " microsecs";
    } else {
        char buffer[128];
        snprintf(buffer, 128, "%.3d", (int) (time_in_nanosecs % 1000));
        result << time_in_microsecs << "." << buffer << " microsecs";
    }

    return result.str();
}


template <typename D>
static string to_milliseconds(D duration){
    uint64_t time_in_microsecs = (uint64_t) duration_cast<chrono::microseconds>(duration).count();
    uint64_t time_in_millisecs = time_in_microsecs / 1000;

    stringstream result;
    if(time_in_microsecs >= 3){
        result << time_in_millisecs << " milliseconds";
    } else {
        char buffer[128];
        snprintf(buffer, 128, "%.3d", (int) (time_in_microsecs % 1000));
        result << time_in_millisecs << "." << buffer << " milliseconds";
    }

    return result.str();
}


template <typename D>
static string to_seconds(D duration){
    uint64_t time_in_millisecs = (uint64_t) duration_cast<chrono::milliseconds>(duration).count();
    uint64_t time_in_seconds = time_in_millisecs / 1000;

    stringstream result;
    if(time_in_seconds >= 10){
        result << time_in_seconds << " seconds";
    } else {
        char buffer[128];
        snprintf(buffer, 128, "%.3d", (int) (time_in_millisecs % 1000));
        result << time_in_seconds << "." << buffer << " seconds";
    }

    return result.str();
}

template <typename D>
static string to_minutes(D duration){
    uint64_t seconds = ((uint64_t) duration_cast<chrono::seconds>(duration).count()) % 60ull;
    uint64_t minutes = (uint64_t) duration_cast<chrono::minutes>(duration).count();

    stringstream result;
    result << minutes << "." << seconds << " minutes";
    return result.str();
}

template <typename D>
static string to_hours(D duration){
    uint64_t seconds = ((uint64_t) duration_cast<chrono::seconds>(duration).count()) % 60ull;
    uint64_t minutes = (uint64_t) duration_cast<chrono::minutes>(duration).count() % 60ull;
    uint64_t hours = (uint64_t) duration_cast<chrono::hours>(duration).count() % 60ull;

    stringstream result;
    result << hours << ":" << minutes << ":" << seconds << " hours";
    return result.str();
}

template <bool with_barrier>
string Timer<with_barrier>::to_string() const{
    // start and stop points in time
    if(m_t0 == clock::time_point{}) ERROR("Timer not even started");
    clock::time_point t1;
    if(m_t1 == clock::time_point{})
        t1 = clock::now();
    else
        t1 = m_t1;

    // duration
    auto d = t1 - m_t0;

    stringstream result;

    uint64_t time_in_nanosecs = (uint64_t) duration_cast<chrono::nanoseconds>(d).count();
    if(time_in_nanosecs <= 1000){
        return to_nanoseconds(d);
    } else if(time_in_nanosecs <= (uint64_t) pow(10, 6)){
        return to_microseconds(d);
    } else if(time_in_nanosecs <= (uint64_t) pow(10, 9)) {
        return to_milliseconds(d);
//  } else {
//        return to_seconds(d);
//    }

    } else if(time_in_nanosecs <= (uint64_t) pow(10, 12)){
        return to_seconds(d);
    } else if(time_in_nanosecs <= (uint64_t) pow(10, 12) * 60){
        return to_minutes(d);
    } else {
        return to_hours(d);
    }
}

template<bool use_barrier>
std::ostream& operator<<(std::ostream& out, const common::Timer<use_barrier>& timer){
    out << timer.to_string();
    return out;
}

template class common::Timer<true>;
template class common::Timer<false>;