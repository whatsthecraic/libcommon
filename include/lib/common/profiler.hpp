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
    Database::BaseRecord data_record() const;
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

std::ostream& operator<<(std::ostream& out, const CachesSnapshot& snapshot);
CachesSnapshot operator+(const CachesSnapshot& s1, const CachesSnapshot& s2);

/*****************************************************************************
 *                                                                           *
 *   Branch mispredictions                                                   *
 *                                                                           *
 *****************************************************************************/

/**
 * Data recorded by the BranchMispredictionsProfiler
 */
struct BranchMispredictionsSnapshot{
    uint64_t m_conditional_branches = 0; // total number of conditional branch instructions
    uint64_t m_branch_mispredictions = 0; // total number of branch mispredictions
    uint64_t m_cache_l1_misses = 0; // number of cache misses in the L1
    uint64_t m_cache_llc_misses = 0; // number of cache misses in the LLC (=L3 assumed)

    void operator+=(BranchMispredictionsSnapshot snapshot);
    Database::BaseRecord data_record() const;
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

std::ostream& operator<<(std::ostream& out, const BranchMispredictionsSnapshot& snapshot);

} // namespace common


#endif //COMMON_PROFILER_HPP
