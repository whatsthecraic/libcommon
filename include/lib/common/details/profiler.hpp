/*
 * profiler.hpp
 *
 *  Created on: 23/07/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef COMMON_PROFILER_DETAILS_HPP
#define COMMON_PROFILER_DETAILS_HPP

#include <cstddef>

namespace common { namespace details {

/**
 * Initialise the PAPI library (if not already initialised) and retrieve the event code
 * associated to a named event
 */
class BaseProfiler {
    static bool library_initialised;
    static void initialise_library();

public:
    BaseProfiler();

    /**
     * Return -1 if the event is not available, otherwise it's PAPI event code
     */
    static int get_event_code(const char* event_name);
};


/**
 * Boiler plate to register the PAPI events and start and stop the recording of perf_events.
 */
class GenericProfiler : public BaseProfiler {
    static constexpr size_t m_events_capacity = 8;

protected:
    int m_events[m_events_capacity];
    int m_events_sz = 0;
    int m_event_set = 0;

    void add_events(const char* errorstring, const char* event_name);
    void add_events(const char* errorstring, const char* alternative_events[], size_t num_alternative_events);
    void register_events();
    void unregister_events();

    void start();
    void stop(long long* resultset);
    void snapshot(long long* resultset);

public:
    GenericProfiler();
    ~GenericProfiler();
};

}} // common::details

#endif //COMMON_PROFILER_DETAILS_HPP