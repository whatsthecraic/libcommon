/**
 * This file is part of libcommon.
 *
 * libcommon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, orF
 * (at your option) any later version.
 *
 * libcommon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libcommon.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "timer.hpp"

#include <cinttypes>
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
//    if(time_in_seconds >= 10){
//        result << time_in_seconds << " seconds";
//    } else {
        char buffer[128];
        snprintf(buffer, 128, "%.3d", (int) (time_in_millisecs % 1000));
        result << time_in_seconds << "." << buffer << " seconds";
//    }

    return result.str();
}

template <typename D>
static string to_minutes(D duration){
    uint64_t seconds = ((uint64_t) duration_cast<chrono::seconds>(duration).count()) % 60ull;
    uint64_t minutes = (uint64_t) duration_cast<chrono::minutes>(duration).count();

    char buffer[128];
    snprintf(buffer, 128, "%" PRIu64 ":%02" PRIu64 " minutes", minutes, seconds);
    return string(buffer);
}

template <typename D>
static string to_hours(D duration){
    uint64_t seconds = ((uint64_t) duration_cast<chrono::seconds>(duration).count()) % 60ull;
    uint64_t minutes = (uint64_t) duration_cast<chrono::minutes>(duration).count() % 60ull;
    uint64_t hours = (uint64_t) duration_cast<chrono::hours>(duration).count();

    char buffer[128];
    snprintf(buffer, 128, "%" PRIu64 ":%02" PRIu64 ":%02" PRIu64 " hours", hours, minutes, seconds);
    return string(buffer);
}

template <typename D>
static string to_days(D duration){
    uint64_t seconds = ((uint64_t) duration_cast<chrono::seconds>(duration).count()) % 60ull;
    uint64_t minutes = (uint64_t) duration_cast<chrono::minutes>(duration).count() % 60ull;
    uint64_t hours = (uint64_t) duration_cast<chrono::hours>(duration).count() % 24ull;
    uint64_t days = (uint64_t) duration_cast<chrono::hours>(duration).count() / 24;

    char buffer[128];
    snprintf(buffer, 128, "%" PRIu64 " day(s) and %" PRIu64 ":%02" PRIu64 ":%02" PRIu64 " hours", days, hours, minutes, seconds);
    return string(buffer);
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
    } else if(time_in_nanosecs <= (uint64_t) pow(10, 9) * 90){ // 90 seconds
        return to_seconds(d);
    } else if(time_in_nanosecs < (uint64_t) pow(10, 9) * 60 * 60){
        return to_minutes(d);
    } else if(time_in_nanosecs < (uint64_t) pow(10, 9) * 60 * 60 * 24){
        return to_hours(d);
    } else {
        return to_days(d);
    }
}

template<bool use_barrier>
std::ostream& (common::operator<<)(std::ostream& out, const common::Timer<use_barrier>& timer){
    out << timer.to_string();
    return out;
}

template<bool B1, bool B2>
Timer<B1> (common::operator+)(Timer<B1> timer1, Timer<B2> timer2){
    using clock = typename Timer<B1>::clock;
    typename clock::time_point nullpoint{};
    Timer<B1> result;

    // stop the timer if they are executing. Note we are passing the timers by value, no changes are forwarded to the original timers
    if(timer1.m_t0 != nullpoint && timer1.m_t1 == nullpoint){
        timer1.m_t1 = clock::now();
    }
    if(timer2.m_t0 != nullpoint && timer2.m_t1 == nullpoint){
        timer2.m_t1 = clock::now();
    }

    // first timer never launched?
    if(timer1.m_t0 == nullpoint){
        result.m_t0 = timer2.m_t0;
        result.m_t1 = timer2.m_t1;
    } else {
        // sum the duration from the two timers
        result = timer1;

        if(timer2.m_t0 != nullpoint){
            result.m_t0 -= (timer2.m_t1 - timer2.m_t0);
        }
    }

    return result;
};


// Template instantiantions
template class common::Timer<true>;
template class common::Timer<false>;
template ostream& (common::operator<<)(ostream& out, const Timer<true>& timer);
template ostream& (common::operator<<)(ostream& out, const Timer<false>& timer);
template Timer<true> (common::operator+)(Timer<true> t1, Timer<true> t2);
template Timer<true> (common::operator+)(Timer<true> t1, Timer<false> t2);
template Timer<false> (common::operator+)(Timer<false> t1, Timer<true> t2);
template Timer<false> (common::operator+)(Timer<false> t1, Timer<false> t2);