/*
 * math.cpp
 *
 *  Created on: 05/08/18
 *      Author: hello@whatsthecraic.net
 */

#include "math.hpp"

#include <cinttypes>
#include <cmath>

namespace common {

uint64_t hyperceil(uint64_t value) {
    return (uint64_t) pow(2, ceil(log2(static_cast<double>(value))));
}

}