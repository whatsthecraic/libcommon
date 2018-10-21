/*
 * spinlock.hpp
 *
 *  Created on: 21/10/18
 *      Author: hello@whatsthecraic.net
 */
#ifndef COMMON_SPINLOCK_HPP
#define COMMON_SPINLOCK_HPP

#include <atomic>

namespace common {

class SpinLock {
    std::atomic_flag m_lock = ATOMIC_FLAG_INIT; // atomic integral
public:
    /**
     * Acquire the lock
     */
    void lock() {
        while (m_lock.test_and_set(std::memory_order_acquire)) /* nop */;
    }

    /**
     * Release the lock
     */
    void unlock() {
        m_lock.clear(std::memory_order_release);
    }
};

} // namespace common

#endif /* COMMON_SPINLOCK_HPP */