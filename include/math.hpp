/*
 * math.hpp
 *
 *  Created on: 05/08/18
 *      Author: hello@whatsthecraic.net
 *
 * A set of miscellaneous math functions
 *
 */

#ifndef PERM_VS_SORT_MATH_HPP
#define PERM_VS_SORT_MATH_HPP

#include <cinttypes>

namespace common {

/**
 * Get the lowest power of 2, p,  such that p >= x
 * That is: 2^ceil(log2(x))
 */
uint64_t hyperceil(uint64_t x);


} // common

#endif //PERM_VS_SORT_MATH_HPP
