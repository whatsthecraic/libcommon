/*
 * optimisation.hpp
 *
 *  Created on: 22/07/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef COMMON_OPTIMISATION_HPP
#define COMMON_OPTIMISATION_HPP

#include <cinttypes>

namespace common {

/**
 * Compiler barrier
 */
inline void barrier(){
    __asm__ __volatile__("": : :"memory");
};

/**
 * Read the CPU timestamp counter
 */
inline uint64_t rdtscp(){
    uint64_t rax;
    asm volatile (
    "rdtscp ; shl $32, %%rdx; or %%rdx, %%rax; "
    : "=a" (rax)
    : /* no inputs */
    : "rcx", "rdx"
    );
    return rax;
}

/**
 * Branch prediction macros
 */
#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)

}


#endif //COMMON_OPTIMISATION_HPP
