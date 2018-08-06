/*
 * permutation.hpp
 *
 *  Created on: 05/08/18
 *      Author: hello@whatsthecraic.net
 */

#ifndef PERM_VS_SORT_PERMUTATION_HPP
#define PERM_VS_SORT_PERMUTATION_HPP

#include <cinttypes>

namespace common {

/**
 * Create a random permutation of the allocated (but not initialised) array of cardinality array_sz.
 * The final elements will be a shuffle of the `array_sz' elements in [0, array_sz).
 */
template<typename T>
void permute(T* array, uint64_t array_sz, uint64_t seed);

}

#endif //PERM_VS_SORT_PERMUTATION_HPP
