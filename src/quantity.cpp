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

#include "quantity.hpp"

#include <cassert>
#include <cctype> // tolower
#include <cmath>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#undef CURRENT_ERROR_TYPE
#define CURRENT_ERROR_TYPE QuantityError

using namespace common;
using namespace std;

Quantity::Quantity() : Quantity(0) {}

Quantity::Quantity(uint64_t magnitude, bool byte_suffix) : m_magnitude(magnitude), m_is_byte_quantity(byte_suffix) { }

Quantity::Quantity(const string& quantity, bool byte_suffix) :
        m_magnitude(parse(quantity, byte_suffix ? ByteSuffix::Optional : ByteSuffix::Missing)),
        m_is_byte_quantity(byte_suffix){ }

uint64_t Quantity::magnitude() const noexcept {
    return m_magnitude;
}

Quantity::operator int64_t() const noexcept {
    return static_cast<int64_t>(m_magnitude);
}

uint64_t Quantity::parse(const string& quantity, ByteSuffix byte_suffix) {
    string pattern {"^\\s*(\\d+)?(\\.\\d+)?\\s*([KMGT]?)("}; // kilo, mega, giga tera
    if(byte_suffix == ByteSuffix::Optional || byte_suffix == ByteSuffix::Mandatory) pattern += "B"; // bytes
    if(byte_suffix == ByteSuffix::Optional) pattern += "?";
    pattern += ")\\s*$";
//    cout << "pattern: " << pattern << endl;
    regex regex{pattern, regex::icase};
    smatch match;
    if (!regex_match(begin(quantity), end(quantity), match, regex)){ ERROR("Invalid quantity: " << quantity); }
//    cout << "Number of matches: " << match.size() << endl;
//    for(size_t i = 0; i < match.size(); i++){
//        cout << "[" << i << "] " << match[i] << ", length: " << match[i].length() << endl;
//    }

    // check the `byte' suffix is present
    if(byte_suffix == ByteSuffix::Mandatory && match[4].length() == 0){
        ERROR("Invalid quantity: " << quantity << ". The byte (b) suffix is required.");
    }

    // multiply the quantity by the unit
    uint64_t mult = 1;
    if(match[3].length() > 0){
        assert(match[3].length() == 1 && "Expected one of the following single units K, M, G or T");
        char unit = tolower(match[3].str().c_str()[0]);
        switch(unit){
        case 'k': mult = (1ull <<10); break;
        case 'm': mult = (1ull <<20); break;
        case 'g': mult = (1ull <<30); break;
        case 't': mult = (1ull <<40); break;
        default: assert(0 && "Invalid unit");
        }
    }

    // parse the magnitude
    uint64_t result{0};
    string str_value = match[1];
    str_value += match[2];
    if(str_value.empty()){ ERROR("Magnitude missing: " << quantity); }
    bool is_decimal = match[2].length() > 0;
    if(is_decimal){
        size_t leftover;
        double value = stod(str_value, &leftover);
        if(leftover != str_value.length()) ERROR("Parse error: " << quantity);
        value *= mult;
        double intpart;
        double fractpart = modf(value, &intpart);
        if(fractpart != 0.) ERROR("Cannot cast to an integer value: " << quantity << ". Absolute value: " << value);
        result = value;
    } else {
        size_t leftover;
        result = stoull(str_value, &leftover);
        if(leftover != str_value.length()) ERROR("Parse error: " << quantity);
        result *= mult;
    }

    return result;
}

static string convert_to_string(uint64_t magnitude, Quantity::Unit unit, bool byte_suffix){
    using Unit = Quantity::Unit;

    // unit
    uint64_t mult = 1;
    string suffix;
    switch(unit){
    case Unit::BASIC: break; // nop
    case Unit::KILO: mult = (1ull << 10); suffix = "K"; break;
    case Unit::MEGA: mult = (1ull << 20); suffix = "M"; break;
    case Unit::GIGA: mult = (1ull << 30); suffix = "G"; break;
    case Unit::TERA: mult = (1ull << 40); suffix = "T"; break;
    default: ERROR("Invalid unit: " << (int) unit);
    }
    if(byte_suffix){
        if(mult == 1){
            suffix += "bytes";
        } else {
            suffix += "B";
        }
    }

    stringstream ss;
    if(magnitude % mult == 0){
        ss << magnitude / mult;
    } else {
        char buffer[128];
        snprintf(buffer, 128, "%.2f", ((double) magnitude) / mult);
        ss << buffer;
    }

    if(!suffix.empty()) {
        if (byte_suffix) ss << " ";
        ss << suffix;
    }

    return ss.str();
}

string Quantity::to_string(Quantity::Unit unit) const {
    switch(unit){
    case Unit::AUTO:
        if(m_magnitude >= (1ull << 40)){
            return to_string(Unit::TERA);
        } else if(m_magnitude >= (1ull << 30)){
            return to_string(Unit::GIGA);
        } else if(m_magnitude >= (1ull << 20)){
            return to_string(Unit::MEGA);
        } else if(m_magnitude >= (1ull << 10)){
            return to_string(Unit::KILO);
        } else {
            return to_string(Unit::BASIC);
        }
    default:
        return convert_to_string(m_magnitude, unit, m_is_byte_quantity);
    }
}

ostream& (::common::operator<<)(ostream& out, const Quantity& q) {
    out << q.to_string();
    return out;
}

ostream& (::common::operator<<)(ostream& out, const Quantity* q){
    out << "ptr: 0x" << std::hex << (void*) q << std::dec;
    if(q != nullptr){
        out << " (" << q->to_string() << ")";
    }
    return out;
}


std::istream& (::common::operator>>)(std::istream& in, common::Quantity& q){
    string value;
    in >> value;
    auto magnitude = Quantity::parse(value, q.is_byte_quantity() ? Quantity::ByteSuffix::Mandatory : Quantity::ByteSuffix::Missing);
    q.m_magnitude = magnitude;
    return in;
}

Quantity (::common::operator+)(Quantity q1, int64_t q2) {
    int64_t v1 = q1.magnitude();
    int64_t v2 = q2;
    int64_t res = v1 + v2;
    if(res < 0) ERROR("Negative quantity: " << v1 << " + " << v2);
    return Quantity(res, q1.is_byte_quantity());
}

Quantity (::common::operator-)(Quantity q1, int64_t q2){
    return q1 + (-q2);
}

Quantity (::common::operator*)(Quantity q1, int64_t q2){
    int64_t v1 = q1.magnitude();
    int64_t v2 = q2;
    int64_t res = v1 * v2;
    if(res < 0) ERROR("Negative quantity: " << q1 << " * " << q2);
    return Quantity(res, q1.is_byte_quantity());
}

Quantity (::common::operator/)(Quantity q1, int64_t q2){
    int64_t n = q1.magnitude();
    int64_t d = q2;
    int64_t res = n / d;
    if(res < 0) ERROR("Negative quantity: " << q1 << " * " << q2);
    return Quantity(res, q1.is_byte_quantity());
}

Quantity& Quantity::operator+=(int64_t value) {
    int64_t v1 = magnitude();
    int64_t v2 = value;
    int64_t res = v1 + v2;
    if(res < 0) ERROR("Negative quantity: " << v1 << " + " << v2);
    m_magnitude = static_cast<uint64_t>(res);
    return *this;
}

Quantity& Quantity::operator-=(int64_t value) {
    return this->operator+=(-value);
}

Quantity& Quantity::operator*=(int64_t v2) {
    int64_t v1 = magnitude();
    int64_t res = v1 * v2;
    if(res < 0) ERROR("Negative quantity: " << *this << " * " << v2);
    m_magnitude = static_cast<uint64_t>(res);
    return *this;
}

Quantity& Quantity::operator/=(int64_t d) {
    int64_t n = magnitude();
    int64_t res = n / d;
    if(res < 0) ERROR("Negative quantity: " << *this << " * " << d);
    m_magnitude = static_cast<uint64_t>(res);
    return *this;
}
