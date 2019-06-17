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

#ifndef COMMON_SYSTEM_HPP
#define COMMON_SYSTEM_HPP

#include <string>

namespace common {

/**
 * Retrieve the current hostname
 */
std::string hostname();

/**
 * Try to retrieve the last git commit for the current program (not libcommon). It returns an empty string in case of failure.
 * The idea is that a build is performed inside some directory from the source, e.g. <src>/build. Then the git repository
 * should be visible as part of <src>.
 */
std::string git_last_commit(); // impl in system_introspection.cpp

namespace filesystem {

/**
 * Retrieve the absolute path to the program executable
 */
std::string path_executable(); // impl in filesystem.cpp

/**
 * Retrieve the absolute path to the directory of the executable
 */
std::string directory_executable(); // impl in filesystem.cpp

} // namespace filesystem

/**
 * Concurrency related settings
 */
namespace concurrency {

/**
 * Get the Linux thread id, the value shown in the debugger
 */
int64_t get_thread_id();

/**
 * Whether NUMA settings are available
 */
bool has_numa();

/**
 * Get the processor ID where the current thread is running
 */
int get_current_cpu();

/**
 * Get the Numa node associated to the CPU where the current thread is running
 */
int get_current_numa_node();

/**
 * Get the Numa node for the given CPU
 */
int get_numa_id(int cpu_id);

/**
 * Get the highest numa node in the system. Wrapper to `numa_max_node()'
 */
int get_numa_max_node();

/**
 * Pin the current thread to the current cpu and numa node. Optionally also pin the memory allocations from other NUMA nodes.
 */
void pin_thread_to_cpu(bool pin_numa_node = true);

/**
 * Pin the current thread to the given cpu and numa node. Disable memory allocations from the other NUMA nodes.
 * Note: as new created threads will inherit the same cpu mask, it is important to invoke this call to pin
 * an execution/sequential worker, rather than the main thread.
 */
void pin_thread_to_cpu(int cpu_id, bool pin_numa_node = true);

/**
 * Pin the current thread to the CPUs running at the given NUMA node
 */
void pin_thread_to_numa_node(int numa_node);

/**
 * Reset the pinning of the current thread
 * @param numa: if true, also unset the pinning to the NUMA node
 */
void unpin_thread(bool numa = true);


} // concurrency

} // common


#endif //COMMON_SYSTEM_HPP
