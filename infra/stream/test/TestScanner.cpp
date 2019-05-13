#include "infra/stream/Scanner.hpp"
#include "gtest/gtest.h"

class ScannerTest : public testing::Test
{
};

TEST_F(ScannerTest, Empty)
{
    const infra::BoundedString::WithStorage<10> string("");
    infra::StringInputStream stream(string);
    stream >> infra::Scanner("");
}

TEST_F(ScannerTest, Simple)
{
    const infra::BoundedString::WithStorage<10>value;
    infra::StringInputStream stream("a");


    stream >> Scanner("{}", value);
}
