/*
 * system_concurrency.cpp
 *
 *  Created on: 04/08/18
 *      Author: hello@whatsthecraic.net
 */

#include "system.hpp"

#include <cerrno>
#include <cstring> // strerror
#include <memory>
#include <pthread.h>
#include <sched.h> // sched_getcpu()
#include <sys/sysinfo.h> // get_nprocs(): return the number of available processors
#include "error.hpp"

// Support for libnuma
#if __has_include(<numa.h>)
#include <numa.h>
#if !defined(HAVE_LIBNUMA)
#define HAVE_LIBNUMA
#endif
#endif

using namespace std;

namespace common::concurrency {

bool has_numa(){
#if defined(HAVE_LIBNUMA)
    return numa_available() != -1;
#else
    return false;
#endif
}

int get_current_cpu(){
    return sched_getcpu();
}

int get_current_numa_node(){
    return get_numa_id(get_current_cpu());
}

int get_numa_id(int cpu_id){
    int numa_node = -1;
#if defined(HAVE_LIBNUMA)
    if( has_numa() )
        numa_node = numa_node_of_cpu(cpu_id);
#endif
    return numa_node;
}

int get_numa_max_node(){
    int max_node = -1;
#if defined(HAVE_LIBNUMA)
    if( has_numa() )
        max_node = numa_max_node();
#endif
    return max_node;
}

void pin_thread_to_cpu(bool pin_numa_node){
    pin_thread_to_cpu(get_current_cpu(), pin_numa_node);
}

void pin_thread_to_cpu(int target_cpu, bool pin_numa_node){
    auto current_thread = pthread_self();
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET(target_cpu, &cpu_set);
    auto rc = pthread_setaffinity_np(current_thread, sizeof(cpu_set), &cpu_set);
    if (rc != 0) { ERROR("[pin_thread_to_cpu] pthread_setaffinity_np, rc: " << rc); }

#if defined(HAVE_LIBNUMA)
    if(pin_numa_node){
        const bool use_libnuma { has_numa() };

        if (use_libnuma) {
            auto current_node = use_libnuma ? numa_node_of_cpu(target_cpu) : -1;
//          numa_set_bind_policy(true); // not required: numa_set_membind is already strict (MPOL_BIND)
            auto nodemask_deleter = [](struct bitmask* ptr) {
                numa_free_nodemask(ptr);
            };
            unique_ptr<struct bitmask, decltype(nodemask_deleter)> nodemask_ptr {
                    numa_allocate_nodemask(), nodemask_deleter };
            auto nodemask = nodemask_ptr.get();
            numa_bitmask_setbit(nodemask, current_node);
            numa_set_membind(nodemask);
        }
    }
#endif
}

static void throw_error_numa_not_available(){
    ERROR("[pin_thread_to_numa_node] NUMA is not available in this system");
}

void pin_thread_to_numa_node(int numa_node){
    if(!has_numa()) throw_error_numa_not_available();
    int rc = numa_run_on_node(numa_node);
    if(rc != 0){
       ERROR("[pin_thread_to_numa_node] Cannot pin the given node: " << numa_node << ", rc: " << rc << ", error: " << strerror(errno) << " (" << errno << "), ");
    }
}

void unpin_thread(bool unpin_numa) {
    auto current_thread = pthread_self();

    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    for(size_t i = 0, n = get_nprocs(); i < n; i++){ CPU_SET(i, &cpu_set); }
    auto rc = pthread_setaffinity_np(current_thread, sizeof(cpu_set), &cpu_set);
    if (rc != 0) { ERROR("[unpin_thread] pthread_setaffinity_np, rc: " << rc); }

#if defined(HAVE_LIBNUMA)
    if( unpin_numa && has_numa() ){
        numa_set_localalloc(); // MPOL_DEFAULT
    }
#endif
}

} // common::concurrency