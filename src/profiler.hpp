/*
 * profiler.hpp
 *
 *  Created on: 23/07/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef COMMON_PROFILER_HPP
#define COMMON_PROFILER_HPP

#include <cinttypes>
#include <cstddef>
#include <ostream>

#include "error.hpp"
#include "details/profiler.hpp"
#include "database.hpp"

/*****************************************************************************
 *                                                                           *
 *   Exceptions                                                              *
 *                                                                           *
 *****************************************************************************/
namespace common {
DEFINE_EXCEPTION(ProfilerError);
}

/*****************************************************************************
 *                                                                           *
 *   Cache faults                                                            *
 *                                                                           *
 *****************************************************************************/
namespace common {
/**
 * Data recorded by the CachesProfiler
 */
struct CachesSnapshot {
    uint64_t m_cache_l1_misses = 0; // number of misses in the L1
    uint64_t m_cache_llc_misses = 0; // number of misses in the LLC (=L3 assumed)
    uint64_t m_cache_tlb_misses = 0; // number of misses in the TLB (I think from LLC, assumes page-walk)

    void operator+=(CachesSnapshot snapshot);
};

/**
 * Record the amount of L1, LLC and TLB faults hit during the execution.
 * Usage:
 *      CachesProfiler profiler;
 *      profiler.start();
 *      ... computation ...
 *      profiler.stop();
 *      results = profiler.snapshot();
 */
class CachesProfiler : public details::GenericProfiler {
    CachesSnapshot m_current_snapshot;

public:
    /**
     * Initialise the profiler
     */
    CachesProfiler();

    /**
     * Destructor
     */
    ~CachesProfiler();


    /**
     * Start recording
     */
    void start();

    /**
     * Retrieve the data associated to this profiler
     */
    CachesSnapshot snapshot();

    /**
     * Stop the recording
     */
    CachesSnapshot stop();

    /**
     * Retrieve a data record ready to be stored in the database
     */
    Database::BaseRecord data_record();
};

} // namespace common

std::ostream& operator<<(std::ostream& out, const common::CachesSnapshot& snapshot);

/*****************************************************************************
 *                                                                           *
 *   Branch mispredictions                                                   *
 *                                                                           *
 *****************************************************************************/
namespace common {

/**
 * Data recorded by the BranchMispredictionsProfiler
 */
struct BranchMispredictionsSnapshot{
    uint64_t m_conditional_branches = 0; // total number of conditional branch instructions
    uint64_t m_branch_mispredictions = 0; // total number of branch mispredictions
    uint64_t m_cache_l1_misses = 0; // number of cache misses in the L1
    uint64_t m_cache_llc_misses = 0; // number of cache misses in the LLC (=L3 assumed)

    void operator+=(BranchMispredictionsSnapshot snapshot);
};

/**
 * Record the amount of branch misprediction AND L1 and LLC faults during the executions
 */
struct BranchMispredictionsProfiler : public details::GenericProfiler {
    BranchMispredictionsSnapshot m_current_snapshot;

public:
    /**
 * Initialise the profiler
 */
    BranchMispredictionsProfiler();

    /**
     * Destructor
     */
    ~BranchMispredictionsProfiler();


    /**
     * Start recording
     */
    void start();

    /**
     * Retrieve the data associated to this profiler
     */
    BranchMispredictionsSnapshot snapshot();

    /**
     * Stop the recording
     */
    BranchMispredictionsSnapshot stop();

    /**
     * Retrieve a data record ready to be stored in the database
     */
    Database::BaseRecord data_record();
};

} // namespace common

std::ostream& operator<<(std::ostream& out, const common::BranchMispredictionsSnapshot& snapshot);

#endif //COMMON_PROFILER_HPP
