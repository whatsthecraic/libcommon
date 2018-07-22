#include "gtest/gtest.h"

#include "quantity.hpp"

using namespace std;
using namespace common;


/**
 * Unit testing for the static method Quantity::parse()
 */
TEST(Quantity, parse){
    using ByteSuffix = Quantity::ByteSuffix;

    // straighforward numbers
    ASSERT_EQ( Quantity::parse("0", ByteSuffix::Missing), 0 );
    ASSERT_EQ( Quantity::parse("7", ByteSuffix::Missing), 7 );
    ASSERT_THROW( Quantity::parse("-1", ByteSuffix::Missing), QuantityError );

    // units
    ASSERT_EQ( Quantity::parse("0k", ByteSuffix::Missing), 0 );
    ASSERT_EQ( Quantity::parse("7k", ByteSuffix::Missing), 7 * 1024 );
    ASSERT_EQ( Quantity::parse("1K", ByteSuffix::Missing), 1024);
    ASSERT_EQ( Quantity::parse("1 k", ByteSuffix::Missing), 1024);
    ASSERT_THROW( Quantity::parse("-1k", ByteSuffix::Missing), QuantityError );
    ASSERT_EQ( Quantity::parse("1m", ByteSuffix::Missing), (1ull << 20));
    ASSERT_EQ( Quantity::parse("1M", ByteSuffix::Missing), (1ull << 20));
    ASSERT_EQ( Quantity::parse("1g", ByteSuffix::Missing), (1ull << 30));
    ASSERT_EQ( Quantity::parse("1G", ByteSuffix::Missing), (1ull << 30));
    ASSERT_EQ( Quantity::parse("1t", ByteSuffix::Missing), (1ull << 40));
    ASSERT_EQ( Quantity::parse("1T", ByteSuffix::Missing), (1ull << 40));
    ASSERT_THROW( Quantity::parse("0b", ByteSuffix::Missing), QuantityError );
    ASSERT_THROW( Quantity::parse("0B", ByteSuffix::Missing), QuantityError );
    ASSERT_THROW( Quantity::parse("0Kb", ByteSuffix::Missing), QuantityError );
    ASSERT_THROW( Quantity::parse("0 kB", ByteSuffix::Missing), QuantityError );
    ASSERT_THROW( Quantity::parse("0 Mb", ByteSuffix::Missing), QuantityError );
    ASSERT_THROW( Quantity::parse("0 MB", ByteSuffix::Missing), QuantityError );

    // byte quantities, with mandatory suffix
    ASSERT_THROW( Quantity::parse("0", ByteSuffix::Mandatory), QuantityError );
    ASSERT_THROW( Quantity::parse("0k", ByteSuffix::Mandatory), QuantityError );
    ASSERT_EQ( Quantity::parse("0b", ByteSuffix::Mandatory), 0 );
    ASSERT_EQ( Quantity::parse("0KB", ByteSuffix::Mandatory), 0 );
    ASSERT_EQ( Quantity::parse("2 KB", ByteSuffix::Mandatory), 2 * 1024 );

    // byte quantities, with optional byte suffix
    ASSERT_EQ( Quantity::parse("0", ByteSuffix::Optional), 0 );
    ASSERT_EQ( Quantity::parse("0k", ByteSuffix::Optional), 0 );
    ASSERT_EQ( Quantity::parse("0b", ByteSuffix::Optional), 0 );
    ASSERT_EQ( Quantity::parse("0KB", ByteSuffix::Optional), 0 );
    ASSERT_EQ( Quantity::parse("2 k", ByteSuffix::Optional), 2 * 1024 );
    ASSERT_EQ( Quantity::parse("2 KB", ByteSuffix::Optional), 2 * 1024 );
}

TEST(Quantity, to_string) {

    ASSERT_EQ( Quantity("1").to_string(), string("1"));
    ASSERT_EQ( Quantity("1 k").to_string(), string("1K"));
    ASSERT_EQ( Quantity("1 m ").to_string(), string("1M"));
    ASSERT_EQ( Quantity("1 G ").to_string(), string("1G"));
    ASSERT_EQ( Quantity("1", true).to_string(), string("1 bytes"));
    ASSERT_EQ( Quantity("1 k", true).to_string(), string("1 KB"));
    ASSERT_EQ( Quantity("1 m ", true).to_string(), string("1 MB"));
    ASSERT_EQ( Quantity("1 G ", true).to_string(), string("1 GB"));
    ASSERT_EQ( Quantity("1242Kb", true).to_string(), string("1.21 MB"));
}


TEST(Quantity, math) {
    Quantity q{0};
    q += 10;
    ASSERT_EQ( q, 10 );
    q *= 1024;
    ASSERT_EQ( q, 10 * 1024);
    q -= 7;
    ASSERT_EQ( q, 10 * 1024 - 7);

    Quantity q2{2};
    Quantity q3{3};
    Quantity q4 = q2 * q3;
    ASSERT_EQ(q4, 6);

    ASSERT_THROW(operator-(q4, 2048), QuantityError);
}
